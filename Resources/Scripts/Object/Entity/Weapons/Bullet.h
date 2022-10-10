// The Bullet Projectile

class Bullet
{
public:

	// Position of Bullet
	glm::vec3 Position;

	// Velocity of Bullet
	glm::vec2 Velocity;

	// How much Damage the Bullet Deals
	int MaxDamage;
	int CurrentDamage = 0;
	double StartDecay = 0;
	double CurrentDecay = 0;

	// The Team the Bullet Belongs to
	// 0 = None, 1 = Friendly, 2 = Enemy
	int Team;

	// The Bullet Object
	Bullet(glm::vec2 origin, double speed, double angle, int damage, double decay, double stun, int team)
	{
		// Set Initial Variables
		texture = Foreground_Textures.find("this_is_a_meme.png")->second;
		Position = glm::vec3(origin.x, origin.y , 0.0f);
		Theta = angle;
		CurrentDamage = MaxDamage = damage;
		StartDecay = CurrentDecay = decay;
		Team = team;

		// Calculate the Velocity
		Velocity.x = (float)(speed * cos(Theta));
		Velocity.y = (float)(speed * sin(Theta));

		// Initialize Vertex Objects
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		// Bind Vertex Objects
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Create Vertices
		float vertices[30];
		float* pointer = Rect_GenVertices_Texture(0.0f, 0.0f, -4.0f, 0.5f, 0.1f);
		for (int i = 0; i < 30; i++)
		{
			vertices[i] = *(pointer + i);
		}

		// Bind Vertex Data
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Assign Position Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
		glEnableVertexAttribArray(0);

		// Assign Texture Vertices
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);

		// Unbind Object
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// Default Bullet
	Bullet() { VAO = NULL; VBO = NULL; Position = glm::vec3(NULL, NULL, NULL); Velocity = glm::vec2(NULL, NULL); MaxDamage = NULL; Team = NULL; Theta = NULL; }

	// Update Object
	bool Update()
	{
		// Update Position
		Position.x += Velocity.x;
		Position.y += Velocity.y;

		// Calculate New Damage
		CurrentDecay -= deltaTime;
		CurrentDamage = 1 + (int)floor(MaxDamage * (CurrentDecay / StartDecay));

		// Kill Bullet if it Decayed
		if (CurrentDecay < 0)
		{
			return true;
		}

		// Update Model Matrix
		model = glm::translate(glm::mat4(1.0f), Position);
		model = glm::rotate(model, (float)Theta, glm::vec3(0.0f, 0.0f, 1.0f));

		// Test Collisions
		int sign;
		for (int level = 0; level < 9; level++)
		{
			// CollisionMaskFloor
			for (int i = 0; i < levels_list[level].floor_objects_size; i++)
			{
				if (levels_list[level].floor_objects[i].TestCollisions(Position.x, Position.y, SPEED * deltaTime * 1.5f + 1.0f, false))
				{
					return true;
				}
			}

			// CollisionMaskWall
			for (int i = 0; i < levels_list[level].wall_objects_size; i++)
			{
				sign = -1 + 2 * (levels_list[level].wall_objects[i].Type % 2);
				if (levels_list[level].wall_objects[i].TestCollisions(Position.x + sign * 0.75f, Position.y))
				{
					return true;
				}
			}

			// CollisionMaskReverseFloor
			for (int i = 0; i < levels_list[level].ceiling_objects_size; i++)
			{
				if (levels_list[level].ceiling_objects[i].TestCollisions(Position.x, Position.y))
				{
					return true;
				}
			}
		}

		// Test if Bullet is Inside Enemy
		if (Position.x < enemy.position.x + enemy.posXmodifier && Position.x > enemy.position.x - enemy.posXmodifier && Position.y < enemy.position.y + enemy.posYmodifier && Position.y > enemy.position.y - enemy.posYmodifier)
		{
			enemy.Damage(CurrentDamage);
			return true;
		}

		// Send Transformations to Vertex Shader
		glUniformMatrix4fv(modelLocTextureStatic, 1, GL_FALSE, glm::value_ptr(model));

		// Enable Texture
		glActiveTexture(texture.index + 3);
		glBindTexture(texture.type, texture.texture);
		glUniform1i(textureLayerLocTextureStatic, 0);

		// Draw Object
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		// Disable Texture
		glBindTexture(texture.type, 0);

		return false;
	}

private:

	// Vertex Objects
	GLuint VAO, VBO;

	// Model Matrix
	glm::mat4 model = glm::mat4(1.0f);

	// Texture of Bullet
	Texture texture = texture_null;

	// Angle of the Object
	double Theta;
};
