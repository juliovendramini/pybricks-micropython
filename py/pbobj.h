// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#ifndef PYBRICKS_INCLUDED_PBOBJ_H
#define PYBRICKS_INCLUDED_PBOBJ_H

#include "py/obj.h"
#include <fixmath.h>

// Shortcut for the equivalent of: import name
#define PB_IMPORT_MODULE(name) \
mp_store_global(name, mp_import_name(name, mp_const_none, MP_OBJ_NEW_SMALL_INT(0)))

// Shortcut for the equivalent of: from name import *
#define PB_FROM_MODULE_IMPORT_ALL(name) \
mp_import_all(mp_import_name(name, mp_const_none, MP_OBJ_NEW_SMALL_INT(0)))

// like mp_obj_get_int() but also allows float
#if MICROPY_PY_BUILTINS_FLOAT
mp_int_t pb_obj_get_int(mp_obj_t arg);
#else
#define pb_obj_get_int mp_obj_get_int
#endif

fix16_t pb_obj_get_fix16(mp_obj_t arg);

// Get value if object is not none, else return default
mp_int_t pb_obj_get_default_int(mp_obj_t obj, mp_int_t default_val);

// Get base instance if object is instance of subclass of type
mp_obj_t pb_obj_get_base_class_obj(mp_obj_t obj, const mp_obj_type_t *type);

#endif // PYBRICKS_INCLUDED_PBOBJ_H
