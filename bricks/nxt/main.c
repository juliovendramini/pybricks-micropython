// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <pbio/main.h>
#include <pbio/servo.h>
#include <pbio/button.h>
#include <pbio/main.h>
#include <pbio/light.h>
#include <pbsys/sys.h>

#include "pbobj.h"

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/persistentcode.h"
#include "lib/utils/pyexec.h"
#include "lib/utils/interrupt_char.h"

#include "py/mphal.h"

#include <nxt/display.h>
#include <nxt/maininit.h>

// FIXME: Decide whether or not to pre-allocate
// memory for logging instead or just disable
// logging altogether on some constrained ports.
void *malloc(size_t n) {
    return m_malloc(n);
}
void free(void *p) {
    m_free(p);
}

static char *stack_top;
#if MICROPY_ENABLE_GC
static char heap[PYBRICKS_HEAP_KB * 1024];
#endif


// Receive single character
int mp_hal_stdin_rx_chr(void) {
    unsigned char c = 0;
    return c;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    display_string(str);
    display_update();
}

// callback for when stop button is pressed
static void user_program_stop_func(void) {
    // we can only raise an exception if the VM is running
    // mp_interrupt_char will be either -1 or 0 when VM is not running
    if (mp_interrupt_char > 0) {
        mp_keyboard_interrupt();
    }
}

static bool user_program_stdin_event_func(uint8_t c) {
    if (c == mp_interrupt_char) {
        mp_keyboard_interrupt();
        return true;
    }

    return false;
}

static const pbsys_user_program_callbacks_t user_program_callbacks = {
    .stop           = user_program_stop_func,
    .stdin_event    = user_program_stdin_event_func,
};

static void pb_imports() {
    // Import port-specific hub module
    // PB_IMPORT_MODULE(PYBRICKS_HUB_NAME);

    // Import generic modules
    #if PYBRICKS_PY_PARAMETERS
    PB_FROM_MODULE_IMPORT_ALL(MP_QSTR_parameters);
    #endif
    #if PYBRICKS_PY_TOOLS
    PB_FROM_MODULE_IMPORT_ALL(MP_QSTR_tools);
    #endif
    #if PYBRICKS_PY_ROBOTICS
    PB_FROM_MODULE_IMPORT_ALL(MP_QSTR_robotics);
    #endif
    #if PYBRICKS_PY_NXTDEVICES
    PB_FROM_MODULE_IMPORT_ALL(MP_QSTR_nxtdevices);
    #endif
}

int main(int argc, char **argv) {
    int stack_dummy;
    stack_top = (char*)&stack_dummy;

    pbio_init();
    nxt_init();

    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + sizeof(heap));
    #endif

    // (re)boot message
    mp_print_str(&mp_plat_print, "Pybricks\n");

    // Get system hardware ready
    pbsys_prepare_user_program(&user_program_callbacks);

    // Initialize MicroPython and run default imports
    mp_init();
    pb_imports();

    // Run a program
    pyexec_frozen_module("frozen.py");

    // Uninitialize MicroPython and the system hardware
    mp_deinit();
    pbsys_unprepare_user_program();

    pbio_deinit();
    nxt_deinit();
    return 0;
}

// defined in linker script
extern uint32_t _estack;
// defined in lib/utils/gchelper_arm7tdmi.s
extern uintptr_t gc_helper_get_regs_and_sp(uintptr_t *regs);

void gc_collect(void) {
    // start the GC
    gc_collect_start();

    // get the registers and the sp
    uintptr_t regs[10];
    uintptr_t sp = gc_helper_get_regs_and_sp(regs);

    // trace the stack, including the registers (since they live on the stack in this function)
    gc_collect_root((void**)sp, ((uint32_t)&_estack - sp) / sizeof(uint32_t));

    // end the GC
    gc_collect_end();

    // for debug during development
    gc_dump_info();
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

void nlr_jump_fail(void *val) {
    while (1);
}

void NORETURN __fatal_error(const char *msg) {
    while (1);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
