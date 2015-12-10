#include "Bullet.h"
#include <JEngine/ResourceManager.h>
#include <JEngine/GLTexture.h>


Bullet::Bullet(glm::vec2 position, glm::vec2 direction, float speed, int lifeTime)
{
	_position = position;
	_direction = direction;
	_speed = speed;
	_lifeTime = lifeTime;
}


Bullet::~Bullet()
{
}

void Bullet::draw(JEngine::SpriteBatch& spriteBatch)
{
	glm::vec4 uv(0.0f, 0.0f, 1.0f, 1.0f);
	static JEngine::GLTexture texture = JEngine::ResourceManager::getTexture("Textures/Jimmy Jump pack/PNG/CharacterRight_Standing.png");
	JEngine::Color color;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;

	glm::vec4 posAndSize = glm::vec4(_position.x, _position.y, 30, 30);

	spriteBatch.draw(posAndSize, uv, texture.id, 0.0f, color);
}

bool Bullet::update()
{
	_position += _direction * _speed;
	_lifeTime--;
	if (_lifeTime <= 0)
	{
		return true;
	}
	return false;
}
