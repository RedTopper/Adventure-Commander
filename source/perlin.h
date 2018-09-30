#pragma once

#include "point.h"

/**
 * A Perlin noise generator in 2D
 *
 * Prerequisites: 
 * float* seed must be a pointer to an array of side dim.x * dim.y
 * float bias must be greater that zero
 *
 * Author: Javidx9
 * License: GNU GPLv3 https://github.com/OneLoneCoder/videos/blob/master/LICENSE
 */
float noise2D(Point dim, Point pos, const float* seed, int octaves, float bias);
