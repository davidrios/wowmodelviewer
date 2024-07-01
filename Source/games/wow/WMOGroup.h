#pragma once

#include <string>

#include "types.h"
#include "GL/glew.h"
#include "glm/glm.hpp"

class GameFile;
class WMO;

struct WMOBatch
{
	unsigned short a[2 * 3]; // indices? a box? (-2,-2,-1,2,2,3 in cameron)
	unsigned int indexStart;
	unsigned short indexCount, vertexStart, vertexEnd;
	unsigned char flags, texture;
};

struct WMOVertColor
{
	uint8 b, g, r, a;
};

class WMOGroup
{
	WMO* wmo;
	int flags;
	GLuint dl, dl_light;
	glm::vec3 center;
	float rad;
	int num;
	int fog;
	size_t nDoodads;
	short* ddr;
	//Liquid *lq;
public:
	glm::vec3 *vertices, *normals;
	glm::vec2* texcoords;
	uint16* indices;
	uint16* materials;
	uint32 nTriangles, nVertices, nIndices, nBatches;
	unsigned int* cv;
	WMOBatch* batches;
	WMOVertColor* VertexColors;
	uint32* IndiceToVerts;

	glm::vec3 v1, v2;
	glm::vec3 b1, b2;
	glm::vec3 vmin, vmax;
	bool indoor, hascv, visible, ok;

	bool outdoorLights;
	std::wstring name, desc;

	WMOGroup();
	~WMOGroup();
	void init(WMO* wmo, GameFile& f, int num, char* names);
	void initDisplayList();
	void initLighting(int nLR, short* useLights);
	void draw();
	void drawLiquid();
	void drawDoodads(int doodadset);
	void setupFog();
	void cleanup();

	void updateModels(bool load);
};
