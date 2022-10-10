// Best Gun

class The_One : public Weapon_Base
{
public:

	The_One(int team, float zPos)
	{
		// Save Team of Glock
		Team = team;

		// Set Texture
		texture = Weapon_Textures.find("The_One.png")->second;

		// Initialize Vertex Objects
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		// Bind Vertex Objectsd
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Calculate Vertices
		float vertices[30];
		float* pointer = Rect_GenVertices_Texture(0.0f, 0.0f, zPos + 0.1f, 3.0f, 1.0f);
		for (int i = 0; i < 30; i++)
		{
			vertices[i] = *(pointer + i);
		}

		// Bind Buffer Data
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Enable Position Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
		glEnableVertexAttribArray(0);

		// Enable Texture Vertices
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);

		// Unbind Vertex Objects
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Set Display Info
		displayClipMax = THE_ONE_MAX_MAGAZINE_SIZE;
		displayAmmoMax = THE_ONE_MAX_AMMO_SIZE;
	}

	// Default Constructor
	The_One() { VAO = NULL; VBO = NULL; Team = NULL; }

	// Draw and Use Object
	void Update(float xPos, float yPos, float xPos2, float yPos2, bool& shoot, bool& reload, glm::vec2& momentum)
	{
		// Calculate Angle
		int direction;
		double theta = Angle(direction, xPos, xPos2, yPos, yPos2, 0);

		// Shoot
		Shoot_Knockback(bullets, ShootTimer, ReloadTimer, Magazine, bulletCount, shoot, reload, THE_ONE_RATE_OF_FIRE, glm::vec2(xPos, yPos), momentum, THE_ONE_KOCKBACK, THE_ONE_BULLET_SPEED, theta, THE_ONE_DAMAGE, THE_ONE_DECAY_TIMER, THE_ONE_STUN_TIMER, Team, true);

		// Reload
		Reload(ReloadTimer, Magazine, Ammo, reload, THE_ONE_MAX_MAGAZINE_SIZE, THE_ONE_RELOAD_TIMER);

		// Calculate Model Matrix
		model = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, yPos, 0.0f));
		model = glm::rotate(model, (float)theta, glm::vec3(0.0f, 0.0f, 1.0f));

		// Send Model Matrix to Shader
		glUniformMatrix4fv(modelLocTextureStatic, 1, GL_FALSE, glm::value_ptr(model));

		// Send Direction to Shader
		glUniform1i(directionLoc, direction);

		// Enable Texture
		glActiveTexture(texture.index);
		glBindTexture(texture.type, texture.texture);
		glUniform1i(textureLayerLocTextureStatic, 0);

		// Draw Object
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		// Disable Texture
		glBindTexture(texture.type, 0);

		// Draw and Update Bullets
		UpdateBullets(bullets, bulletCount);

		// Update Display Information
		displayClip = Magazine;
		displayAmmo = Ammo;
	}

private:

	// Model Matrix
	glm::mat4 model = glm::mat4(1.0f);

	// Vertex Objects
	GLuint VAO, VBO;

	// The One Constants
	int THE_ONE_DAMAGE = 2;
	double THE_ONE_BULLET_SPEED = 4;
	double THE_ONE_RATE_OF_FIRE = 0.05;
	short unsigned int THE_ONE_MAX_MAGAZINE_SIZE = 60;
	short unsigned int THE_ONE_MAX_AMMO_SIZE = 240;
	double THE_ONE_STUN_TIMER = 0.2;
	double THE_ONE_DECAY_TIMER = 1;
	double THE_ONE_RELOAD_TIMER = 2;
	double THE_ONE_KOCKBACK = 6;

	// Which Entity is Holding The One
	int Team;

	// Lists of Bullets
	Bullet bullets[100];
	unsigned int bulletCount = 0;

	// The Magazine
	short unsigned int Magazine = 60;
	short unsigned int Ammo = 240;

	// Timers
	double ReloadTimer = 0;
	double ShootTimer = 0;
};