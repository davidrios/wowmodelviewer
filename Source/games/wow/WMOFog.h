#pragma once

#include "glm/glm.hpp"

class GameFile;

struct WMOFog
{
	unsigned int flags;
	glm::vec3 pos;
	float r1; // Smaller radius
	float r2; // Larger radius
	float fogend;
	// This is the distance at which all visibility ceases, and you see no objects or terrain except for the fog color.
	float fogstart;
	// This is where the fog starts. Obtained by multiplying the fog end value by the fog start multiplier. multiplier (0..1)
	unsigned int color1; // The back buffer is also cleared to this colour 
	float f2; // Unknown (almost always 222.222)
	float f3; // Unknown (-1 or -0.5)
	unsigned int color2;
	// read to here (0x30 bytes)
	glm::vec4 color;
	void init(GameFile& f);
	void setup();
};
