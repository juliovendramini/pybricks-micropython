// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#include "py/mphal.h"
#include "py/runtime.h"
#include "pberror.h"
#include "pbobj.h"
#include "pbkwarg.h"

STATIC mp_obj_t tools_wait(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    PB_PARSE_ARGS_FUNCTION(n_args, pos_args, kw_args,
        PB_ARG_REQUIRED(time)
    );
    mp_int_t duration = pb_obj_get_int(time);
    if (duration > 0) {
        mp_hal_delay_ms(duration);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(tools_wait_obj, 0, tools_wait);

// Class structure for StopWatch
typedef struct _tools_StopWatch_obj_t {
    mp_obj_base_t base;
    uint32_t time_start;
    uint32_t time_stop;
    uint32_t time_spent_pausing;
    bool running;
} tools_StopWatch_obj_t;

STATIC mp_obj_t tools_StopWatch_reset(mp_obj_t self_in) {
    tools_StopWatch_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->time_start = mp_hal_ticks_ms();
    self->time_stop = self->time_start;
    self->time_spent_pausing = 0;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tools_StopWatch_reset_obj, tools_StopWatch_reset);

STATIC mp_obj_t tools_StopWatch_time(mp_obj_t self_in) {
    tools_StopWatch_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(
        self->running ?
        mp_hal_ticks_ms()- self->time_start - self->time_spent_pausing :
        self->time_stop  - self->time_start - self->time_spent_pausing
    );
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tools_StopWatch_time_obj, tools_StopWatch_time);

STATIC mp_obj_t tools_StopWatch_pause(mp_obj_t self_in) {
    tools_StopWatch_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->running) {
        self->running = false;
        self->time_stop = mp_hal_ticks_ms();
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tools_StopWatch_pause_obj, tools_StopWatch_pause);

STATIC mp_obj_t tools_StopWatch_resume(mp_obj_t self_in) {
    tools_StopWatch_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->running) {
        self->running = true;
        self->time_spent_pausing += mp_hal_ticks_ms() - self->time_stop;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(tools_StopWatch_resume_obj, tools_StopWatch_resume);

STATIC mp_obj_t tools_StopWatch_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args ) {
    // Initialize self
    tools_StopWatch_obj_t *self = m_new_obj(tools_StopWatch_obj_t);
    self->base.type = (mp_obj_type_t*) type;
    tools_StopWatch_reset(self);
    tools_StopWatch_resume(self);
    return MP_OBJ_FROM_PTR(self);
}

/*
StopWatch class tables
*/
STATIC const mp_rom_map_elem_t tools_StopWatch_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&tools_StopWatch_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&tools_StopWatch_time_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&tools_StopWatch_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&tools_StopWatch_resume_obj) },
};
STATIC MP_DEFINE_CONST_DICT(tools_StopWatch_locals_dict, tools_StopWatch_locals_dict_table);

STATIC const mp_obj_type_t tools_StopWatch_type = {
    { &mp_type_type },
    .name = MP_QSTR_StopWatch,
    .make_new = tools_StopWatch_make_new,
    .locals_dict = (mp_obj_dict_t*)&tools_StopWatch_locals_dict,
};

/*
tools module tables
*/

STATIC const mp_rom_map_elem_t tools_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),    MP_ROM_QSTR(MP_QSTR_tools)         },
    { MP_ROM_QSTR(MP_QSTR_wait),        MP_ROM_PTR(&tools_wait_obj)  },
    { MP_ROM_QSTR(MP_QSTR_StopWatch),   MP_ROM_PTR(&tools_StopWatch_type)  },
};
STATIC MP_DEFINE_CONST_DICT(pb_module_tools_globals, tools_globals_table);

const mp_obj_module_t pb_module_tools = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&pb_module_tools_globals,
};
