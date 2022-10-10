// Several Functions that Preform Mathmatical Operations

// Calculates the Time Between Each Frame
void DeltaTime()
{
	// Get Age of Program
	double currentFrame = glfwGetTime() - framebyframeOffset;

	// Calculate Difference Between Frames
	deltaTime = (float)(currentFrame - lastFrame);
	lastFrame = currentFrame;

	// Calculate FPS if Neccessary
	if (editor_options.FPS_debug)
		FPS(currentFrame);

	// Update RNG
	xorshift64(&rng);
	Random();
}

// Calculates Random Numbers
uint64_t xorshift64(struct xorshift64_state* state)
{
	uint64_t x = state->number;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	state->number = x;
	return x * 0x2545F4914F6CDD1DULL;
}

// Turns a RNG into a Float
long double Random()
{
	long double decimal_rng = rng.number * 0.0000000000000000001;
	decimal_rng -= (floor(decimal_rng) == 1);
	xorshift64(&rng);

	return decimal_rng;
}

// Round Number to Specified Decimal Point
void roundPrecision(float& number, int precision)
{
	// The Constant Used for Calculating Precision
	float exactPrecision = (float)pow(10, precision);

	// Multiply by exactPrecision Constant
	number *= exactPrecision;

	// Round Number
	number = round(number);

	// Divide by exactPrecision Constant
	number /= exactPrecision;
}

// Calculate FPS
void FPS(double currentTime)
{
	// Increment Frame Count
	frameCount++;

	// For Every Second That Passes Update FPS
	if (currentTime > lastTime + 1.0)
	{
		std::cout << frameCount << " FPS\n" << " " << frame_by_frame << " " << frame_step << " " << frame_run << " \n";

		// Reset FPS Values
		last_frameCount = frameCount;
		frameCount = 0;
		lastTime = currentTime;
	}
}

// Displays FPS on Screen
void display_FPS()
{
	// Draw FPS if Enabled
	if (editor_options.FPS_display)
	{
		fontShader.Use();
		AdvancedRenderText(std::to_string(last_frameCount), 80.0f, 94.0f, 100.0f, 0.2f, glm::vec3(0.0f, 0.0f, 1.0f), false);
	}

}

// Calculates the Magnitude Squared of a Vector 2
float magnitude_squared(glm::vec2 vector)
{
	return vector.x * vector.x + vector.y * vector.y;
}

// Calculates the Magnitude Squared of a Vector 3
float magnitude_squared(glm::vec3 vector)
{
	return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

// Calculates the Magnitude of a Vector 2
float magnitude(glm::vec2 vector)
{
	return glm::sqrt(vector.x * vector.x + vector.y * vector.y);
}

// Calculates the Magnitude of a Vector 3
float magnitude(glm::vec3 vector)
{
	return glm::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

// Calculates the Angle of a Vector
float angle_from_vector(glm::vec2 vector)
{
	return angle_from_vector(vector.x, vector.y);
}

// Calculates the Angle of a Vector
float angle_from_vector(float x, float y)
{
	static float angle;

	// Get initial angle
	angle = atan(y / x);
	if (x < 0)
		angle += 3.14159f;

	return angle;
}

// Takes an Index in the level_list and Returns the Level Name
glm::vec2 index_to_level(int index)
{
	return levels_list[index].Coords;
}

// Takes a Level Name and Returns its Index in level_list. Returns -1 if Index is out-of-bounds
int level_to_index(glm::vec2 name)
{
	if (abs(name.x - camera.level.x) > 1 || abs(name.y - camera.level.y) > 1) { return -1; }
	return (4 + (int)floor(name.x - camera.level.x) - ((name.y - camera.level.y) * 3));
}

// Allows For Frame By Frame of Program
void Frame_By_Frame(GLFWwindow* window, Player &player, Camera &camera)
{
	frame_step = false;
	glfwSetKeyCallback(window, FrameByFrameCallback);
	framebyframeStart = glfwGetTime();
	while (frame_by_frame && !(frame_step || frame_run || glfwWindowShouldClose(window)))
	{
		glfwPollEvents();
		SmoothKeyCallback_Gameplay(camera, player);
	}
	glfwSetTime(framebyframeStart);
	glfwSetKeyCallback(window, KeyCallback);
}