#ifndef PRE_RENDER_H
#define PRE_RENDER_H

namespace Source::Rendering::Pre
{
	// Main Pre-Rendering Function;
	void preRender();

	// Proccess Error Log
	void Error_Log();

	// Calculate Change in Time Since Last Frame
	void getDeltaTime();

	// Calculate FPS, if Enabled
	void getFPS(double current_frame);

	// Calculates Random Numbers
	void xorshift64();

	// Turns a RNG into a Float
	long double randomFloat();
}

#endif
