#pragma once

#include <glm/glm.hpp>
#include <JEngine/SpriteBatch.h>

class Bullet
{
public:
	Bullet(glm::vec2 position, glm::vec2 direction, float speed, int lifeTime);
	~Bullet();
	
	void draw(JEngine::SpriteBatch& spriteBatch);
	bool update();

private:
	float _speed;
	int _lifeTime;
	glm::vec2 _direction;
	glm::vec2 _position;

};

