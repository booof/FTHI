// Object that Spawns Particle Effects

class Particle_Generator
{
public:

	// Positions of Spawning Box
	float xPos;
	float yPos;
	float zPos;

	// Size of Spawning Box
	float Width;
	float Height;

	// Particle Values
	float MaxLife;
	float Speed;
	float Spread;
	float Size;
	float SpawnWeight;

	// Maximum Number of Particles this Generator Can Have Spawned at any Given Time
	int Maximum_Particles;

	// Number of Particles Currently Active
	int Active_Particles = 0;

	// Indicies of Dead Particles
	int* dead_particles;

	// Particles to Spawn Per Frame
	int particles_per_second;

	// Offset of Object in Unifrom Particle Buffer
	int buffer_offset = 0;

	// Color of Object
	glm::vec4 Colors;

	// Normals of Object
	glm::vec3 Normals;

	// Texture of Object
	Texture texture;
	bool textured = false;
	bool animated = false;
	float* animation_counter;

	// What the Object is Bound to
	int Frequency;
	bool State = false;
	Particle* particles = 0;

	// Determines if the Visualizer Should Be Drawn
	bool Visualizing = true;

	// Vertex Objects of Visulizer for Generator
	GLuint visualizerVAO;
	GLuint visualizerVBO;

	// Editor Data
	bool Clamp = false;
	bool Lock = false;

	// Material
	Material material;

	Particle_Generator(float xpos, float ypos, float zpos, float width, float height, glm::vec4 color, glm::vec3 normals, Texture texture_, Material material_, float life, float speed, float spread, float size, int max_particles, float spawn_weight, int frequency, bool clamp, bool lock)
	{
		// Store Important Variables
		xPos = xpos;
		yPos = ypos;
		zPos = zPos;
		Width = width;
		Height = height;
		MaxLife = life;
		Speed = speed;
		Spread = spread;
		Size = size;
		Maximum_Particles = max_particles;
		SpawnWeight = spawn_weight;
	//	std::cout << SpawnWeight << "\n";
		particles_per_second = Maximum_Particles * (SpawnWeight / 100);
		Colors = color;
		Normals = normals;
		texture = texture_;
		textured = !(texture_ == texture_null);
		material = material_;
		Frequency = frequency;
		State = !frequency;
		particles = new Particle[Maximum_Particles];
		initialized = true;

		if (texture.type == GL_TEXTURE_2D_ARRAY)
		{
			animated = true;
			animation_counter = new float[Maximum_Particles];
		}

		// Editor Data
		Clamp = clamp;
		Lock = lock;

		// Create List of Dead Particles
		dead_particles = new int[Maximum_Particles];
		for (int i = 0; i < Maximum_Particles; i++)
		{
			dead_particles[i] = true;
		}

		// Find Lower Left Corner of Object
		cornerx = xPos - width / 2.0f;
		cornery = yPos - height / 2.0f;

		// Calculate Lower Angle Bound and Angle Range
		float x = (!Normals.x && !Normals.y) ? 1 : Normals.x;
		float default_angle = atan(Normals.y / x);
		if (Normals.x < 0) { default_angle += 3.14159f; }
		angle_lower_bound = default_angle - 3.14159f * spread;
		angle_range = 6.2832f * spread;

		// Create Model Matrix
		model = glm::translate(model, glm::vec3(xPos, yPos, 0.0f));

		// Create Visualizer Object

		// Generate Object
		glGenVertexArrays(1, &visualizerVAO);
		glGenBuffers(1, &visualizerVBO);

		// Bind Object
		glBindVertexArray(visualizerVAO);
		glBindBuffer(GL_ARRAY_BUFFER, visualizerVBO);

		// Generate Vertices
		float vertices[36];
		float* pointer = Rect_GenVertices_Color(0.0f, 0.0f, -5.5f, Width, Height, color);
		for (int i = 0; i < 36; i++)
		{
			vertices[i] = *(pointer + i);
		}

		// Bind Vertices to Object
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Assign Position Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(0));
		glEnableVertexAttribArray(0);

		// Assign Color Vertices
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);

		// Unbind Object
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Create Particle Object

		// Generate Object
		glGenVertexArrays(1, &particleVAO);
		glGenBuffers(1, &particleVBO);

		// Bind Object
		glBindVertexArray(particleVAO);
		glBindBuffer(GL_ARRAY_BUFFER, particleVBO);

		// Object is Colored
		if (!textured)
		{
			// Generate Vertices
			pointer = Rect_GenVertices_Color(0.0f, 0.0f, -5.5f, Size, Size, color);
			for (int i = 0; i < 36; i++)
			{
				vertices[i] = *(pointer + i);
			}

			// Bind Vertices to Object
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// Assign Position Vertices
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(0));
			glEnableVertexAttribArray(0);

			// Assign Color Vertices
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
			glEnableVertexAttribArray(1);
		}

		// Object is Textured
		else
		{
			// Generate Vertices
			float vertices2[30];
			pointer = Rect_GenVertices_Texture(0.0f, 0.0f, -5.5f, Size, Size);
			for (int i = 0; i < 30; i++)
			{
				vertices2[i] = *(pointer + i);
			}

			// Bind Vertices to Object
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

			// Assign Position Vertices
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
			glEnableVertexAttribArray(0);

			// Assign Texture Vertices
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
			glEnableVertexAttribArray(1);
		}

		// Unbind Object
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// Default Object
	Particle_Generator() { Frequency = NULL; }

	// Deconstructor
	~Particle_Generator()
	{
		// Memory Leaks are Bad
		if (initialized)
		{
			delete particles;
			delete dead_particles;
			if (animated) { delete animation_counter; }
		}
	}

	// Add New Particles to List
	void Add_Particles(int index)
	{
		// Create a New Particle Object at Given Index
		particles[index] = Particle();

		// Set Coords to a Random Location Inside Generator Box
		particles[index].Position.x = cornerx + Width * (float)Random();
		particles[index].Position.y = cornery + Height * (float)Random();
		//particles[index].Position.x = 0.0f;
		//particles[index].Position.y = 0.0f;

		// Generate Random Angle of Particle
		float random_angle = angle_lower_bound + angle_range * (float)Random();

		// Transform Angle Into a Vector Based on Speed
		particles[index].Velocity.x = Speed * cos(random_angle);
		particles[index].Velocity.y = Speed * sin(random_angle);

		// Store Life of Particle
		particles[index].Life = MaxLife;

		// Tell Program that Particle is Alive
		dead_particles[index] = false;

		if (animated) { animation_counter[index] = 0; }

		//std::cout << particles[index].Position.x << " " << particles[index].Position.y << "\n";
		//std::cout << Speed << " g\n";
	}

	// Update Particles
	void Update_Particles()
	{
		// Iterate Through All Particles
		for (int i = 0; i < Maximum_Particles; i++)
		{
			// Reference to Current Particle
			Particle& current_particle = particles[i];

			// Update Particle if Still Alive
			if (current_particle.Life > 0)
			{
				// Update Position
				current_particle.Position += (current_particle.Velocity * glm::vec2(deltaTime));

				// Update Life
				current_particle.Life -= (float)deltaTime;

				// Animate
				if (animated)
				{
					Animate(animation_counter[i], Particle_Positions[buffer_offset + i].w, texture);
				}

				// Store Position in Uniform
				Particle_Positions[buffer_offset + i] = glm::vec4(current_particle.Position, animated, Particle_Positions[buffer_offset + i].w);
			}

			// Kill Particle if Life is Empty
			else
			{
				Particle_Positions[buffer_offset + i] = glm::vec4(-99999999.0f, -9999999999.0f, 0.0f, 1.0f);
				dead_particles[i] = true;
			}
		}
	}

	// Draws Particles
	void Draw_Particles()
	{
		// Textured
		if (textured)
		{
			// Send Transformations to Vertex Shader
			particleShaderTexture.Use();
			glUniformMatrix4fv(modelLocParticleTexture, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
			glUniform1f(alphaLocParticleTexture, Colors.w);
			glUniform1i(ParticleOffsetLocTexture, buffer_offset);
			glActiveTexture(texture.index);
			glBindTexture(texture.type, texture.texture);
			glBindVertexArray(particleVAO);
			glDepthMask(false);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, Maximum_Particles);
			glBindVertexArray(0);
			glBindTexture(texture.type, 0);
			glDepthMask(true);
		}

		// Colored
		else
		{
			// Send Transformations to Vertex Shader
			particleShaderColor.Use();
			glUniformMatrix4fv(modelLocParticleColor, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
			glUniform1f(alphaLocParticleColor, Colors.w);
			glUniform1i(ParticleOffsetLocColor, buffer_offset);
			glBindVertexArray(particleVAO);
			//glDepthMask(false);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, Maximum_Particles);
			glBindVertexArray(0);
			//glDepthMask(true);
		}
	}

	// Draws the Visualizer
	void Draw_Visualizer()
	{
		// Send Transformations to Vertex Shader
		glUniformMatrix4fv(modelLocColor, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(alphaLocColor, 1);
		glBindVertexArray(visualizerVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	// Draws Static Visualizer
	void Draw_Static_Visualizer()
	{
		// Send Transformations to Vertex Shader
		glUniformMatrix4fv(modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));
		glUniform4fv(brightnessLoc, 1, glm::value_ptr(glm::vec4(0.0f, 0.0f, 0.0f, 1)));
		glBindVertexArray(visualizerVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	// Update the Particle Generator
	void Update()
	{
		// Update Particle Data
		Update_Particles();

		// Create New Particles
		int particles_this_frame = 0;
		for (int i = 0; i < Maximum_Particles && particles_this_frame <= floor(particles_per_second * (deltaTime)); i++)
		{
			// Create New Particles Only if It is Dead
			if (dead_particles[i])
			{
				Add_Particles(i);
				particles_this_frame++;
			}
		}
	}

	// Test if Mouse is Inside Object
	bool TestCollisions()
	{
		if (mouseRelativeX >= xPos - Width / 2 && mouseRelativeX <= xPos + Width / 2)
		{
			if (mouseRelativeY >= yPos - Height / 2 && mouseRelativeY <= yPos + Height / 2)
			{
				return true;
			}
		}
		return false;
	}

private:

	// Determines if the Object has been initialized
	bool initialized = false;

	// Vertex Objects of Particle
	GLuint particleVAO;
	GLuint particleVBO;

	// Model Matrix of Object
	glm::mat4 model = glm::mat4(1.0f);

	// Lower Left Corner of Generator
	float cornerx, cornery;

	// Angle Lower Bound and Angle Range
	float angle_lower_bound, angle_range;
};