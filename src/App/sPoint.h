#pragma once

#include <Core/Math/Vector3.h>
#include <Core/Math/Vector4.h>

class cNode;

struct sPoint
{
	wv::cVector3d position;
	wv::cVector3d velocity;
	wv::cVector3d last_position;
	double mass;
	cNode* node;
};

struct sRenderPoint
{
	wv::cVector3d position;
	wv::cVector4d color;
};