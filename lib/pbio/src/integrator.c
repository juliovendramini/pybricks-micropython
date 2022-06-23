// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2022 The Pybricks Authors

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <pbio/config.h>
#include <pbio/math.h>
#include <pbio/trajectory.h>
#include <pbio/integrator.h>

/* Speed integrator used for speed-based control */

void pbio_speed_integrator_pause(pbio_speed_integrator_t *itg, uint32_t time_now, int32_t position_error) {

    // Pause only if running
    if (!itg->running) {
        return;
    }

    // The integrator is not running anymore
    itg->running = false;

    // Increment the paused integrator state with the integrated amount between the last resume and the newly enforced pause
    itg->speed_err_integral_paused += position_error - itg->position_error_resumed;

    // Store time at which we started pausing
    itg->time_pause_begin = time_now;
}

void pbio_speed_integrator_resume(pbio_speed_integrator_t *itg, uint32_t time_now, int32_t position_error) {

    // Resume only if paused
    if (itg->running) {
        return;
    }

    // The integrator is running again
    itg->running = true;

    // Set starting point from which we resume, if needed
    // Begin integrating again from the current point
    itg->position_error_resumed = position_error;
}

void pbio_speed_integrator_reset(pbio_speed_integrator_t *itg, uint32_t time_now, int32_t position_error) {

    // Set integral to 0
    itg->speed_err_integral_paused = 0;

    // Set state to paused
    itg->running = false;

    // Resume integration
    pbio_speed_integrator_resume(itg, time_now, position_error);
}

// Get reference errors and integrals
int32_t pbio_speed_integrator_get_error(pbio_speed_integrator_t *itg, int32_t position_error) {

    // The speed error integral is at least the value at which we paused it last
    int32_t speed_err_integral = itg->speed_err_integral_paused;

    // If integrator is active, add the exact integral since its last restart
    if (itg->running) {
        speed_err_integral += position_error - itg->position_error_resumed;
    }
    return speed_err_integral;
}

bool pbio_speed_integrator_stalled(pbio_speed_integrator_t *itg, uint32_t time_now, int32_t speed_now, int32_t speed_ref, uint32_t time_stall, int32_t speed_stall) {
    // If were running, we're not stalled
    if (itg->running) {
        return false;
    }

    // Equivalent to checking both directions, flip to positive for simplicity.
    if (speed_ref < 0) {
        speed_ref *= -1;
        speed_now *= -1;
    }

    // If we're still running faster than the stall limit, we're certainly not stalled.
    if (speed_ref != 0 && speed_now > speed_stall) {
        return false;
    }

    // If the integrator is paused for less than the stall time, we're still not stalled for now.
    if (time_now - itg->time_pause_begin < time_stall) {
        return false;
    }

    // All checks have failed, so we are stalled
    return true;
}

/* Count integrator used for position-based control */

uint32_t pbio_position_integrator_get_ref_time(pbio_position_integrator_t *itg, uint32_t time_now) {
    // The wall time at which we are is either the current time, or whenever we stopped last.
    uint32_t real_time = itg->trajectory_running ? time_now : itg->time_pause_begin;

    // But we want to evaluate the reference compensating for the time we spent waiting.
    return real_time - itg->time_paused_total;
}

void pbio_position_integrator_pause(pbio_position_integrator_t *itg, uint32_t time_now) {

    // Return if already paused
    if (!itg->trajectory_running) {
        return;
    }

    // Disable the integrator
    itg->trajectory_running = false;
    itg->time_pause_begin = time_now;
}

void pbio_position_integrator_resume(pbio_position_integrator_t *itg, uint32_t time_now) {

    // Return if already trajectory_running
    if (itg->trajectory_running) {
        return;
    }

    // Then we must restart the time
    itg->trajectory_running = true;

    // Increment total wait time by time elapsed since we started pausing
    itg->time_paused_total += time_now - itg->time_pause_begin;
}

void pbio_position_integrator_reset(pbio_position_integrator_t *itg, uint32_t time_now) {

    // Reset integrator state variables
    itg->count_err_integral = 0;
    itg->time_paused_total = 0;
    itg->time_pause_begin = time_now;
    itg->count_err_prev = 0;
    itg->trajectory_running = false;

    // Resume integration
    pbio_position_integrator_resume(itg, time_now);

}

int32_t pbio_position_integrator_update(pbio_position_integrator_t *itg, uint32_t time_now, int32_t position_error, int32_t position_remaining, int32_t integral_range, int32_t integral_max, int32_t integral_change_max) {

    // Previous error will be multiplied by time delta and then added to integral (unless we limit growth)
    int32_t cerr = itg->count_err_prev;

    // Check if integrator magnitude would decrease due to this error
    bool decrease = abs(itg->count_err_integral + pbio_integrator_times_loop_time(cerr)) < abs(itg->count_err_integral);

    // Integrate and update position error
    if (itg->trajectory_running || decrease) {

        // If not deceasing, so growing, limit error growth by maximum integral rate
        if (!decrease) {
            cerr = cerr > integral_change_max ?  integral_change_max : cerr;
            cerr = cerr < -integral_change_max ? -integral_change_max : cerr;

            // It might be decreasing now after all (due to integral sign change), so re-evaluate
            decrease = abs(itg->count_err_integral + pbio_integrator_times_loop_time(cerr)) < abs(itg->count_err_integral);
        }

        // Add change if we are near target, or always if it decreases the integral magnitude
        if (abs(position_remaining) <= integral_range || decrease) {
            itg->count_err_integral += pbio_integrator_times_loop_time(cerr);
        }

        // Limit integral to predefined bound
        if (itg->count_err_integral > integral_max) {
            itg->count_err_integral = integral_max;
        }
        if (itg->count_err_integral < -integral_max) {
            itg->count_err_integral = -integral_max;
        }
    }

    // Keep the error for use in next update
    itg->count_err_prev = position_error;

    return itg->count_err_integral;
}

bool pbio_position_integrator_stalled(pbio_position_integrator_t *itg, uint32_t time_now, int32_t speed_now, int32_t speed_ref, uint32_t time_stall, int32_t speed_stall, int32_t integral_max) {
    // If we're running and the integrator is not saturated, we're not stalled
    if (itg->trajectory_running && abs(itg->count_err_integral) < integral_max) {
        return false;
    }

    // Equivalent to checking both directions, flip to positive for simplicity.
    if (speed_ref < 0) {
        speed_ref *= -1;
        speed_now *= -1;
    }

    // If we're still running faster than the stall limit, we're certainly not stalled.
    if (speed_ref != 0 && speed_now > speed_stall) {
        return false;
    }

    // If the integrator is paused for less than the stall time, we're still not stalled for now.
    if (time_now - itg->time_pause_begin < time_stall) {
        return false;
    }

    // All checks have failed, so we are stalled
    return true;
};
