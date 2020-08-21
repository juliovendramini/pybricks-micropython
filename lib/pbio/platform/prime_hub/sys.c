// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#include <string.h>

#include "pbio/color.h"
#include "pbio/event.h"
#include "pbio/light.h"
#include "pbio/lightgrid.h"
#include "pbio/motorpoll.h"

#include <pbsys/hmi.h>
#include <pbsys/status.h>
#include <pbsys/sys.h>

#include "sys/clock.h"
#include "sys/etimer.h"
#include "sys/process.h"

// user program stop function
static pbsys_stop_callback_t user_stop_func;
// user program stdin event function
static pbsys_stdin_event_callback_t user_stdin_event_func;

PROCESS(pbsys_process, "System");

void pbsys_prepare_user_program(const pbsys_user_program_callbacks_t *callbacks) {
    if (callbacks) {
        user_stop_func = callbacks->stop;
        user_stdin_event_func = callbacks->stdin_event;
    } else {
        user_stop_func = NULL;
        user_stdin_event_func = NULL;
    }
    pbio_light_on_with_pattern(PBIO_PORT_SELF, PBIO_COLOR_GREEN, PBIO_LIGHT_PATTERN_BREATHE);

    pbio_lightgrid_t *lightgrid;
    if (pbio_lightgrid_get_dev(&lightgrid) == PBIO_SUCCESS) {
        pbio_lightgrid_start_pattern(lightgrid, pbio_lightgrid_sys_pattern, 40, 25);
    }
    pbsys_status_set(PBSYS_STATUS_USER_PROGRAM_RUNNING);
}

void pbsys_unprepare_user_program(void) {
    pbsys_status_clear(PBSYS_STATUS_USER_PROGRAM_RUNNING);
    user_stop_func = NULL;
    user_stdin_event_func = NULL;
    _pbio_motorpoll_reset_all();

    pbio_lightgrid_t *lightgrid;
    if (pbio_lightgrid_get_dev(&lightgrid) == PBIO_SUCCESS) {
        pbio_lightgrid_stop_pattern(lightgrid);
        uint8_t rows[5] = {0};
        // 3x3 "stop sign" at top center of light grid
        rows[0] = rows[1] = rows[2] = 0b01110;
        pbio_lightgrid_set_rows(lightgrid, rows);
    }
}

PROCESS_THREAD(pbsys_process, ev, data) {
    static struct etimer timer;

    PROCESS_BEGIN();

    etimer_set(&timer, clock_from_msec(50));

    while (true) {
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_TIMER && etimer_expired(&timer)) {
            etimer_reset(&timer);
            pbsys_hmi_poll();
        } else if (ev == PBIO_EVENT_STATUS_SET) {
            if ((pbsys_status_t)data == PBSYS_STATUS_POWER_BUTTON_PRESSED && user_stop_func) {
                user_stop_func();
            }
        } else if (ev == PBIO_EVENT_COM_CMD) {
            pbio_com_cmd_t cmd = (uint32_t)data;

            switch (cmd) {
                case PBIO_COM_CMD_START_USER_PROGRAM:
                    break;
                case PBIO_COM_CMD_STOP_USER_PROGRAM:
                    if (user_stop_func) {
                        user_stop_func();
                    }
                    break;
            }
        }
    }

    PROCESS_END();
}
