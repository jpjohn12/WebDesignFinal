#pragma once

#include <string>

#include <SDL/SDL.h>
#include <GL/glew.h>

enum WindowFlags {INVISIBLE = 0x1, FULLSCREEN = 0x2, BORDERLESS = 0x4};
namespace JEngine {
	class Window
	{
	public:
		Window();
		~Window();
		int create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlag);
		int getScreenWidth() { return _screenWidth; }
		int getScreenHeight() { return _screenHeight; }
		void swapBuffer();

	private:
		SDL_Window* _sdlWindow;
		int _screenWidth, _screenHeight;
	};

}