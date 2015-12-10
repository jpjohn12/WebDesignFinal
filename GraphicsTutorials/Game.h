#pragma once

#include <JEngine/JEngine.h>
#include <JEngine/Sprite.h>
#include <JEngine/Errors.h>
#include <JEngine/GLSLProgram.h>
#include <JEngine/GLTexture.h>
#include <JEngine/Window.h>
#include <JEngine/Camera2D.h>
#include <JEngine/SpriteBatch.h>
#include <JEngine/InputManager.h>
#include <JEngine/Timing.h>
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <vector>

#include "Bullet.h"

enum class GameState {PLAY, EXIT};

class Game
{
public:
	Game();
	~Game();
	void run();
	

private:
	void init();
	void initShaders();
	void gameLoop();
	void processInput();
	void drawGame();

	JEngine::Window _window;
	int _screenWidth;
	int _screenHeight;
	GameState _gameState;

	float _time;
	float _fps;
	float _maxFPS;
	std::vector<Bullet> _bullets;


	JEngine::GLSLProgram _colorShaderProgram;
	JEngine::Camera2D _camera;
	JEngine::SpriteBatch _spriteBatch;
	JEngine::InputManager _inputManager;
	JEngine::FPSLimiter _fpsLimiter;
	
};

