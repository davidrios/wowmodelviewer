#pragma once

#include "types.h"

#include "glm/glm.hpp"

class WoWModel;

#ifdef _WIN32
#    ifdef BUILDING_WOW_DLL
#        define _MODELRENDERPASS_API_ __declspec(dllexport)
#    else
#        define _MODELRENDERPASS_API_ __declspec(dllimport)
#    endif
#else
#    define _MODELRENDERPASS_API_
#endif

class _MODELRENDERPASS_API_ ModelRenderPass
{
public:
	ModelRenderPass(WoWModel*, int geo);

	bool useTex2, useEnvMap, cull, trans, unlit, noZWrite, billboard;

	int16 texanim, color, opacity, blendmode, specialTex;
	uint16 tex;

	// texture wrapping
	bool swrap, twrap;

	// colours
	glm::vec4 ocol, ecol;

	WoWModel* model;

	int geoIndex;

	bool init();
	int BlendValueForMode(int mode);

	void render(bool animated);

	void deinit();

	static const uint16 INVALID_TEX = 50000;

	/*
	  bool operator< (const ModelRenderPass &m) const
	  {
	    // Probably not 100% right, but seems to work better than just geoset sorting.
	    // Blend mode mostly takes into account transparency and material - Wain
	    if (trans == m.trans)
	    {
	      if (blendmode == m.blendmode)
	        return (geoIndex < m.geoIndex);
	      return blendmode < m.blendmode;
	    }
	    return (trans < m.trans);
	  }
	*/
};
