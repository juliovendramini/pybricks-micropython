// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#include "stm32f030xc.h"

#define MICROPY_HW_BOARD_NAME           "Powered Up Smart Hub"
#define MICROPY_HW_MCU_NAME             "STM32F030RC"

#define PYBRICKS_HEAP_KB                16 // half of RAM

#define PYBRICKS_HUB_CITYHUB            (1)

// Pybricks modules
#define PYBRICKS_PY_IODEVICES           (1)
#define PYBRICKS_PY_PARAMETERS          (1)
#define PYBRICKS_PY_PUPDEVICES          (1)
#define PYBRICKS_PY_TOOLS               (1)
#define PYBRICKS_PY_ROBOTICS            (1)

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
// Requires about 19K (19568) of flash
#define MICROPY_ENABLE_COMPILER         (0)

// Set to MICROPY_FLOAT_IMPL_FLOAT to enable floating point support in user code or
// set to MICROPY_FLOAT_IMPL_NONE to disable floating point support in user code
// Requires about 20K (21312) of flash
#define MICROPY_FLOAT_IMPL              (MICROPY_FLOAT_IMPL_NONE)

#include "../stm32/configport.h"
