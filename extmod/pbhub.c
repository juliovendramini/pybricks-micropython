// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#include "py/mpconfig.h"

#include <pbsys/sys.h>

#include "py/obj.h"

#include "pbhub.h"

STATIC mp_obj_t hub_shutdown(void) {
    pbsys_power_off();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(hub_shutdown_obj, hub_shutdown);

STATIC mp_obj_t hub_reboot() {
    pbsys_reboot(0);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(hub_reboot_obj, hub_reboot);

STATIC mp_obj_t hub_update() {
    pbsys_reboot(1);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(hub_update_obj, hub_update);

