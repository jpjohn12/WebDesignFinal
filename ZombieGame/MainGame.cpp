#include "MainGame.h"
#include "Zombie.h"
#include "Gun.h"

#include <JEngine/ResourceManager.h>
#include <JEngine/Errors.h>

#include <glm/gtx/rotate_vector.hpp>

#include <ctime>
#include <random>
#include <iostream>
#include <algorithm>

const float PLAYER_SPEED = 5.0f;
const float HUMAN_SPEED = 1.0f;
const float ZOMBIE_SPEED = 1.3f;

MainGame::MainGame() : _screenWidth(1024), 
					   _screenHeight(600), 
					   _gameState(GameState::PLAY), 
					   _time(0.0f), 
					   _maxFPS(60.0f), 
					   _player(nullptr),
					   _numHumansKilled(0),
					   _numZombiesKilled(0)
{
	_camera.init(_screenWidth, _screenHeight);
	_hudCamera.init(_screenWidth, _screenHeight);
	_hudCamera.setPosition(glm::vec2(_screenWidth / 2, _screenHeight / 2));
}


MainGame::~MainGame()
{
	for (size_t i = 0; i < _levels.size(); i++)
	{
		delete _levels[i];
	}

	for (size_t i = 0; i < _humans.size(); i++)
	{
		delete _humans[i];
	}

	for (size_t i = 0; i < _zombies.size(); i++)
	{
		delete _zombies[i];
	}
}

void MainGame::run()
{
	initSystems();
	initLevel();

	JEngine::Music music = _audioEngine.loadMusic("Sound/XYZ.ogg");
	music.play(-1);

	gameLoop();
}

void MainGame::initSystems()
{
	const float CAMERA_SCALE = 1.0f / 3.0f;
	_camera.setScale(CAMERA_SCALE);

	JEngine::init();
	_audioEngine.init();
	_window.create("Zombie Game", _screenWidth, _screenHeight, 0);
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	initShaders();
	
	_spriteBatch.init();
	_hudSpriteBatch.init();

	_spriteFont = new JEngine::SpriteFont("Fonts/chintzy.ttf", 64);

	_fpsLimiter.init(_maxFPS);
	_fpsLimiter.setMaxFPS(_maxFPS);

	_bloodParticleBatch = new JEngine::ParticleBatch2D();
	_bloodParticleBatch->init(1000, 0.05f, JEngine::ResourceManager::getTexture("Textures/particle.png"),
		[](JEngine::Particle2D& particle, float deltaTime) 
	{
		particle.position += particle.velocity * deltaTime;
		particle.color.a = GLubyte(particle.life * 255.0f);
	});
	_particleEngine.addParticleBatch(_bloodParticleBatch);
}

void MainGame::initLevel()
{
	_levels.push_back(new Level("Levels/level1.txt"));
	_currentLevel = 0;

	_player = new Player();
	_player->init(PLAYER_SPEED, _levels[_currentLevel]->getStartPlayerPosition(), &_inputManager, _camera, &_bullets);

	_humans.push_back(_player);

	std::mt19937 randomEngine;
	randomEngine.seed((unsigned long)time(nullptr));
	std::uniform_int_distribution<int> randX(2, _levels[_currentLevel]->getWidth() - 2);
	std::uniform_int_distribution<int> randY(2, _levels[_currentLevel]->getHeight() - 2);

	for (int i = 0; i < _levels[_currentLevel]->getNumHumans(); i++)
	{
		_humans.push_back(new Human);
		glm::vec2 pos(randX(randomEngine) * TILE_WIDTH, randY(randomEngine) * TILE_WIDTH);
		_humans.back()->init(HUMAN_SPEED, pos);
	}

	const std::vector<glm::vec2>& zombiePositions = _levels[_currentLevel]->getStartZombiePosition();
	for (size_t i = 0; i < zombiePositions.size(); i++)
	{
		_zombies.push_back(new Zombie);
		_zombies.back()->init(ZOMBIE_SPEED, zombiePositions[i]);
	}

	const float BULLET_SPEED = 20.0f;
	_player->addGun(new Gun("Magnum", 10, 1, 5.0f, 30, BULLET_SPEED, _audioEngine.loadSoundEffect("Sound/shots/pistol.wav")));
	_player->addGun(new Gun("Shotgun", 30, 12, 20.0f, 4, BULLET_SPEED, _audioEngine.loadSoundEffect("Sound/shots/shotgun.wav")));
	_player->addGun(new Gun("MP5", 2, 1, 10.0f, 20, BULLET_SPEED, _audioEngine.loadSoundEffect("Sound/shots/cg1.wav")));


}

void MainGame::initShaders()
{
	_textureProgram.compileShaders("Shaders/ColorShading.vert", "Shaders/ColorShading.frag");
	_textureProgram.addAttribute("vertexPosition");
	_textureProgram.addAttribute("vertexColor");
	_textureProgram.addAttribute("vertexUV");
	_textureProgram.linkShaders();
}

void MainGame::gameLoop()
{
	const float DESIRED_FPS = 60.0f;
	const float MS_PER_SECOND = 1000;
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
	const float MAX_DELTA_TIME = 1.0f;
	const int MAX_PHYSICS_STEPS = 6;

	Uint32 previousTicks = SDL_GetTicks();

	while (_gameState != GameState::EXIT)
	{
		Uint32 newTicks = SDL_GetTicks();
		Uint32 frameTime = newTicks - previousTicks;
		previousTicks = newTicks;
		float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

		checkVictory();

		_inputManager.update();

		processInput();

		int i = 0;
		while (totalDeltaTime > 0.0f && i < MAX_PHYSICS_STEPS)
		{
			float deltaTime = std::min(totalDeltaTime, MAX_DELTA_TIME);
			updateAgents(deltaTime);
			updateBullets(deltaTime);
			_particleEngine.update(deltaTime);
			totalDeltaTime -= deltaTime;
			i++;
		}

		_camera.setPosition(_player->getPosition());
		
		_fpsLimiter.begin();
		_time += 0.01f;
		_camera.update();
		_hudCamera.update();

		drawGame();

		_fps = _fpsLimiter.end();
		std::cout << _fps << std::endl;
	}
}

void MainGame::updateAgents(float deltaTime)
{
	for (size_t i = 0; i < _humans.size(); i++)
	{
		_humans[i]->update(_levels[_currentLevel]->getLevelData(), _humans, _zombies, deltaTime);
	}

	for (size_t i = 0; i < _zombies.size(); i++)
	{
		_zombies[i]->update(_levels[_currentLevel]->getLevelData(), _humans, _zombies, deltaTime);
	}

	for (size_t i = 0; i < _zombies.size(); i++)
	{
		for (size_t j = i + 1; j < _zombies.size(); j++)
		{
			_zombies[i]->collideWithAgent(_zombies[j]);
		}
		for (size_t j = 1; j < _humans.size(); j++)
		{
			if (_zombies[i]->collideWithAgent(_humans[j]))
			{
				_zombies.push_back(new Zombie);
				_zombies.back()->init(ZOMBIE_SPEED, _humans[j]->getPosition());

				delete _humans[j];
				_humans[j] = _humans.back();
				_humans.pop_back();
			}
		}
		
		if (_zombies[i]->collideWithAgent(_player))
		{
			JEngine::fatalError("YOU LOSE");
		}
	}

	for (size_t i = 0; i < _humans.size(); i++)
	{
		for (size_t j = i + 1; j < _humans.size(); j++)
		{
			_humans[i]->collideWithAgent(_humans[j]);
		}
	}

}

void MainGame::updateBullets(float deltaTime)
{
	for (size_t i = 0; i < _bullets.size();)
	{
		if (_bullets[i].update(_levels[_currentLevel]->getLevelData(), deltaTime))
		{
			_bullets[i] = _bullets.back();
			_bullets.pop_back();
		}
		else
		{
			i++;
		}
	}

	for (size_t i = 0; i < _bullets.size(); i++)
	{
		bool wasBulletRemoved = false;

		for (size_t j = 0; j < _zombies.size();)
		{
			if (_bullets[i].collideWithAgent(_zombies[j]))
			{
				addBlood(_bullets[i].getPosition(), 5);

				if (_zombies[j]->applyDamage(_bullets[i].getDamage()))
				{
					delete _zombies[j];
					_zombies[j] = _zombies.back();
					_zombies.pop_back();
					_numZombiesKilled++;
				}
				else
				{
					j++;
				}

				_bullets[i] = _bullets.back();
				_bullets.pop_back();
				wasBulletRemoved = true;
				break;
			}
			else
			{
				j++;
			}
		}
		if (wasBulletRemoved == false)
		{
			for (size_t j = 1; j < _humans.size();)
			{
				if (_bullets[i].collideWithAgent(_humans[j]))
				{
					addBlood(_bullets[i].getPosition(), 5);

					if (_humans[j]->applyDamage(_bullets[i].getDamage()))
					{
						delete _humans[j];
						_humans[j] = _humans.back();
						_humans.pop_back();
						_numHumansKilled++;
					}
					else
					{
						j++;
					}

					_bullets[i] = _bullets.back();
					_bullets.pop_back();
					wasBulletRemoved = true;
					break;
				}
				else
				{
					j++;
				}
			}
		}
	}
}

void MainGame::checkVictory()
{
	if (_zombies.empty())
	{
		std::printf("*** You win! ***\n You killed %d humans and %d zombies. There are %d out of %d humans remaining", _numHumansKilled, _numZombiesKilled, _humans.size() - 1, _levels[_currentLevel]->getNumHumans());

		JEngine::fatalError("");
	}
}

void MainGame::processInput()
{
	SDL_Event evnt;
	const float CAMERA_SPEED = 10.0f;
	const float SCALE_SPEED = 0.1f;
	while (SDL_PollEvent(&evnt))
	{
		switch (evnt.type)
		{
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			_inputManager.setMouseCoords((float)evnt.motion.x, (float)evnt.motion.y);
			break;
		case SDL_KEYDOWN:
			_inputManager.pressKey(evnt.key.keysym.sym);
			break;
		case SDL_KEYUP:
			_inputManager.releaseKey(evnt.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			_inputManager.pressKey(evnt.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			_inputManager.releaseKey(evnt.button.button);
			break;
		}
	}
}

void MainGame::drawGame()
{
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_textureProgram.use();

	glActiveTexture(GL_TEXTURE0);

	GLint textureUniform = _textureProgram.getUniformLocation("mySampler");
	glUniform1i(textureUniform, 0);

	glm::mat4 projectionMatrix = _camera.getCameraMatrix();
	GLint pUniform = _textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	_levels[_currentLevel]->draw();

	_spriteBatch.begin();

	glm::vec2 agentDimensions(AGENT_RADIUS * 2.0f);

	for (size_t i = 0; i < _humans.size(); i++)
	{
		if (_camera.isBoxInView(_humans[i]->getPosition(), agentDimensions)) 
		{
			_humans[i]->draw(_spriteBatch);
		}
	}

	for (size_t i = 0; i < _zombies.size(); i++)
	{
		if (_camera.isBoxInView(_zombies[i]->getPosition(), agentDimensions))
		{
			_zombies[i]->draw(_spriteBatch);
		}
	}

	for (size_t i = 0; i < _bullets.size(); i++)
	{
		_bullets[i].draw(_spriteBatch);
	}

	_spriteBatch.end();

	_spriteBatch.renderBatch();

	_particleEngine.draw(&_spriteBatch);

	drawHud();

	_textureProgram.unuse();

	_window.swapBuffer();
}

void MainGame::drawHud()
{
	char buffer[256];

	glm::mat4 projectionMatrix = _hudCamera.getCameraMatrix();
	GLint pUniform = _textureProgram.getUniformLocation("P");
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, &projectionMatrix[0][0]);

	_hudSpriteBatch.begin();

	sprintf_s(buffer, "Num Humans %d", _humans.size());
	_spriteFont->draw(_hudSpriteBatch, buffer, glm::vec2(0, 0), glm::vec2(0.5f), 0.0f, JEngine::Color(255, 255, 255, 255));

	sprintf_s(buffer, "Num Zombies %d", _zombies.size());
	_spriteFont->draw(_hudSpriteBatch, buffer, glm::vec2(0, 36), glm::vec2(0.5f), 0.0f, JEngine::Color(255, 255, 255, 255));

	_hudSpriteBatch.end();

	_hudSpriteBatch.renderBatch();
}

void MainGame::addBlood(const glm::vec2& position, int numParticles)
{
	static std::mt19937 randEngine((size_t)time(nullptr));
	static std::uniform_real_distribution<float> randAngle(0.0f, 360);

	glm::vec2 velocity(2.0f, 0.0f);
	JEngine::Color color(255, 0, 0, 255);

	for (int i = 0; i < numParticles; i++)
	{
		_bloodParticleBatch->addParticle(position, glm::rotate(velocity, randAngle(randEngine)), color, 30.0f);
	}
}