#pragma once

namespace JEngine {

class FPSLimiter
{
public:
	FPSLimiter();
	void init(float targetFPS);

	void begin();
	void setMaxFPS(float targetFPS);
	float end(); //returns current FPS

private:
	void calculateFPS();

	float _fps;
	float _frameTime;
	float _maxFPS;
	unsigned int _startTicks;
};

}