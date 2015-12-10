#pragma once

#include <JEngine/SpriteBatch.h>

#include <string>
#include <vector>

const int TILE_WIDTH = 64;

class Level
{
public:
	Level(const std::string& fileName);
	~Level();

	void draw();

	//getters
	int getWidth() const { return _levelData[0].size(); }
	int getHeight() const { return _levelData.size(); }
	int getNumHumans() const { return _numHumans; }
	const std::vector<std::string>& getLevelData() const { return _levelData; }
	glm::vec2 getStartPlayerPosition() const { return _startPlayerPosition; }
	const std::vector<glm::vec2>& getStartZombiePosition() const { return _zombieStartPositions; }


private:
	std::vector<std::string> _levelData;
	int _numHumans;
	JEngine::SpriteBatch _spriteBatch;

	glm::vec2 _startPlayerPosition;
	std::vector<glm::vec2> _zombieStartPositions;
};

