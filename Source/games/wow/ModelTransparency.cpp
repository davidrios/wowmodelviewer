#include "ModelTransparency.h"

void ModelTransparency::init(GameFile* f, ModelTransDef& mcd, std::vector<uint32>& global)
{
	trans.init(mcd.trans, f, global);
}
