// The Base Class for All Weapons

class Weapon_Base
{
public:

	// Texture
	Texture texture = texture_null;

	// Clip and Ammo Information
	int displayClip = 0;
	int displayClipMax = 0;
	int displayAmmo = 0;
	int displayAmmoMax = 0;

	// Virtual Weapon Update Function
	virtual void Update(float xPos, float yPos, float xPos2, float yPos2, bool& shoot, bool& reload, glm::vec2& momentum) = 0;

protected:

	// Calculate Angle of Weapon
	double Angle(int& direction, float xPos, float xPos2, float yPos, float yPos2, double Recoil)
	{
		// Calculate Angle
		double theta = atan((yPos - yPos2) / (xPos - xPos2));

		// Determine Which Side The Angle Should Be On
		if (xPos2 < xPos)
		{
			theta += 3.142 - Recoil;
			direction = -1;
		}
		else
		{
			theta += Recoil;
			direction = 1;
		}

		return theta;
	}

	// Calculate Recoil
	void Update_Recoil(double& Recoil)
	{
		if (Recoil > 0)
		{
			Recoil -= deltaTime;

			// Prevent Recoil From Going Below 0
			if (Recoil < 0)
			{
				Recoil = 0;
			}
		}
	}

	// Click Once, One Bullet
	void Shoot_SemiAuto(Bullet bullets[], double& ShootTimer, double& Recoil, double& ReloadTimer, short unsigned int& Clip, unsigned int& bulletCount, bool& shoot, bool& reload, double RateOfFire, double MaxRecoil, glm::vec2 position, double speed, double angle, int damage, double decay, double stun, int Team)
	{
		// Weapon Cooldown
		if (ShootTimer > 0)
		{
			ShootTimer -= deltaTime;
		}

		// Test if Weapon Should Shoot
		else if (shoot && Clip && !ReloadTimer)
		{
			bullets[bulletCount] = Bullet(position, speed, angle, damage, decay, stun, Team);
			bulletCount++;
			Clip--;
			shoot = false;
			ShootTimer = RateOfFire;
			Recoil += MaxRecoil;

			// If Clip is Empty, Reload
			if (!Clip)
			{
				reload = true;
			}
		}
	}

	// Hold Mouse, Many Bullets
	void Shoot_Auto(Bullet bullets[], double& ShootTimer, double& Recoil, double& ReloadTimer, short unsigned int& Magazine, unsigned int& bulletCount, bool& shoot, bool& reload, double RateOfFire, double MaxRecoil, glm::vec2 position, double speed, double angle, int damage, double decay, double stun, int Team)
	{
		// Weapon Cooldown
		if (ShootTimer > 0)
		{
			ShootTimer -= deltaTime;
		}

		// Test if Weapon Should Shoot
		else if (shoot && Magazine && !ReloadTimer)
		{
			bullets[bulletCount] = Bullet(position, speed, angle, damage, decay, stun, Team);
			bulletCount++;
			Magazine--;
			ShootTimer = RateOfFire;
			Recoil += MaxRecoil;

			// If Magazine is Empty, Reload
			if (!Magazine)
			{
				reload = true;
				shoot = false;
			}
		}
	}

	// Hold Mouse, Bullets Knock You Back
	void Shoot_Knockback(Bullet bullets[], double& ShootTimer, double& ReloadTimer, short unsigned int& Magazine, unsigned int& bulletCount, bool& shoot, bool& reload, double RateOfFire, glm::vec2 position, glm::vec2& momentum, double Knockback, double speed, double angle, int damage, double decay, double stun, int Team, bool Auto)
	{
		// Weapon Cooldown
		if (ShootTimer > 0)
		{
			ShootTimer -= deltaTime;
		}

		// Test if Weapon Should Shoot
		else if (shoot && Magazine && !ReloadTimer)
		{
			bullets[bulletCount] = Bullet(position, speed, angle, 1, decay, stun, 0);
			bulletCount++;
			Magazine--;
			ShootTimer = RateOfFire;
			shoot = Auto;

			// Move Entity
			double newTheta = angle + 3.142;
			momentum.x += (float)Knockback * (float)cos(newTheta);
			momentum.y += (float)Knockback * (float)sin(newTheta);

			// Prevent Momentum From Going Too Extreme
			if (momentum.x > 30.0f) { momentum.x = 30.0f; }
			if (momentum.y > 30.0f) { momentum.y = 30.0f; }

			// If Magazine is Empty, Reload
			if (!Magazine)
			{
				reload = true;
				shoot = false;
			}
		}
	}

	// Reload
	void Reload(double& ReloadTimer, short unsigned int& Clip, short unsigned int& Ammo, bool& reload, short unsigned int Max_Clip_Size, double Max_Reload_Timer)
	{
		// Test if Weapon Should Reload
		if (reload && !ReloadTimer && Ammo && Clip < Max_Clip_Size)
		{
			ReloadTimer = Max_Reload_Timer;
		}

		if (ReloadTimer)
		{
			ReloadTimer -= deltaTime;

			// Test if Reloading is Done
			if (ReloadTimer < 0)
			{
				ReloadTimer = 0;

				// The Number of Ammo to Refil
				int AmmoToRefil = Max_Clip_Size - Clip;

				// When Ammo is Less Than Clip Size
				if (Ammo < AmmoToRefil)
				{
					Clip += Ammo;
					Ammo = 0;
				}

				// When Ammo is Greater Than Clip Size
				else
				{
					Clip += AmmoToRefil;
					Ammo -= AmmoToRefil;
				}
			}
		}

		reload = false;
	}

	// Update and Display Bullets
	void UpdateBullets(Bullet bullets[], unsigned int& bulletCount)
	{
		// Draw and Update Bullets
		for (unsigned int i = 0; i < bulletCount; i++)
		{
			// Delete Bullet if Collision Occoured
			if (bullets[i].Update())
			{
				// Pop Bullet from List
				int offset = 0;
				for (unsigned int index = 0; index < bulletCount; index++)
				{
					// If Popped Bullet is Reached, Set Offset
					if (index == i)
					{
						offset++;
					}

					// Re-assign Bullets to List sans Popped Bullet
					bullets[index] = bullets[index + offset];
				}

				// Decrease Bullet Count
				bulletCount--;
			}
		}
	}
}; 
