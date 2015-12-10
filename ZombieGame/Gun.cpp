#include "Gun.h"

#include <glm/gtx/rotate_vector.hpp>
#include <random>
#include <ctime>


Gun::Gun(std::string name, int fireRate, int bulletsPerShot,
	float spread, float bulletSpeed, float bulletDamage, JEngine::SoundEffect fireEffect) :
	_name(name),
	_fireRate(fireRate),
	_bulletsPerShot(bulletsPerShot),
	_spread(spread),
	_bulletSpeed(bulletSpeed),
	_bulletDamage(bulletDamage),
	_frameCounter(0),
	_fireEffect(fireEffect)
{

}


Gun::~Gun()
{
}

void Gun::update(bool isMouseDown, const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets, float deltaTime)
{
	_frameCounter += 1.0f * deltaTime;
	if (_frameCounter >= _fireRate && isMouseDown)
	{
		fire(position, direction, bullets);
		_frameCounter = 0;
	}

}

void Gun::fire(const glm::vec2& position, const glm::vec2& direction, std::vector<Bullet>& bullets)
{
	static std::mt19937 randomEngine((size_t)time(nullptr));
	std::uniform_real_distribution<float> randRotate(-_spread, _spread);

	_fireEffect.play();

	for (int i = 0; i < _bulletsPerShot; i++)
	{
		bullets.emplace_back(position, glm::rotate(direction, randRotate(randomEngine) * float(3.14/180)), _bulletDamage, _bulletSpeed);
	}
}