#pragma once

#include <string>

#include "GLTexture.h"

namespace JEngine {
	class ImageLoader
	{
	public:
		static GLTexture loadPNG(std::string filePath);
	};
}

