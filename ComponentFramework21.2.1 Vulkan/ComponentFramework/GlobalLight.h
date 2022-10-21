#pragma once

#include "Matrix.h"
using namespace MATH;

struct GlobalLightUBO {
	Vec4 lightPos[4];
	Vec4 lightColour[4];
};