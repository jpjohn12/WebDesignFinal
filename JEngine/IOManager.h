#pragma once

#include <vector>

namespace JEngine {
	class IOManager
	{
	public:
		static bool readFileToBuffer(std::string filePath, std::vector<unsigned char>& buffer);
	};

}