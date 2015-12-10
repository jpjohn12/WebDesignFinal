#include "Agent.h"
#include "Level.h"

#include <algorithm>
#include <JEngine/ResourceManager.h>

Agent::Agent()
{
}


Agent::~Agent()
{
}

bool Agent::collideWithLevel(const std::vector<std::string>& levelData)
{
	std::vector<glm::vec2> collideTilePositions;
	
	checkTilePosition(collideTilePositions, levelData, _position.x, _position.y);

	checkTilePosition(collideTilePositions, levelData, _position.x + AGENT_WIDTH, _position.y);

	checkTilePosition(collideTilePositions, levelData, _position.x, _position.y + AGENT_WIDTH);

	checkTilePosition(collideTilePositions, levelData, _position.x + AGENT_WIDTH, _position.y + AGENT_WIDTH);

	if (collideTilePositions.size() == 0)
	{
		return false;
	}

	for (size_t i = 0; i < collideTilePositions.size(); i++)
	{
		collideWithTile(collideTilePositions[i]);
	}

	return true;
}

bool Agent::collideWithAgent(Agent* agent)
{
	const int MIN_DISTANCE = (int)AGENT_RADIUS * 2;
	
	glm::vec2 centerPosA = _position + glm::vec2(AGENT_RADIUS);
	glm::vec2 centerPosB = agent->getPosition() + glm::vec2(AGENT_RADIUS);

	glm::vec2 distVec = centerPosA - centerPosB;

	float distance = glm::length(distVec);

	float collisionDepth = MIN_DISTANCE - distance;

	if (collisionDepth > 0)
	{
		glm::vec2 collisionDepthVec = glm::normalize(distVec) * collisionDepth;

		_position += collisionDepthVec / 2.0f;
		agent->_position -= distVec / 2.0f;

		return true;
	}
	return false;
}

bool Agent::applyDamage(float damage)
{
	_health -= damage;
	if (_health <= 0)
	{
		return true;
	}
	return false;
}

void Agent::draw(JEngine::SpriteBatch& spriteBatch)
{
	const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec4 destRect(_position.x, _position.y, AGENT_WIDTH, AGENT_WIDTH);
	
	spriteBatch.draw(destRect, uvRect, _textureID, 0.0f, _color, _direction);
}

void Agent::checkTilePosition(std::vector<glm::vec2>& collideTilePositions,
	const std::vector<std::string>& levelData,
	float x, float y)
{
	glm::vec2 cornerPos = glm::vec2(floor(x / (float)TILE_WIDTH),
		floor(y / (float)TILE_WIDTH));

	if (cornerPos.x < 0 || cornerPos.x >= levelData[0].length() ||
		cornerPos.y < 0 || cornerPos.y >= levelData.size())
	{
		return;
	}

	if (levelData[(size_t)cornerPos.y][(size_t)cornerPos.x] != '.')
	{
		collideTilePositions.push_back(cornerPos * (float)TILE_WIDTH + glm::vec2(TILE_WIDTH / 2.0f));
	}
}

void Agent::collideWithTile(glm::vec2 tilePosition)
{
	const float TILE_RADIUS = (float)TILE_WIDTH / 2.0f;
	const float MIN_DISTANCE = AGENT_RADIUS + TILE_RADIUS;

	glm::vec2 centerPlayerPos = _position + glm::vec2(AGENT_RADIUS);
	glm::vec2 distVec = centerPlayerPos - tilePosition;
	float xDepth = MIN_DISTANCE - abs(distVec.x);
	float yDepth = MIN_DISTANCE -abs( distVec.y);

	if (xDepth > 0 && yDepth > 0)
	{
		if (std::max(xDepth, 0.0f) < std::max(yDepth, 0.0f))
		{
			if (distVec.x < 0)
			{
				_position.x -= xDepth;
			}
			else
			{
				_position.x += xDepth;
			}
		}
		else
		{
			if (distVec.y < 0)
			{
				_position.y -= yDepth;
			}
			else
			{
				_position.y += yDepth;
			}
		}
	}

}