#pragma once

#include <JEngine/Window.h>
#include <JEngine/GLSLProgram.h>
#include <JEngine/InputManager.h>
#include <JEngine/Camera2D.h>
#include <JEngine/SpriteBatch.h>
#include <JEngine/SpriteFont.h>
#include <JEngine/Timing.h>
#include <JEngine/JEngine.h>
#include <JEngine/AudioEngine.h>
#include <JEngine/ParticleEngine2D.h>
#include <JEngine/ParticleBatch2D.h>

#include "Level.h"
#include "Bullet.h"
#include "Player.h"

class Zombie;

enum class GameState {PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();

private:
	void initSystems();
	void initLevel();
	void initShaders();
	void gameLoop();
	void updateAgents(float deltaTime);
	void updateBullets(float deltaTime);
	void checkVictory();
	void processInput();
	void drawGame();
	void drawHud();
	void addBlood(const glm::vec2& position, int numParticles);

	JEngine::Window _window;
	JEngine::GLSLProgram _textureProgram;
	JEngine::InputManager _inputManager;
	JEngine::Camera2D _camera;
	JEngine::Camera2D _hudCamera;
	JEngine::SpriteBatch _spriteBatch;
	JEngine::SpriteBatch _hudSpriteBatch;
	JEngine::SpriteFont* _spriteFont;
	JEngine::FPSLimiter _fpsLimiter;
	JEngine::AudioEngine _audioEngine;
	JEngine::ParticleEngine2D _particleEngine;
	JEngine::ParticleBatch2D* _bloodParticleBatch;

	Player* _player;
	std::vector<Human*> _humans;
	std::vector<Zombie*> _zombies;
	std::vector<Bullet> _bullets;
	std::vector<Level*> _levels;

	GameState _gameState;
	int _screenWidth;
	int _screenHeight;
	int _currentLevel;

	int _numHumansKilled;
	int _numZombiesKilled;
	
	float _time;
	float _fps;
	float _maxFPS;
};

