# SPDX-License-Identifier: MIT
# Copyright (c) 2018-2020 The Pybricks Authors

"""Enum classes of parameters, used by modules in the Pybricks package."""

from parameters_c import Direction, Stop, Color, Button, Port

# Expose ev3dev media through pybricks.parameters
# for backwards compatibility with EV3 MicroPython 1.0
from .media.ev3dev import ImageFile, SoundFile


class Align():
    BOTTOM_LEFT = 1
    BOTTOM = 2
    BOTTOM_RIGHT = 3
    LEFT = 4
    CENTER = 5
    RIGHT = 6
    TOP_LEFT = 7
    TOP = 8
    TOP_RIGHT = 9
