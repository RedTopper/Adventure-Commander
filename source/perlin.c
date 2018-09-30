#include <wchar.h>
#include <stdint.h>

#include "point.h"
#include "perlin.h"

//"Public" functions

float noise2D(Point dim, Point pos, const float* seed, int octaves, float bias) {
	float noise = 0.0f;
	float scaleAcc = 0.0f;
	float scale = 1.0f;

	for (int o = 0; o < octaves; o++) {
		Point sample1;
		Point sample2;
		
		int pitch = dim.x >> o;
		sample1.x = (pos.x / pitch) * pitch;
		sample1.y = (pos.y / pitch) * pitch;
		
		sample2.x = (sample1.x + pitch) % dim.x;
		sample2.y = (sample1.y + pitch) % dim.y;

		float blendX = (float)(pos.x - sample1.x) / (float)pitch;
		float blendY = (float)(pos.y - sample1.y) / (float)pitch;

		float sampleT = (1.0f - blendX) * seed[sample1.y * dim.x + sample1.x] + blendX * seed[sample1.y * dim.x + sample2.x];
		float sampleB = (1.0f - blendX) * seed[sample2.y * dim.x + sample1.x] + blendX * seed[sample2.y * dim.x + sample2.x];

		scaleAcc += scale;
		noise += (blendY * (sampleB - sampleT) + sampleT) * scale;
		scale = scale / bias;
	}

	// Scale to seed range
	return noise / scaleAcc;
}
