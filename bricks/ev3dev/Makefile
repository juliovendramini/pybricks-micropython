# SPDX-License-Identifier: MIT
# Copyright (c) 2013, 2014 Damien P. George

# Sanity check
include ../check.mk

# Environment
-include mpconfigport.mk
include ../../../../py/mkenv.mk

FROZEN_DIR = scripts
FROZEN_MPY_DIR = modules
QSTR_GLOBAL_DEPENDENCIES = brickconfig.h

# define main target
PROG = pybricks-micropython

# qstr definitions (must come before including py.mk)
#QSTR_DEFS = qstrdefsport.h

# OS name, for simple autoconfig
UNAME_S := $(shell uname -s)

# include py core make definitions
include $(TOP)/py/py.mk

INC +=  -I.
INC +=  -I$(TOP)
INC += -I$(BUILD)
INC += -I$(TOP)/ports/pybricks/lib/contiki-core
INC += -I$(TOP)/ports/pybricks/lib/ev3dev
INC += -I$(TOP)/ports/pybricks/lib/ev3dev/include
INC += -I$(TOP)/ports/pybricks/lib/lego
INC += -I$(TOP)/ports/pybricks/lib/libfixmath/libfixmath
INC += -I$(TOP)/ports/pybricks/lib/pbio
INC += -I$(TOP)/ports/pybricks/lib/pbio/include
INC += -I$(TOP)/ports/pybricks/lib/pbio/platform/ev3dev_stretch
INC += -I$(TOP)/ports/pybricks/extmod
INC += -I$(TOP)/ports/pybricks/py
INC += -I$(TOP)/ports/unix

# compiler settings
CWARN = -Wall -Werror
CWARN += -Wpointer-arith -Wuninitialized
CFLAGS = $(INC) $(CWARN) -std=gnu99 -DUNIX $(CFLAGS_MOD) $(COPT) $(CFLAGS_EXTRA)

# Debugging/Optimization
ifdef DEBUG
CFLAGS += -g
COPT = -O0
else
COPT = -Os -fdata-sections -ffunction-sections -DNDEBUG
# _FORTIFY_SOURCE is a feature in gcc/glibc which is intended to provide extra
# security for detecting buffer overflows. Some distros (Ubuntu at the very least)
# have it enabled by default.
#
# gcc already optimizes some printf calls to call puts and/or putchar. When
# _FORTIFY_SOURCE is enabled and compiling with -O1 or greater, then some
# printf calls will also be optimized to call __printf_chk (in glibc). Any
# printfs which get redirected to __printf_chk are then no longer synchronized
# with printfs that go through mp_printf.
#
# In MicroPython, we don't want to use the runtime library's printf but rather
# go through mp_printf, so that stdout is properly tied into streams, etc.
# This means that we either need to turn off _FORTIFY_SOURCE or provide our
# own implementation of __printf_chk. We've chosen to turn off _FORTIFY_SOURCE.
# It should also be noted that the use of printf in MicroPython is typically
# quite limited anyways (primarily for debug and some error reporting, etc
# in the unix version).
#
# Information about _FORTIFY_SOURCE seems to be rather scarce. The best I could
# find was this: https://securityblog.redhat.com/2014/03/26/fortify-and-you/
# Original patchset was introduced by
# https://gcc.gnu.org/ml/gcc-patches/2004-09/msg02055.html .
#
# Turning off _FORTIFY_SOURCE is only required when compiling with -O1 or greater
CFLAGS += -U _FORTIFY_SOURCE
endif

# On OSX, 'gcc' is a symlink to clang unless a real gcc is installed.
# The unix port of MicroPython on OSX must be compiled with clang,
# while cross-compile ports require gcc, so we test here for OSX and
# if necessary override the value of 'CC' set in py/mkenv.mk
ifeq ($(UNAME_S),Darwin)
ifeq ($(MICROPY_FORCE_32BIT),1)
CC = clang -m32
else
CC = clang
endif
# Use clang syntax for map file
LDFLAGS_ARCH = -Wl,-map,$@.map -Wl,-dead_strip
else
# Use gcc syntax for map file
LDFLAGS_ARCH = -Wl,-Map=$@.map,--cref -Wl,--gc-sections
endif
LDFLAGS = $(LDFLAGS_MOD) $(LDFLAGS_ARCH) -lm $(LDFLAGS_EXTRA)

# Flags to link with pthread library
LDFLAGS += -lpthread

ifeq ($(MICROPY_USE_READLINE),1)
INC +=  -I$(TOP)/lib/mp-readline
CFLAGS_MOD += -DMICROPY_USE_READLINE=1
LIB_SRC_C_EXTRA += mp-readline/readline.c
endif
ifeq ($(MICROPY_PY_TERMIOS),1)
CFLAGS_MOD += -DMICROPY_PY_TERMIOS=1
SRC_MOD += ports/unix/modtermios.c
endif
ifeq ($(MICROPY_PY_SOCKET),1)
CFLAGS_MOD += -DMICROPY_PY_SOCKET=1
SRC_MOD += ports/unix/modusocket.c
endif

ifeq ($(MICROPY_PY_FFI),1)
LIBFFI_CFLAGS_MOD := $(shell pkg-config --cflags libffi)
LIBFFI_LDFLAGS_MOD := $(shell pkg-config --libs libffi)

ifeq ($(UNAME_S),Linux)
LIBFFI_LDFLAGS_MOD += -ldl
endif

CFLAGS_MOD += $(LIBFFI_CFLAGS_MOD) -DMICROPY_PY_FFI=1
LDFLAGS_MOD += $(LIBFFI_LDFLAGS_MOD)
SRC_MOD += ports/unix/modffi.c
endif

ifeq ($(MICROPY_PY_JNI),1)
# Path for 64-bit OpenJDK, should be adjusted for other JDKs
CFLAGS_MOD += -I/usr/lib/jvm/java-7-openjdk-amd64/include -DMICROPY_PY_JNI=1
SRC_MOD += ports/unix/modjni.c
endif

CFLAGS_MOD += $(shell pkg-config --cflags libudev)
LDFLAGS_MOD += $(shell pkg-config --libs libudev)

CFLAGS_MOD += $(shell pkg-config --cflags grx-3.0)
LDFLAGS_MOD += $(shell pkg-config --libs grx-3.0)

# for pbsmbus
ifneq ($(shell $(CC) -print-file-name=libi2c.a),libi2c.a)
# in i2ctools v4, there is an acutal library and the header file has moved
CFLAGS_MOD += -DPB_HAVE_LIBI2C=1
LDFLAGS_MOD += -li2c
endif

# source files
SRC_C = $(addprefix ports/unix/,\
	main.c \
	gccollect.c \
	mpthreadport.c \
	input.c \
	file.c \
	modmachine.c \
	modos.c \
	modmmap.c \
	moduos_vfs.c \
	modtime.c \
	modufcntl.c \
	moduselect.c \
	alloc.c \
	coverage.c \
	)

SRC_C += $(SRC_MOD)

# Pybricks port core source files
PYBRICKS_SRC_C += \
	ev3dev_mphal.c \
	modbluetooth.c \
	modusignal.c \
	pb_type_ev3dev_font.c \
	pb_type_ev3dev_image.c \
	pb_type_ev3dev_speaker.c \
	pbdevice.c \
	pbinit.c \
	pbsmbus.c \

LIB_SRC_C = $(addprefix lib/,\
	$(LIB_SRC_C_EXTRA) \
	timeutils/timeutils.c \
	utils/pyexec.c \
	)

# Pybricks drivers and modules
PYBRICKS_EXTMOD_SRC_C = $(addprefix ports/pybricks/extmod/,\
	modbattery.c \
	modbuiltins.c \
	modbuttons.c \
	modev3devices.c \
	modexperimental.c \
	modhubs.c \
	modiodevices.c \
	modlogger.c \
	modmedia_ev3dev.c \
	modmotor.c \
	modnxtdevices.c \
	modparameters.c \
	modrobotics.c \
	modtools.c \
	)

PYBRICKS_PY_SRC_C = $(addprefix ports/pybricks/py/,\
	pb_type_enum.c \
	pberror.c \
	pbobj.c \
	pbthread.c \
	)

PYBRICKS_LIB_SRC_C = $(addprefix ports/pybricks/lib/,\
	contiki-core/sys/autostart.c \
	contiki-core/sys/etimer.c \
	contiki-core/sys/process.c \
	contiki-core/sys/timer.c \
	ev3dev/src/ev3dev_stretch/lego_port.c \
	ev3dev/src/ev3dev_stretch/lego_sensor.c \
	ev3dev/src/ev3dev_stretch/sysfs.c \
	ev3dev/src/ev3dev_stretch/nxtcolor.c \
	libfixmath/libfixmath/fix16_sqrt.c \
	libfixmath/libfixmath/fix16_str.c \
	libfixmath/libfixmath/fix16.c \
	libfixmath/libfixmath/uint32.c \
	pbio/drv/battery/battery_linux_ev3.c \
	pbio/drv/button/button_linux_ev3.c \
	pbio/drv/counter/counter_core.c \
	pbio/drv/counter/counter_ev3dev_stretch_iio.c \
	pbio/drv/ev3dev_stretch/light.c \
	pbio/drv/ev3dev_stretch/motor.c \
	pbio/drv/ev3dev_stretch/serial.c \
	pbio/drv/ioport/ioport_ev3dev_stretch.c \
	pbio/platform/ev3dev_stretch/clock.c \
	pbio/src/control.c \
	pbio/src/drivebase.c \
	pbio/src/error.c \
	pbio/src/dcmotor.c \
	pbio/src/light.c \
	pbio/src/logger.c \
	pbio/src/main.c \
	pbio/src/math.c \
	pbio/src/motorpoll.c \
	pbio/src/serial.c \
	pbio/src/servo.c \
	pbio/src/sound.c \
	pbio/src/tacho.c \
	pbio/src/trajectory.c \
	pbio/src/trajectory_ext.c \
	pbio/src/integrator.c \
	)

OBJ = $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PYBRICKS_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PYBRICKS_PY_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PYBRICKS_EXTMOD_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(PYBRICKS_LIB_SRC_C:.c=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(LIB_SRC_C) $(PYBRICKS_SRC_C) $(PYBRICKS_PY_SRC_C) $(PYBRICKS_EXTMOD_SRC_C)
# Append any auto-generated sources that are needed by sources listed in
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

ifneq ($(FROZEN_MPY_DIR),)
# To use frozen bytecode, put your .py files in a subdirectory (eg frozen/) and
# then invoke make with FROZEN_MPY_DIR=frozen (be sure to build from scratch).
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
CFLAGS += -DMPZ_DIG_SIZE=16 # force 16 bits to work on both 32 and 64 bit archs
MPY_CROSS_FLAGS += -mcache-lookup-bc
endif


include $(TOP)/py/mkrules.mk

.PHONY: test

EV3DEV_TEST_DIRS = $(addprefix ../ports/pybricks/tests/ev3dev/, \
	brick \
	experimental \
	media \
	messaging \
	motor \
	parameters \
	uev3dev \
	v1 \
	)

GRX_TEST_PLUGIN_OBJ := $(BUILD)/grx-plugin.o
GRX_TEST_PLUGIN_LIB := $(BUILD)/libgrx-3.0-vdriver-test.so

$(GRX_TEST_PLUGIN_OBJ): ../../tests/ev3dev/grx-plugin.c
	$(Q)$(CC) -c -fPIC -o $@ $^ $(CFLAGS)

$(GRX_TEST_PLUGIN_LIB): $(GRX_TEST_PLUGIN_OBJ)
	$(Q)$(CC) -shared -o $@ $^ $(LDFLAGS)

test-ev3dev: $(PROG) $(TOP)/tests/run-tests $(GRX_TEST_PLUGIN_LIB)
	cd $(TOP)/tests && MICROPY_MICROPYTHON="../ports/pybricks/tests/ev3dev/test-wrapper.sh" \
		GRX_PLUGIN_PATH=$(realpath $(BUILD)) GRX_DRIVER=test \
		./run-tests --test-dirs $(EV3DEV_TEST_DIRS)

test: $(PROG) $(TOP)/tests/run-tests $(GRX_TEST_PLUGIN_LIB)
	$(eval DIRNAME=ports/pybricks/bricks/$(notdir $(CURDIR)))
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../$(DIRNAME)/$(PROG) \
		GRX_PLUGIN_PATH=$(realpath $(BUILD)) GRX_DRIVER=test \
		./run-tests

# install micropython in /usr/local/bin
PREFIX = /usr/local
BINDIR = $(DESTDIR)$(PREFIX)/bin

install: $(PROG)
	install -d $(BINDIR)
	install $(PROG) $(BINDIR)/$(PROG)

# uninstall micropython
uninstall:
	-rm $(BINDIR)/$(PROG)

# Value of configure's --host= option (required for cross-compilation).
# Deduce it from CROSS_COMPILE by default, but can be overridden.
ifneq ($(CROSS_COMPILE),)
CROSS_COMPILE_HOST = --host=$(patsubst %-,%,$(CROSS_COMPILE))
else
CROSS_COMPILE_HOST =
endif

deplibs: axtls

axtls: $(TOP)/lib/axtls/README

$(TOP)/lib/axtls/README:
	@echo "You cloned without --recursive, fetching submodules for you."
	(cd $(TOP); git submodule update --init --recursive)
