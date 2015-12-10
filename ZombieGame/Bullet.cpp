#include "Bullet.h"
#include "Agent.h"
#include "Human.h"
#include "Zombie.h"
#include "Level.h"

#include <JEngine/GLTexture.h>
#include <JEngine/ResourceManager.h>
#include <JEngine/ResourceManager.h>


Bullet::Bullet(glm::vec2 position, glm::vec2 direction, float damage, float speed) :
_position(position),
_direction(direction),
_damage(damage),
_speed(speed)
{
	
}


Bullet::~Bullet()
{
}

bool Bullet::update(const std::vector<std::string>& levelData, float deltaTime)
{
	_position += _direction * _speed * deltaTime;
	return collideWithWorld(levelData);
}

void Bullet::draw(JEngine::SpriteBatch& spriteBatch)
{
	glm::vec4 destRect(_position.x + BULLET_RADIUS, _position.y + BULLET_RADIUS, BULLET_RADIUS * 2, BULLET_RADIUS * 2);
	const glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
	JEngine::Color color;
	color.r = 75;
	color.g = 75;
	color.b = 75;
	color.a = 255;
	spriteBatch.draw(destRect, uvRect, JEngine::ResourceManager::getTexture("Textures/circle.png").id, 0.0f, color);


}

bool Bullet::collideWithAgent(Agent* agent)
{
	const int MIN_DISTANCE = (int)(AGENT_RADIUS + BULLET_RADIUS);

	glm::vec2 centerPosA = _position;
	glm::vec2 centerPosB = agent->getPosition() + glm::vec2(AGENT_RADIUS);

	glm::vec2 distVec = centerPosA - centerPosB;

	float distance = glm::length(distVec);

	float collisionDepth = MIN_DISTANCE - distance;

	if (collisionDepth > 0)
	{
		return true;
	}
	return false;
}

bool Bullet::collideWithWorld(const std::vector<std::string>& levelData)
{
	glm::ivec2 gridPosition;
	gridPosition.x = (int)(floor(_position.x / (float)TILE_WIDTH));
	gridPosition.y = (int)(floor(_position.y / (float)TILE_WIDTH));

	if ((size_t)gridPosition.x < 0 || (size_t)gridPosition.x >= levelData[0].length() ||
		(size_t)gridPosition.y < 0 || (size_t)gridPosition.y >= levelData.size())
	{
		return true;
	}

	return levelData[gridPosition.y][gridPosition.x] != '.';

}