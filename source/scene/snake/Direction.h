#pragma once

#ifdef _MSC_VER
#  include <intrin.h>
#  define __builtin_popcount __popcnt
#endif

#define DIRECTION_RANDOM (Direction) (rand() % 4)
#define DIRECTION_REVERSE(direction) (Direction) ((int) direction ^ 0b11)

#define DIRECTION_IS_DOWN_OR_RIGHT(direction) ((int) direction <= 0b01)
#define DIRECTION_IS_LEFT_OR_UP(direction)    ((int) direction >= 0b10)
#define DIRECTION_IS_RIGHT_OR_LEFT(direction) __builtin_popcount((unsigned int) direction) == 1
#define DIRECTION_IS_DOWN_OR_UP(direction)    __builtin_popcount((unsigned int) direction) == 2

enum class Direction
{
    DOWN  = 0b00,
    RIGHT = 0b01,
    LEFT  = 0b10,
    UP    = 0b11,

    INVALID = 0b100,
};