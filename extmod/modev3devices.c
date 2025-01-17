// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#include "py/mpconfig.h"

#include "modmotor.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "pbdevice.h"
#include "pbobj.h"
#include "pbkwarg.h"
#include "modparameters.h"

#include "py/objtype.h"

#include <pbio/iodev.h>
#include <pbio/button.h>
#include <pberror.h>

// pybricks.ev3devices.TouchSensor class object
typedef struct _ev3devices_TouchSensor_obj_t {
    mp_obj_base_t base;
    pbdevice_t *pbdev;
} ev3devices_TouchSensor_obj_t;

// pybricks.ev3devices.TouchSensor.__init__
STATIC mp_obj_t ev3devices_TouchSensor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args ) {
    PB_PARSE_ARGS_CLASS(n_args, n_kw, args,
        PB_ARG_REQUIRED(port)
    );

    ev3devices_TouchSensor_obj_t *self = m_new_obj(ev3devices_TouchSensor_obj_t);
    self->base.type = (mp_obj_type_t*) type;

    mp_int_t port_num = pb_type_enum_get_value(port, &pb_enum_type_Port);

    self->pbdev = pbdevice_get_device(port_num, PBIO_IODEV_TYPE_ID_EV3_TOUCH_SENSOR);

    return MP_OBJ_FROM_PTR(self);
}

// pybricks.ev3devices.TouchSensor.pressed
STATIC mp_obj_t ev3devices_TouchSensor_pressed(mp_obj_t self_in) {
    ev3devices_TouchSensor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int32_t analog;
    pbdevice_get_values(self->pbdev, PBIO_IODEV_MODE_EV3_TOUCH_SENSOR__TOUCH, &analog);
    return mp_obj_new_bool(analog > 250);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ev3devices_TouchSensor_pressed_obj, ev3devices_TouchSensor_pressed);


// dir(pybricks.ev3devices.TouchSensor)
STATIC const mp_rom_map_elem_t ev3devices_TouchSensor_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_pressed), MP_ROM_PTR(&ev3devices_TouchSensor_pressed_obj) },
};
STATIC MP_DEFINE_CONST_DICT(ev3devices_TouchSensor_locals_dict, ev3devices_TouchSensor_locals_dict_table);

// type(pybricks.ev3devices.TouchSensor)
STATIC const mp_obj_type_t ev3devices_TouchSensor_type = {
    { &mp_type_type },
    .name = MP_QSTR_TouchSensor,
    .make_new = ev3devices_TouchSensor_make_new,
    .locals_dict = (mp_obj_dict_t*)&ev3devices_TouchSensor_locals_dict,
};

// pybricks.ev3devices.InfraredSensor class object
typedef struct _ev3devices_InfraredSensor_obj_t {
    mp_obj_base_t base;
    pbdevice_t *pbdev;
} ev3devices_InfraredSensor_obj_t;

// pybricks.ev3devices.InfraredSensor.__init__
STATIC mp_obj_t ev3devices_InfraredSensor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args ) {
    PB_PARSE_ARGS_CLASS(n_args, n_kw, args,
        PB_ARG_REQUIRED(port)
    );

    ev3devices_InfraredSensor_obj_t *self = m_new_obj(ev3devices_InfraredSensor_obj_t);
    self->base.type = (mp_obj_type_t*) type;

    mp_int_t port_num = pb_type_enum_get_value(port, &pb_enum_type_Port);

    self->pbdev = pbdevice_get_device(port_num, PBIO_IODEV_TYPE_ID_EV3_IR_SENSOR);

    return MP_OBJ_FROM_PTR(self);
}

// pybricks.ev3devices.InfraredSensor.distance
STATIC mp_obj_t ev3devices_InfraredSensor_distance(mp_obj_t self_in) {
    ev3devices_InfraredSensor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int32_t distance;
    pbdevice_get_values(self->pbdev, PBIO_IODEV_MODE_EV3_INFRARED_SENSOR__PROX, &distance);
    return mp_obj_new_int(distance);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ev3devices_InfraredSensor_distance_obj, ev3devices_InfraredSensor_distance);

// pybricks.ev3devices.InfraredSensor.beacon
STATIC mp_obj_t ev3devices_InfraredSensor_beacon(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
        ev3devices_InfraredSensor_obj_t, self,
        PB_ARG_REQUIRED(channel)
    );

    mp_int_t channel_no = pb_obj_get_int(channel);
    if (channel_no < 1 || channel_no > 4) {
        pb_assert(PBIO_ERROR_INVALID_ARG);
    }

    int32_t beacon_data[8];
    pbdevice_get_values(self->pbdev, PBIO_IODEV_MODE_EV3_INFRARED_SENSOR__SEEK, beacon_data);

    mp_int_t heading = beacon_data[channel_no*2-2]*3;
    mp_int_t distance = beacon_data[channel_no*2-1];

    mp_obj_t ret[2];

    if (distance == -128) {
        ret[0] = mp_const_none;
        ret[1] = mp_const_none;
    }
    else {
        ret[0] = mp_obj_new_int(distance);
        ret[1] = mp_obj_new_int(heading);
    }

    return mp_obj_new_tuple(2, ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(ev3devices_InfraredSensor_beacon_obj, 1, ev3devices_InfraredSensor_beacon);

// pybricks.ev3devices.InfraredSensor.buttons
STATIC mp_obj_t ev3devices_InfraredSensor_buttons(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
        ev3devices_InfraredSensor_obj_t, self,
        PB_ARG_REQUIRED(channel)
    );

    mp_int_t channel_no = pb_obj_get_int(channel);
    if (channel_no < 1 || channel_no > 4) {
        pb_assert(PBIO_ERROR_INVALID_ARG);
    }

    int32_t buttons_data[4];
    pbdevice_get_values(self->pbdev, PBIO_IODEV_MODE_EV3_INFRARED_SENSOR__REMOTE, buttons_data);

    mp_int_t encoded = buttons_data[channel_no-1];
    mp_obj_t pressed[2];
    uint8_t len = 0;

    switch(encoded) {
        case 0:
            break;
        case 1:
            pressed[len++] = pb_const_button_left_up;
            break;
        case 2:
            pressed[len++] = pb_const_button_left_down;
            break;
        case 3:
            pressed[len++] = pb_const_button_right_up;
            break;
        case 4:
            pressed[len++] = pb_const_button_right_down;
            break;
        case 5:
            pressed[len++] = pb_const_button_left_up;
            pressed[len++] = pb_const_button_right_up;
            break;
        case 6:
            pressed[len++] = pb_const_button_left_up;
            pressed[len++] = pb_const_button_right_down;
            break;
        case 7:
            pressed[len++] = pb_const_button_left_down;
            pressed[len++] = pb_const_button_right_up;
            break;
        case 8:
            pressed[len++] = pb_const_button_left_down;
            pressed[len++] = pb_const_button_right_down;
            break;
        case 9:
            pressed[len++] = pb_const_button_beacon;
            break;
        case 10:
            pressed[len++] = pb_const_button_left_up;
            pressed[len++] = pb_const_button_left_down;
            break;
        case 11:
            pressed[len++] = pb_const_button_right_up;
            pressed[len++] = pb_const_button_right_down;
            break;
        default:
            pb_assert(PBIO_ERROR_IO);
            break;
    }

    return mp_obj_new_list(len, pressed);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(ev3devices_InfraredSensor_buttons_obj, 1, ev3devices_InfraredSensor_buttons);

// pybricks.ev3devices.InfraredSensor.keypad
STATIC mp_obj_t ev3devices_InfraredSensor_keypad(mp_obj_t self_in) {

    ev3devices_InfraredSensor_obj_t *self = MP_OBJ_TO_PTR(self_in);

    int32_t keypad_data;
    pbdevice_get_values(self->pbdev, PBIO_IODEV_MODE_EV3_INFRARED_SENSOR__REM_A, &keypad_data);

    if (keypad_data == 384) {
        return mp_obj_new_list(0, NULL);
    }

    mp_obj_t pressed[4];
    uint8_t len = 0;

    if (keypad_data & 0x10) {
        pressed[len++] = pb_const_button_left_up;
    }
    if (keypad_data & 0x20) {
        pressed[len++] = pb_const_button_left_down;
    }
    if (keypad_data & 0x40) {
        pressed[len++] = pb_const_button_right_up;
    }
    if (keypad_data & 0x80) {
        pressed[len++] = pb_const_button_right_down;
    }

    return mp_obj_new_list(len, pressed);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ev3devices_InfraredSensor_keypad_obj, ev3devices_InfraredSensor_keypad);

// dir(pybricks.ev3devices.InfraredSensor)
STATIC const mp_rom_map_elem_t ev3devices_InfraredSensor_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_distance), MP_ROM_PTR(&ev3devices_InfraredSensor_distance_obj) },
    { MP_ROM_QSTR(MP_QSTR_beacon),   MP_ROM_PTR(&ev3devices_InfraredSensor_beacon_obj) },
    { MP_ROM_QSTR(MP_QSTR_buttons),  MP_ROM_PTR(&ev3devices_InfraredSensor_buttons_obj) },
    { MP_ROM_QSTR(MP_QSTR_keypad),   MP_ROM_PTR(&ev3devices_InfraredSensor_keypad_obj) },
};
STATIC MP_DEFINE_CONST_DICT(ev3devices_InfraredSensor_locals_dict, ev3devices_InfraredSensor_locals_dict_table);

// type(pybricks.ev3devices.InfraredSensor)
STATIC const mp_obj_type_t ev3devices_InfraredSensor_type = {
    { &mp_type_type },
    .name = MP_QSTR_InfraredSensor,
    .make_new = ev3devices_InfraredSensor_make_new,
    .locals_dict = (mp_obj_dict_t*)&ev3devices_InfraredSensor_locals_dict,
};

// pybricks.ev3devices.ColorSensor class object
typedef struct _ev3devices_ColorSensor_obj_t {
    mp_obj_base_t base;
    pbdevice_t *pbdev;
} ev3devices_ColorSensor_obj_t;

// pybricks.ev3devices.ColorSensor.__init__
STATIC mp_obj_t ev3devices_ColorSensor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args ) {
    PB_PARSE_ARGS_CLASS(n_args, n_kw, args,
        PB_ARG_REQUIRED(port)
    );
    ev3devices_ColorSensor_obj_t *self = m_new_obj(ev3devices_ColorSensor_obj_t);
    self->base.type = (mp_obj_type_t*) type;

    mp_int_t port_num = pb_type_enum_get_value(port, &pb_enum_type_Port);

    self->pbdev = pbdevice_get_device(port_num, PBIO_IODEV_TYPE_ID_EV3_COLOR_SENSOR);

    return MP_OBJ_FROM_PTR(self);
}

// pybricks.ev3devices.ColorSensor.color
STATIC mp_obj_t ev3devices_ColorSensor_color(mp_obj_t self_in) {
    ev3devices_ColorSensor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int32_t color;
    pbdevice_get_values(self->pbdev, PBIO_IODEV_MODE_EV3_COLOR_SENSOR__COLOR, &color);

    switch(color) {
        case 1:
            return pb_const_color_black;
        case 2:
            return pb_const_color_blue;
        case 3:
            return pb_const_color_green;
        case 4:
            return pb_const_color_yellow;
        case 5:
            return pb_const_color_red;
        case 6:
            return pb_const_color_white;
        case 7:
            return pb_const_color_brown;
        default:
            return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ev3devices_ColorSensor_color_obj, ev3devices_ColorSensor_color);

// pybricks.ev3devices.ColorSensor.ambient
STATIC mp_obj_t ev3devices_ColorSensor_ambient(mp_obj_t self_in) {
    ev3devices_ColorSensor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int32_t ambient;
    pbdevice_get_values(self->pbdev, PBIO_IODEV_MODE_EV3_COLOR_SENSOR__AMBIENT, &ambient);
    return mp_obj_new_int(ambient);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ev3devices_ColorSensor_ambient_obj, ev3devices_ColorSensor_ambient);

// pybricks.ev3devices.ColorSensor.reflection
STATIC mp_obj_t ev3devices_ColorSensor_reflection(mp_obj_t self_in) {
    ev3devices_ColorSensor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int32_t reflection;
    pbdevice_get_values(self->pbdev, PBIO_IODEV_MODE_EV3_COLOR_SENSOR__REFLECT, &reflection);
    return mp_obj_new_int(reflection);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ev3devices_ColorSensor_reflection_obj, ev3devices_ColorSensor_reflection);

// pybricks.ev3devices.ColorSensor.rgb
STATIC mp_obj_t ev3devices_ColorSensor_rgb(mp_obj_t self_in) {
    ev3devices_ColorSensor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int32_t rgb[3];
    pbdevice_get_values(self->pbdev, PBIO_IODEV_MODE_EV3_COLOR_SENSOR__RGB_RAW, rgb);
    mp_obj_t tup[3];

    rgb[0] = (0.258*rgb[0])-0.3;
    rgb[1] = (0.280*rgb[1])-0.8;
    rgb[2] = (0.523*rgb[2])-3.7;

    for (uint8_t i = 0; i < 3; i++) {
        rgb[i] = (rgb[i] > 100 ? 100 : rgb[i]);
        rgb[i] = (rgb[i] < 0   ?   0 : rgb[i]);
        tup[i] = mp_obj_new_int(rgb[i]);
    }
    return mp_obj_new_tuple(3, tup);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ev3devices_ColorSensor_rgb_obj, ev3devices_ColorSensor_rgb);

// dir(pybricks.ev3devices.ColorSensor)
STATIC const mp_rom_map_elem_t ev3devices_ColorSensor_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reflection), MP_ROM_PTR(&ev3devices_ColorSensor_reflection_obj) },
    { MP_ROM_QSTR(MP_QSTR_ambient   ), MP_ROM_PTR(&ev3devices_ColorSensor_ambient_obj)    },
    { MP_ROM_QSTR(MP_QSTR_color     ), MP_ROM_PTR(&ev3devices_ColorSensor_color_obj)      },
    { MP_ROM_QSTR(MP_QSTR_rgb       ), MP_ROM_PTR(&ev3devices_ColorSensor_rgb_obj)        },
};
STATIC MP_DEFINE_CONST_DICT(ev3devices_ColorSensor_locals_dict, ev3devices_ColorSensor_locals_dict_table);

// type(pybricks.ev3devices.ColorSensor)
STATIC const mp_obj_type_t ev3devices_ColorSensor_type = {
    { &mp_type_type },
    .name = MP_QSTR_ColorSensor,
    .make_new = ev3devices_ColorSensor_make_new,
    .locals_dict = (mp_obj_dict_t*)&ev3devices_ColorSensor_locals_dict,
};

// pybricks.ev3devices.UltrasonicSensor class object
typedef struct _ev3devices_UltrasonicSensor_obj_t {
    mp_obj_base_t base;
    pbdevice_t *pbdev;
} ev3devices_UltrasonicSensor_obj_t;


// pybricks.ev3devices.UltrasonicSensor.__init__
STATIC mp_obj_t ev3devices_UltrasonicSensor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args ) {
    PB_PARSE_ARGS_CLASS(n_args, n_kw, args,
        PB_ARG_REQUIRED(port)
    );

    ev3devices_UltrasonicSensor_obj_t *self = m_new_obj(ev3devices_UltrasonicSensor_obj_t);
    self->base.type = (mp_obj_type_t*) type;

    mp_int_t port_num = pb_type_enum_get_value(port, &pb_enum_type_Port);

    self->pbdev = pbdevice_get_device(port_num, PBIO_IODEV_TYPE_ID_EV3_ULTRASONIC_SENSOR);

    return MP_OBJ_FROM_PTR(self);
}

// pybricks.ev3devices.UltrasonicSensor.distance
STATIC mp_obj_t ev3devices_UltrasonicSensor_distance(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
        ev3devices_UltrasonicSensor_obj_t, self,
        PB_ARG_DEFAULT_FALSE(silent)
    );

    int32_t distance;
    if (mp_obj_is_true(silent)) {
        pbdevice_get_values(self->pbdev, PBIO_IODEV_MODE_EV3_ULTRASONIC_SENSOR__SI_CM, &distance);
    }
    else {
        pbdevice_get_values(self->pbdev, PBIO_IODEV_MODE_EV3_ULTRASONIC_SENSOR__DIST_CM, &distance);
    }
    return mp_obj_new_int(distance);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(ev3devices_UltrasonicSensor_distance_obj, 1, ev3devices_UltrasonicSensor_distance);

// pybricks.ev3devices.UltrasonicSensor.presence
STATIC mp_obj_t ev3devices_UltrasonicSensor_presence(mp_obj_t self_in) {
    ev3devices_UltrasonicSensor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int32_t presence;
    pbdevice_get_values(self->pbdev, PBIO_IODEV_MODE_EV3_ULTRASONIC_SENSOR__LISTEN, &presence);
    return mp_obj_new_bool(presence);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ev3devices_UltrasonicSensor_presence_obj, ev3devices_UltrasonicSensor_presence);

// dir(pybricks.ev3devices.UltrasonicSensor)
STATIC const mp_rom_map_elem_t ev3devices_UltrasonicSensor_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_distance), MP_ROM_PTR(&ev3devices_UltrasonicSensor_distance_obj) },
    { MP_ROM_QSTR(MP_QSTR_presence), MP_ROM_PTR(&ev3devices_UltrasonicSensor_presence_obj) },
};
STATIC MP_DEFINE_CONST_DICT(ev3devices_UltrasonicSensor_locals_dict, ev3devices_UltrasonicSensor_locals_dict_table);

// type(pybricks.ev3devices.UltrasonicSensor)
STATIC const mp_obj_type_t ev3devices_UltrasonicSensor_type = {
    { &mp_type_type },
    .name = MP_QSTR_UltrasonicSensor,
    .make_new = ev3devices_UltrasonicSensor_make_new,
    .locals_dict = (mp_obj_dict_t*)&ev3devices_UltrasonicSensor_locals_dict,
};

// pybricks.ev3devices.GyroSensor class object
typedef struct _ev3devices_GyroSensor_obj_t {
    mp_obj_base_t base;
    pbio_port_t port; // FIXME: Shouldn't be here
    pbdevice_t *pbdev;
    pbio_direction_t direction;
    mp_int_t offset;
} ev3devices_GyroSensor_obj_t;

// pybricks.ev3devices.GyroSensor (internal) Get new offset  for new reset angle
STATIC mp_int_t ev3devices_GyroSensor_get_angle_offset(pbdevice_t *pbdev, pbio_direction_t direction, mp_int_t new_angle) {
    // Read raw sensor values
    int32_t raw_angle;
    pbdevice_get_values(pbdev, PBIO_IODEV_MODE_EV3_GYRO_SENSOR__ANG, &raw_angle);

    // Get new offset using arguments and raw values
    if (direction == PBIO_DIRECTION_CLOCKWISE) {
        return raw_angle - new_angle;
    }
    else {
        return -raw_angle - new_angle;
    }
}

// pybricks.ev3devices.GyroSensor.__init__
STATIC mp_obj_t ev3devices_GyroSensor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args ) {
    PB_PARSE_ARGS_CLASS(n_args, n_kw, args,
        PB_ARG_REQUIRED(port),
        PB_ARG_DEFAULT_OBJ(direction, pb_Direction_CLOCKWISE_obj)
    );

    ev3devices_GyroSensor_obj_t *self = m_new_obj(ev3devices_GyroSensor_obj_t);
    self->base.type = (mp_obj_type_t*) type;
    self->direction = pb_type_enum_get_value(direction, &pb_enum_type_Direction);

    mp_int_t port_num = pb_type_enum_get_value(port, &pb_enum_type_Port);
    self->port = port_num;

    self->pbdev = pbdevice_get_device(port_num, PBIO_IODEV_TYPE_ID_EV3_GYRO_SENSOR);

    self->offset = ev3devices_GyroSensor_get_angle_offset(self->pbdev, self->direction, 0);
    return MP_OBJ_FROM_PTR(self);
}

// pybricks.ev3devices.GyroSensor.speed
STATIC mp_obj_t ev3devices_GyroSensor_speed(mp_obj_t self_in) {
    ev3devices_GyroSensor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int32_t raw_speed;
    pbdevice_get_values(self->pbdev, PBIO_IODEV_MODE_EV3_GYRO_SENSOR__RATE, &raw_speed);

    // changing modes resets angle to 0
    self->offset = 0;

    if (self->direction == PBIO_DIRECTION_CLOCKWISE) {
        return mp_obj_new_int(raw_speed);
    }
    else {
        return mp_obj_new_int(-raw_speed);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ev3devices_GyroSensor_speed_obj, ev3devices_GyroSensor_speed);

// pybricks.ev3devices.GyroSensor.angle
STATIC mp_obj_t ev3devices_GyroSensor_angle(mp_obj_t self_in) {
    ev3devices_GyroSensor_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int32_t raw_angle;
    pbdevice_get_values(self->pbdev, PBIO_IODEV_MODE_EV3_GYRO_SENSOR__ANG, &raw_angle);

    if (self->direction == PBIO_DIRECTION_CLOCKWISE) {
        return mp_obj_new_int(raw_angle - self->offset);
    }
    else {
        return mp_obj_new_int(-raw_angle - self->offset);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ev3devices_GyroSensor_angle_obj, ev3devices_GyroSensor_angle);

// pybricks.ev3devices.GyroSensor.reset_angle
STATIC mp_obj_t ev3devices_GyroSensor_reset_angle(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    PB_PARSE_ARGS_METHOD(n_args, pos_args, kw_args,
        ev3devices_GyroSensor_obj_t, self,
        PB_ARG_REQUIRED(angle)
    );

    self->offset = ev3devices_GyroSensor_get_angle_offset(self->pbdev, self->direction, pb_obj_get_int(angle));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(ev3devices_GyroSensor_reset_angle_obj, 1, ev3devices_GyroSensor_reset_angle);

// dir(pybricks.ev3devices.GyroSensor)
STATIC const mp_rom_map_elem_t ev3devices_GyroSensor_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_angle),       MP_ROM_PTR(&ev3devices_GyroSensor_angle_obj)       },
    { MP_ROM_QSTR(MP_QSTR_speed),       MP_ROM_PTR(&ev3devices_GyroSensor_speed_obj)       },
    { MP_ROM_QSTR(MP_QSTR_reset_angle), MP_ROM_PTR(&ev3devices_GyroSensor_reset_angle_obj) },
};
STATIC MP_DEFINE_CONST_DICT(ev3devices_GyroSensor_locals_dict, ev3devices_GyroSensor_locals_dict_table);

// type(pybricks.ev3devices.GyroSensor)
STATIC const mp_obj_type_t ev3devices_GyroSensor_type = {
    { &mp_type_type },
    .name = MP_QSTR_GyroSensor,
    .make_new = ev3devices_GyroSensor_make_new,
    .locals_dict = (mp_obj_dict_t*)&ev3devices_GyroSensor_locals_dict,
};

// dir(pybricks.ev3devices)
STATIC const mp_rom_map_elem_t ev3devices_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),         MP_ROM_QSTR(MP_QSTR_ev3devices)              },
    { MP_ROM_QSTR(MP_QSTR_Motor),            MP_ROM_PTR(&motor_Motor_type)                },
    { MP_ROM_QSTR(MP_QSTR_TouchSensor),      MP_ROM_PTR(&ev3devices_TouchSensor_type)     },
    { MP_ROM_QSTR(MP_QSTR_InfraredSensor),   MP_ROM_PTR(&ev3devices_InfraredSensor_type)  },
    { MP_ROM_QSTR(MP_QSTR_ColorSensor),      MP_ROM_PTR(&ev3devices_ColorSensor_type)     },
    { MP_ROM_QSTR(MP_QSTR_UltrasonicSensor), MP_ROM_PTR(&ev3devices_UltrasonicSensor_type)},
    { MP_ROM_QSTR(MP_QSTR_GyroSensor),       MP_ROM_PTR(&ev3devices_GyroSensor_type)      },
};

STATIC MP_DEFINE_CONST_DICT(pb_module_ev3devices_globals, ev3devices_globals_table);
const mp_obj_module_t pb_module_ev3devices = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&pb_module_ev3devices_globals,
};
