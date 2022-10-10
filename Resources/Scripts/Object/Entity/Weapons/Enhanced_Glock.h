// Second Weapon and an Upgrade to the Glock

class Enhanced_Glock : public Weapon_Base
{
public:

	Enhanced_Glock(int team, float zPos)
	{
		// Save Team of Enhanced GLock
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

		// Bind Vertex Data
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
		displayClipMax = ENHANCED_GLOCK_MAX_CLIP_SIZE;
		displayAmmoMax = ENHANCED_GLOCK_MAX_AMMO_SIZE;
	}

	// Default Constructor
	Enhanced_Glock() { VAO = NULL; VBO = NULL; Team = NULL; }

	// Draw and Use Object
	void Update(float xPos, float yPos, float xPos2, float yPos2, bool& shoot, bool& reload, glm::vec2& momentum)
	{
		// Calculate Angle
		int direction;
		double theta = Angle(direction, xPos, xPos2, yPos, yPos2, Recoil);

		// Recoil
		Update_Recoil(Recoil);

		// Shoot Weapon
		Shoot_SemiAuto(bullets, ShootTimer, Recoil, ReloadTimer, Clip, bulletCount, shoot, reload, ENHANCED_GLOCK_RATE_OF_FIRE, ENHANCED_GLOCK_RECOIL, glm::vec2(xPos, yPos), ENHANCED_GLOCK_BULLET_SPEED, theta, ENHANCED_GLOCK_DAMAGE, ENHANCED_GLOCK_DECAY_TIMER, ENHANCED_GLOCK_STUN_TIMER, Team);

		// Reload
		Reload(ReloadTimer, Clip, Ammo, reload, ENHANCED_GLOCK_MAX_CLIP_SIZE, ENHANCED_GLOCK_RELOAD_TIMER);
	
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

	// Enhanced Glock Constants
	int ENHANCED_GLOCK_DAMAGE = 1;
	double ENHANCED_GLOCK_BULLET_SPEED = 0.85;
	double ENHANCED_GLOCK_RATE_OF_FIRE = 0.5;
	short unsigned int ENHANCED_GLOCK_MAX_CLIP_SIZE = 8;
	short unsigned int ENHANCED_GLOCK_MAX_AMMO_SIZE = 32;
	double ENHANCED_GLOCK_STUN_TIMER = 0.2;
	double ENHANCED_GLOCK_DECAY_TIMER = 2;
	double ENHANCED_GLOCK_RELOAD_TIMER = 1.5;
	double ENHANCED_GLOCK_RECOIL = 0.52;

	// Which Entity is Holding Enhanced Glock
	int Team;

	// List of Bullets
	Bullet bullets[16];
	unsigned int bulletCount = 0;

	// The Clip
	short unsigned int Clip = 8;
	short unsigned int Ammo = 32;

	// Timers 
	double ReloadTimer = 0;
	double ShootTimer = 0;
	double Recoil = 0;
};
