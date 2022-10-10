// The Default Weapon

class Glock : public Weapon_Base
{
public:

	Glock(int team, float zPos)
	{
		// Save Team of Glock
		Team = team;

		// Set Texture
		texture = Weapon_Textures.find("Glock.png")->second;

		// Initialize Vertex Objects
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		// Bind Vertex Objects
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Calculate Vertices
		float vertices[30];
		float* pointer = Rect_GenVertices_Texture(0.0f, 0.0f, zPos + 0.1f, 3.0f, 1.0f);
		for (int i = 0; i < 30; i++)
		{
			vertices[i] = *(pointer + i);
		}

		// Bind Vertices
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
		displayClipMax = GLOCK_MAX_CLIP_SIZE;
		displayAmmoMax = GLOCK_MAX_AMMO_SIZE;
	}

	// Default Constructor
	Glock() { VAO = NULL; VBO = NULL; Team = NULL; }

	// Draw and Use Object
	void Update(float xPos, float yPos, float xPos2, float yPos2, bool& shoot, bool& reload, glm::vec2& momentum)
	{
		// Calculte Angle
		int direction;
		double theta = Angle(direction, xPos, xPos2, yPos, yPos2, Recoil);

		// Recoil
		Update_Recoil(Recoil);

		// Shoot
		Shoot_SemiAuto(bullets, ShootTimer, Recoil, ReloadTimer, Clip, bulletCount, shoot, reload, GLOCK_RATE_OF_FIRE, GLOCK_RECOIL, glm::vec2(xPos, yPos), GLOCK_BULLET_SPEED, theta, GLOCK_DAMAGE, GLOCK_DECAY_TIMER, GLOCK_STUN_TIMER, Team);

		// Reload
		Reload(ReloadTimer, Clip, Ammo, reload, GLOCK_MAX_CLIP_SIZE, GLOCK_RELOAD_TIMER);

		// Calculate Model Matrix
		model = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, yPos, 0.0f));
		model = glm::rotate(model, (float)theta, glm::vec3(0.0f, 0.0f, 1.0f));

		// Send Model Matrix to Shader
		glUniformMatrix4fv(modelLocTextureStatic, 1, GL_FALSE, glm::value_ptr(model));

		// Send Direction to Shader
		glUniform1i(directionLoc, direction);

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

		// Update and Display Bullets
		UpdateBullets(bullets, bulletCount);

		// Update Display Information
		displayClip = Clip;
		displayAmmo = Ammo;
	}

private:

	// Model Matrix
	glm::mat4 model = glm::mat4(1.0f);

	// Vertex Objects
	GLuint VAO, VBO;

	// Glock Constants
	int GLOCK_DAMAGE = 1;
	double GLOCK_BULLET_SPEED = 0.75;
	double GLOCK_RATE_OF_FIRE = 1;
	short unsigned int GLOCK_MAX_CLIP_SIZE = 5;
	short unsigned int GLOCK_MAX_AMMO_SIZE = 20;
	double GLOCK_STUN_TIMER = 0.1;
	double GLOCK_DECAY_TIMER = 2;
	double GLOCK_RELOAD_TIMER = 1.5;
	double GLOCK_RECOIL = 0.52;

	// Which Entity is Holding Glock
	int Team;

	// List of Bullets
	Bullet bullets[10];
	unsigned int bulletCount = 0;

	// The Clip
	short unsigned int Clip = 5;
	short unsigned int Ammo = 25;

	// Timers
	double ReloadTimer = 0;
	double ShootTimer = 0;
	double Recoil = 0;
};
