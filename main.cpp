#include <GL/glut.h>
#include <SFML/Audio.hpp>

#include <cmath>
#include <iostream>
#include <random>
#include <iostream>

#include "keyboard.hpp"
#include "mouse.hpp"
#include "types.hpp"

#include "textures/textures.ppm"
#include "textures/sprites.ppm"

// Katana
#include "textures/katanaNormal.ppm"
#include "textures/katanaBloody.ppm"
#include "textures/katanaSwing.ppm"
#include "textures/katanaBloodySwing.ppm"
#include  "textures/katanaBlock.ppm"
#include  "textures/katanaBlockHit.ppm"
#include  "textures/katanaBloodyBlock.ppm"
#include  "textures/katanaBloodyBlockHit.ppm"

// Ninja enemy
#include "textures/ninjaWalk.ppm"
#include "textures/ninjaAttack.ppm"
#include "textures/ninjaDeath.ppm"

// Health
#include "textures/health100.ppm"
#include "textures/health80.ppm"
#include "textures/health60.ppm"
#include "textures/health40.ppm"
#include "textures/health20.ppm"
#include "textures/health10.ppm"
#include "textures/test.ppm"

// Splashscreens
#include "textures/titleScreen.ppm"
#include "textures/gameOver.ppm"
#include "textures/getReady.ppm"

#define PI 3.1415926535
#define PI2 PI*2
#define P2 PI/2
#define P3 3*PI/2
#define SENSITIVITY 0.0005
#define WIDTH 952
#define HEIGHT 632
#define DEGREE 0.0174533
#define WALKSPEED 20
#define FOV 30
#define RAYCOUNT 121

// Game states
#define GAMEPLAY 1
#define TITLE 2
#define GETREADY 3
#define GAMEOVER 4

Position player, playerDelta;
float playerAngle, strafeX, strafeY;

float deltaTime;

int xOffset, yOffset;

int health = 100;
int enemyHealth = 100;
int gameState = 2; // Title screen
int level = 0;

// Timers
int timer = 0;
int playerAttackTimer = 0;
int enemyAttackTimer = 0;
int getReadyTimer = 0;
int getReadyTimer2 = 0;
int gameOverTimer = 0;
int blockHitTimer = 0;
int frameTimer = 0;
int enemyAttackingTimer = 0;
int enemyBlockTimer = 0;
int frameTime = 250;

double fade = 0;

double enemyDistance = 999;

int currentAnimFrame = 0;

double enemyMovementSpeed = 0.05;
int spriteBlockChance = 10;
int spriteHP = 100;
int spriteDamage = 20;
double spriteHitSpeed = 1000;

int randPos;

bool canAttack = false;
bool waiting = false;
bool enemyAttacking = false;
bool enemyBlock = false;
bool spriteDead = false;
int randSound;

bool swing;
bool bloody;
bool blocking;
bool blockHit;

Sprite sprites[4];

sf::Music music;

sf::SoundBuffer yell1Buff, yell2Buff, yell3Buff, katanaSwingBuff, gongBuff, introBuff, medkitBuff, enemyHitBuff, enemyDeathBuff, swordClashBuff, hitDeadBuff, hitSlash1Buff, hitSlash2Buff, hitSlash3Buff, slash1Buff, slash2Buff;

sf::Sound katanaSwing, gong, intro, medkit, enemyHit, enemyDeath, swordClash, hitDead;

// Enemy sounds
sf::Sound hitSlash4, hitSlash2, hitSlash3, slash1, slash2;

void init();

// mapWalls
int mapWallsX = 8, mapWallsY = 8, mapWallsTotal = mapWallsX * mapWallsY;
int mapWalls[] = {
	1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	9,0,0,0,0,0,0,9,
	9,0,0,0,0,0,0,9,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1
};

int mapFloors[] = {
	1,1,1,1,1,1,1,1,
	1,7,7,8,8,7,7,1,
	1,7,7,8,8,7,7,1,
	1,7,7,8,8,7,7,1,
	1,7,7,8,8,7,7,1,
	1,7,7,8,8,7,7,1,
	1,7,7,8,8,7,7,1,
	1,1,1,1,1,1,1,1
};

int mapCeiling[] = {
	1,1,1,1,1,1,1,1,
	1,5,5,5,5,5,5,1,
	1,5,5,5,5,5,5,1,
	1,5,5,5,5,5,5,1,
	1,5,5,5,5,5,5,1,
	1,5,5,5,5,5,5,1,
	1,5,5,5,5,5,5,1,
	1,1,1,1,1,1,1,1
};

int random(int bottom, int top) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(bottom, top);

	return dist(rng);
}

inline float calcDist(Position a, Position b) {
	return sqrtf(powf(b.x - a.x, 2) + powf(b.y - a.y, 2));
}

inline double fixAngle(double angle) {
	if (angle < 0) {
		angle += PI2;
	}

	if (angle > PI2) {
		angle -= PI2;
	}
	return angle;
}

inline float fixAngleDeg(float angle) {
	if (angle > 359) {
		angle -= 360;
	} if (angle < 0) {
		angle += 360;
	}
	return angle;
}

inline float radToDeg(float rad) {
	return rad * 180 / PI;
}

inline float degToRad(float deg) {
	return deg * PI / 180;
}

int depth[RAYCOUNT];
float enemyAngle = 999;

void enemyAttack() {
	currentAnimFrame = 0;
	if (enemyAttackTimer > spriteHitSpeed) {
		enemyAttacking = true;
		if (blocking) {
			if (random(1, 5) == 1) {
				health -= spriteDamage / 2;
			}
			blockHit = true;
			int randSound = random(1, 2);
			if (randSound == 1) {
				slash1.play();
			}
			else
			{
				slash2.play();
			}
			swordClash.setPitch(static_cast<float>(random(90, 110)) / 100.0);
			swordClash.play();
		}
		else
		{
			health -= spriteDamage;
		}
		randSound = random(1, 3);
		switch (randSound)
		{
		case 1:
			hitSlash4.setPitch(static_cast<float>(random(90, 110)) / 100.0);
			hitSlash4.play();
			break;
		case 2:
			hitSlash2.setPitch(static_cast<float>(random(90, 110)) / 100.0);
			hitSlash2.play();
			break;
		case 3:
			hitSlash3.setPitch(static_cast<float>(random(90, 110)) / 100.0);
			hitSlash3.play();
			break;
		}
		enemyAttackTimer = 0;
	}
}

void playerAttack() {
	if (playerAttackTimer > 1000 && !blocking) {
		katanaSwing.play();
		swing = true;
		if (canAttack && sprites[2].state == 1) {
			if (random(1, spriteBlockChance) != 1)
			{
				randSound = random(1, 3);
				switch (randSound)
				{
				case 1:
					hitSlash4.setPitch(static_cast<float>(random(90, 110)) / 100.0);
					hitSlash4.play();
					break;
				case 2:
					hitSlash2.setPitch(static_cast<float>(random(90, 110)) / 100.0);
					hitSlash2.play();
					break;
				case 3:
					hitSlash3.setPitch(static_cast<float>(random(90, 110)) / 100.0);
					hitSlash3.play();
					break;
				}

				bloody = true;
				enemyHealth -= 20;
				if (enemyHealth <= 0) {
					currentAnimFrame = 0;
					sprites[2].state = 0;
					hitDead.play();
					enemyDeath.play();
				}
				else
				{
					enemyHit.play();
				}
			}
			else
			{
				enemyBlock = true;
				blockHit = true;
				swordClash.play();
			}
		}
		playerAttackTimer = 0;
	}
}

void drawSprites() {

	int x, y, s;

	// Medkit
	if (sprites[0].state && player.y < abs(sprites[0].pos.x) + 30 && player.y > abs(sprites[0].pos.x) - 30 && player.x < sprites[0].pos.y + 30 && player.x > sprites[0].pos.y - 30) {
		sprites[0].state = 0;
		health += 50;
		if (health > 100) {
			health = 100;
		}
		medkit.play();
	}

	// Enemy
	// 
	// Movement
	if (!enemyAttacking && sprites[2].state == 1 && enemyDistance > 30) {
		if (player.x > sprites[2].pos.y) {
			sprites[2].pos.y += (enemyMovementSpeed)*deltaTime;
		}

		if (player.x < sprites[2].pos.y && enemyDistance > 30) {
			sprites[2].pos.y -= (enemyMovementSpeed)*deltaTime;
		}

		if (player.y < abs(sprites[2].pos.x) && enemyDistance > 30) {
			sprites[2].pos.x += (enemyMovementSpeed)*deltaTime;
		}

		if (player.y > abs(sprites[2].pos.x) && enemyDistance > 30) {
			sprites[2].pos.x -= (enemyMovementSpeed)*deltaTime;
		}
	}
	
	// Attack
	if (sprites[2].state && enemyDistance < 30) {
		enemyAttack();
	}

	if (enemyAttackingTimer > 500) {
		enemyAttacking = false;
		enemyAttackingTimer = 0;
	}

	// Player attack
	if (-5 < enemyAngle && enemyAngle < 5 && enemyDistance < 40) {
		canAttack = true;
	}
	else
	{
		canAttack = false;
	}

	//std::cout << "Player x: " << player.x << " Player y: " << player.y << " Sprite x: " << sprites[2].x << " Sprite y: " << sprites[2].y << std::endl;
	for(int s = 0; s < 3; ++s){
		// Temp
		float spriteX = sprites[s].pos.x + player.y;
		float spriteY = sprites[s].pos.y - player.x;
		float spriteZ = sprites[s].z;

		float a = spriteX * cos(playerAngle) + spriteY * sin(playerAngle);
		float b = spriteX * sin(playerAngle) - spriteY * cos(playerAngle);

		if (s == 2) {
			enemyAngle = a;
		}

		spriteX = a;
		spriteY = b;

		spriteX = (spriteX * (54.0 / FOV * 60) / spriteY) + (60); //convert to screen
		spriteY = (spriteZ * (54.0 / FOV * 60) / spriteY) + (40);

		int scale = 32 * 80 / abs(b);
		if (scale < 0) {
			scale = 0;
		}
		if (scale > 120) {
			scale = 120;
		}

		// Calculate enemy distance
		if (s == 2) {
			enemyDistance = sqrtf(powf(player.y + sprites[2].pos.x, 2) + powf(player.x - sprites[2].pos.y, 2));
		}

		// Texture
		glPointSize(8);
		glBegin(GL_POINTS);
		if (s != 2) {
			float textureX = 0, textureY = 31, textureXStep = 31.5 / (float)scale, textureYStep = 32.0 / (float)scale;

			for (x = spriteX - scale / 2; x < spriteX + scale / 2; ++x) {
				textureY = 31;
				for (y = 0; y < scale; ++y) {
					if (sprites[s].state == 1 && x > 0 && spriteX < 120 && abs(b) < depth[x] && b < 0) {

						int pixel = ((int)textureY * 32 + (int)textureX) * 3 + (sprites[s].texture * 32 * 32 * 3);
						int red = spriteTextures[pixel + 0];
						int green = spriteTextures[pixel + 1];
						int blue = spriteTextures[pixel + 2];

						if (red != 255, blue != 255) {
							glColor3ub(red, green, blue);
							glVertex2i(x * 8, spriteY * 8 - y * 8);
						}
						textureY -= textureYStep;
						if (textureY < 0) {
							textureY = 0;
						}
					}
				}
				textureX += textureXStep;
			}
		}
		else // If current sprite is enemy
		{
			const int* currentNinjaFrame = nullptr;
			if (frameTimer > frameTime)
			{
				++currentAnimFrame;
				frameTimer = 0;
				enemyBlock = false;
			}

			if (enemyAttacking) {
				frameTime = (spriteHitSpeed * 0.75) / 3;
				currentNinjaFrame = ninjaAttack;
				if (currentAnimFrame > 2) {
					currentAnimFrame = 2;
				}
			}
			else if (!spriteDead)
			{
				frameTime = enemyMovementSpeed * 2500;
				currentNinjaFrame = ninjaWalk;
				if (currentAnimFrame > 5) {
					currentAnimFrame = 0;
				}
			}
			else
			{
				frameTime = 100;
				currentNinjaFrame = ninjaDeath;
				if (currentAnimFrame > 6) {
					currentAnimFrame = 6;
				}
			}
			if (enemyBlock) {
				frameTimer = 0;
				currentNinjaFrame = ninjaAttack;
				currentAnimFrame = 0;
			}

			float textureX = 0, textureY = 63.0, textureXStep = 63.5 / (float)scale, textureYStep = 64.0 / (float)scale;

			for (x = spriteX - scale / 2; x < spriteX + scale / 2; ++x) {
				textureY = 63;
				for (y = 0; y < scale; ++y) {
					if (x > 0 && spriteX < 120 && abs(b) < depth[x] && b < 0) {

						int pixel = ((int)textureY * 64 + (int)textureX) * 3 + (currentAnimFrame * 64 * 64 * 3);
						int red = currentNinjaFrame[pixel + 0];
						int green = currentNinjaFrame[pixel + 1];
						int blue = currentNinjaFrame[pixel + 2];

						if (red != 255, blue != 255) {
							glColor3ub(red, green, blue);
							glVertex2i(x * 8, spriteY * 8 - y * 8);
						}
						textureY -= textureYStep;
						if (textureY < 0) {
							textureY = 0;
						}
					}
				}
				textureX += textureXStep;
			}
		}
		glEnd();
	}
}

void drawRays() {
	int rays, mapWallsLocation, steps;
	double rayAngle, dist;

	Position ray, offset, raymapWallsPos;

	rayAngle = playerAngle - DEGREE * FOV;
	rayAngle = fixAngle(rayAngle);

#pragma region rayCasting
	for (rays = 0; rays < RAYCOUNT; ++rays) {

		int verticalmapWallsTexture, horizontalmapWallsTexture;

		// Check vertical lines
		steps = 0;
		float disV = 1000000;
		Position vertical;
		float nTan = -tan(rayAngle);
		if (rayAngle > P2 && rayAngle < P3) { // Looking left
			ray.x = (((int)player.x >> 6) << 6) - 0.0001;
			ray.y = (player.x - ray.x) * nTan + player.y;
			offset.x = -64; offset.y = -offset.x * nTan;
		}

		if (rayAngle < P2 || rayAngle > P3) // Looking right
		{
			ray.x = (((int)player.x >> 6) << 6) + 64;
			ray.y = (player.x - ray.x) * nTan + player.y;
			offset.x = 64;
			offset.y = -offset.x * nTan;
		}

		if (rayAngle == 0 || rayAngle == PI) // Player angle straight up or down
		{
			ray.x == player.x;
			ray.y == player.y;
			steps = 8;
		}

		while (steps < 8)
		{
			raymapWallsPos.x = (int)(ray.x) >> 6; raymapWallsPos.y = (int)(ray.y) >> 6;
			mapWallsLocation = raymapWallsPos.y * mapWallsX + raymapWallsPos.x;
			if (mapWallsLocation > 0 && mapWallsLocation < mapWallsX * mapWallsY && mapWalls[mapWallsLocation] > 0) { // Wall hit & calculate distance
				verticalmapWallsTexture = mapWalls[mapWallsLocation] - 1;
				vertical = ray;
				disV = calcDist(player, vertical);
				steps = 8;
			}
			else
			{
				ray.x += offset.x;
				ray.y += offset.y;
				steps += 1;
			}
		}

		// Check horizontal lines
		steps = 0;
		float disH = 1000000;
		Position horizontal = player;
		float aTan = -1 / tan(rayAngle); // Find slope

		if (rayAngle > PI) // Player angle up
		{
			ray.y = (((int)player.y >> 6) << 6) - 0.0001;
			ray.x = (player.y - ray.y) * aTan + player.x;
			offset.y = -64; offset.x = -offset.y * aTan;
		}

		if (rayAngle < PI) { // Player angle down
			ray.y = (((int)player.y >> 6) << 6) + 64;
			ray.x = (player.y - ray.y) * aTan + player.x;
			offset.y = 64; offset.x = -offset.y * aTan;
		}

		if (rayAngle == 0 || rayAngle == PI) // Player angle straight left or right
		{
			ray.x == player.x;
			ray.y == player.y;
			steps = 8;
		}

		while (steps < 8)
		{
			raymapWallsPos.x = (int)(ray.x) >> 6; raymapWallsPos.y = (int)(ray.y) >> 6;
			mapWallsLocation = raymapWallsPos.y * mapWallsX + raymapWallsPos.x;
			if (mapWallsLocation > 0 && mapWallsLocation < mapWallsX * mapWallsY && mapWalls[mapWallsLocation] > 0) { // Wall hit & calculate distance
				horizontalmapWallsTexture = mapWalls[mapWallsLocation] - 1;
				horizontal = ray;
				disH = calcDist(player, horizontal);
				steps = 8;
			}
			else
			{
				ray.x += offset.x; ray.y += offset.y;
				steps += 1;
			}
		}

#pragma endregion

		float shade = 1;
		if (disV < disH) { // Vertical wall
			horizontalmapWallsTexture = verticalmapWallsTexture;
			shade = 0.75;
			ray = vertical;
			dist = disV;
		}
		else {			   // Horizontal wall
			ray = horizontal;
			dist = disH;
		}

		float darkness = 1 - dist / 500;
		if (darkness < 0) {
			darkness = 0;
		}

		float ca = playerAngle - rayAngle; // Remove distortion
		ca = fixAngle(ca);
		dist = dist * cos(ca);

		int lineHeight = (mapWallsTotal * HEIGHT) / dist;

		// Texture offset and height
		float textureYStep = 32.0 / float(lineHeight);
		float textureYOffset = 0;

		if (lineHeight > HEIGHT) {
			textureYOffset = (lineHeight - HEIGHT) / 2.0;
			lineHeight = HEIGHT;
		}

		int lineOffset = HEIGHT / 2 - (lineHeight >> 1);

		depth[rays] = disH;

		// Draw walls
		int y;
		float textureY = textureYOffset * textureYStep; // Choose correct texture

		float textureX = 0;

		if (shade == 1) { // Horizontal textures
			textureX = (int)(ray.x / 2.0) % 32;

			if (rayAngle < PI) { // Flip texture
				textureX = -textureX - 1;
				textureY = textureY + 1;
			}
		}

		else {			  // Vertical texture
			textureX = (int)(ray.y / 2.0) % 32;

			if (rayAngle > P2 && rayAngle < P3) { // Flip texture
				textureX = -textureX - 1;
				textureY = textureY + 1;
			}
		}
		glPointSize(8);
		glBegin(GL_POINTS);
		for (y = 0; y < lineHeight; ++y) {

			// Calculate colors
			int pixel = ((int)textureY * 32 + (int)textureX) * 3 + (horizontalmapWallsTexture * 32 * 32 * 3);
			int red = textures[pixel + 0] * darkness;
			int green = textures[pixel + 1] * darkness;
			int blue = textures[pixel + 2] * darkness;

			// Plot pixels

			glColor3ub(red, green, blue);
			glVertex2i(rays * 8, y + lineOffset);
			textureY += textureYStep;
		}

		for (y = lineOffset + lineHeight; y < HEIGHT; ++y) {

			// Draw floors
			float dy = y - (HEIGHT / 2.0), deg = (rayAngle), raFix = cos(fixAngleDeg(playerAngle) - rayAngle);
			textureX = player.x / 2 + cos(deg) * 158 * 2 * 32 / dy / raFix;
			textureY = player.y / 2 + sin(deg) * 158 * 2 * 32 / dy / raFix;

			int mp = (mapFloors[(int)(textureY / 32.0) * mapWallsX + (int)(textureX / 32.0)] - 1) * 32 * 32;

			int pixel = (((int)(textureY) & 31) * 32 + ((int)(textureX) & 31)) * 3 + mp * 3;
			int red, green, blue;
			if (pixel >= 0 && pixel + 2 < 27650){ // Prevent rare crash from happening
				red = textures[pixel + 0] * 0.8;
				green = textures[pixel + 1] * 0.8;
				blue = textures[pixel + 2] * 0.8;
			}
			else {
				red = 255;
				green = 255;
				blue = 255;
			}

			// Plot pixels
			glColor3ub(red, green, blue);
			glVertex2i(rays * 8, y);

			// Draw ceiling
			mp = (mapCeiling[(int)(textureY / 32.0) * mapWallsX + (int)(textureX / 32.0)] - 1) * 32 * 32;

			pixel = (((int)(textureY) & 31) * 32 + ((int)(textureX) & 31)) * 3 + mp * 3;
			red = textures[pixel + 0] * 0.8;
			green = textures[pixel + 1] * 0.8;
			blue = textures[pixel + 2] * 0.8;

			// Plot pixels
			if (mp > 0) {
				glColor3ub(red, green, blue);
				glVertex2i(rays * 8, HEIGHT - y);

			}
		}
		glEnd();
		rayAngle += DEGREE * 0.5;
		rayAngle = fixAngle(rayAngle);
	}
}

void drawPlayer() {
	// Player
	glColor3f(1, 1, 0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(player.x, player.y);
	glEnd();

	// Rotation indicator
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(player.x, player.y);
	glVertex2i(player.x + playerDelta.x * 250, player.y + playerDelta.y * 250);
	glEnd();

	// Collision box
	glColor3f(0, 0, 1);
	glBegin(GL_LINES);
	glVertex2i(player.x + playerDelta.x * 100, player.y + playerDelta.y * 100); // Forward
	glVertex2i(player.x - playerDelta.x * 100, player.y - playerDelta.y * 100); // Back

	glVertex2i(player.x + strafeX * 15, player.y + strafeY * 15);				// Strafe left
	glVertex2i(player.x - strafeX * 15, player.y - strafeY * 15);				// Strafe right
	glEnd();
}

void draw2DmapWalls() {
	int x, y, boxX, boxY;

	// Check every grid in mapWalls
	for (y = 0; y < mapWallsY; ++y) {
		for (x = 0; x < mapWallsX; ++x) {
			if (mapWalls[y * mapWallsX + x] > 0) { glColor3f(1, 1, 1); } // y*mapWallsX is every 8th grid aka row
			else { glColor3f(0, 0, 0); }

			boxX = x * mapWallsTotal;
			boxY = y * mapWallsTotal;

			// Create 64x64 boxes
			glBegin(GL_QUADS);
			glVertex2i(boxX + 1, boxY + 1); // Top left
			glVertex2i(boxX + 1, boxY + mapWallsTotal - 1); // Bottom left
			glVertex2i(boxX + mapWallsTotal - 1, boxY + mapWallsTotal - 1); // Bottom right
			glVertex2i(boxX + mapWallsTotal - 1, boxY + 1); // Top right
			glEnd();
		}
	}
}

void gameHandler() {
	int x, y;

	const int* currentSplash = nullptr; // Hold current splash array

	// If in title menu
	if (key.space && gameState == TITLE) {
		gameState = GAMEPLAY;
	}

	if (health <= 0) {
		gameState = GAMEOVER;
	}

	// If dead and pressing space go to title
	if (gameOverTimer > 2000) {
		intro.play();
		init();
		gameOverTimer = 0;
		level = 0;
		gameState = TITLE;
	}

	if (sprites[2].state == 0) {
		spriteDead = true;
	}
	else
	{
		spriteDead = false;
	}

	if (enemyBlockTimer > 250) {
		enemyBlock = false;
		enemyBlockTimer = 0;
	}

	if (getReadyTimer > 2000) {
		gong.play();
		enemyHealth = spriteHP += level*10;
		++level;
		enemyMovementSpeed = 0.05 + (double) level / 400;
		--spriteBlockChance;
		spriteDamage = 20 + level * 2;
		spriteHitSpeed = 1000 - level * 25;

		if (spriteDamage >= 50) {
			spriteDamage = 50;
		}

		if (spriteHitSpeed <= 500) {
			spriteHitSpeed = 500;
		}

		if(spriteBlockChance <= 3){
			spriteBlockChance = 3;
		}

		if (enemyMovementSpeed >= 0.09) {
			enemyMovementSpeed = 0.09;
		}

		gameState = GETREADY;
		getReadyTimer = 0;
	}

	if (getReadyTimer2 > 2000) {
		gameState = GAMEPLAY;
		getReadyTimer2 = 0;
		init();
	}

	if (blockHitTimer > 200) {
		blockHit = false;
		blockHitTimer = 0;
	}

	switch (gameState)
	{
	case 1:
		if (intro.getStatus() == 2) {
			intro.stop();
		}
		if (music.getStatus() != 2) {
			music.play();
			music.setLoop(true);
		}
		return;
	case 2:
		currentSplash = titleScreen;
		break;
	case 3:
		currentSplash = getReady;
		break;
	case 4:
		music.stop();
		currentSplash = gameOver;
		break;
	default:
		break;
	}

	for (y = 0; y < 80; ++y) {
		for (x = 0; x < 120; ++x) {
			int pixel = (y * 120 + x) * 3;
			int red = currentSplash[pixel + 0] * fade;
			int green = currentSplash[pixel + 1] * fade;
			int blue = currentSplash[pixel + 2] * fade;

			glPointSize(8);
			glColor3ub(red, green, blue);
			glBegin(GL_POINTS);
			glVertex2i(x * 8, y * 8);
			glEnd();
		}
	}
}

void drawUI() {
	int x, y;

	const int* currentUIkatana = nullptr; // Hold current UI array
	const int* currentUIhealth = nullptr;

	if (swing) {
		if (bloody) {
			currentUIkatana = katanaBloodySwingUI;
		}
		else
		{
			currentUIkatana = katanaSwingUI;
		}
	}
	else
	{
		if (bloody) {
			currentUIkatana = katanaBloodyUI;
		}
		else
		{
			currentUIkatana = katanaNormalUI;
		}
	}
	if (blocking) {
		if (bloody) {
			currentUIkatana = katanaBloodyBlock;
		}
		else
		{
			currentUIkatana = katanaBlockUI;
		}
	}

	if (blockHit) {
		if (bloody) {
			currentUIkatana = katanaBloodyBlockHit;
		}
		else
		{
			currentUIkatana = katanaBlockHit;
		}
	}

	if (health > 90) {
		currentUIhealth = health100UI;
	}
	else if (health >= 70 && health <= 90) {
		currentUIhealth = health80UI;
	}
	else if (health >= 50 && health < 70) {
		currentUIhealth = health60UI;
	}
	else if (health >= 30 && health < 50) {
		currentUIhealth = health40UI;
	}
	else if (health >= 10 && health < 30) {
		currentUIhealth = health20UI;
	}
	else if (health < 10) {
		currentUIhealth = health10UI;
	}

	if (health <= 0) {
		return;
	}

	if (playerAttackTimer > 1001) {
		swing = false;
	}
	
	glPointSize(8);
	glBegin(GL_POINTS);

	// Sword
	for (y = 0; y < 80; ++y) {
		for (x = 0; x < 120; ++x) {
			int pixel = (y * 120 + x) * 3;
			int red   = currentUIkatana[pixel + 0];
			int green = currentUIkatana[pixel + 1];
			int blue  = currentUIkatana[pixel + 2];

			if (red != 255 && blue != 255) {
				glColor3ub(red, green, blue);
				glVertex2i(x * 8, y * 8);
			}
		}
	}
	// Health
	for (y = 0; y < 80; ++y) {
		for (x = 0; x < 120; ++x) {
			int pixel = (y * 120 + x) * 3;
			int red = currentUIhealth[pixel + 0];
			int green = currentUIhealth[pixel + 1];
			int blue = currentUIhealth[pixel + 2];

			if (red != 255 && blue != 255) {

				glColor3ub(red, green, blue);

				glVertex2i(x * 8, y * 8);
			}
		}
	}
	glEnd();
}

void init() {

	// Init player
	player.x = 100; player.y = 250;
	health = 100;
	playerDelta.x = 0;
	playerDelta.y = 0;
	playerAngle = 0;
	enemyAttackTimer = 0;

	bloody = false;
	canAttack = false;

	// Init sprites

	// Medkit
	sprites[0].type = 2;
	sprites[0].state = 1;
	sprites[0].texture = 0;
	sprites[0].pos.x = -random(2, 6) * 64;
	sprites[0].pos.y = random(3, 6) * 64;
	sprites[0].z = -20;

	// Dummy
	sprites[1].type = 3;
	sprites[1].state = 1;
	sprites[1].texture = 1;
	sprites[1].pos.x = -random(2, 6) * 64;;
	sprites[1].pos.y = random(3, 6) * 64;
	sprites[1].z = -20;

	// Enemy
	sprites[2].type = 4;
	sprites[2].state = 1;
	sprites[2].texture = 2;
	sprites[2].pos.x = -random(2, 6) * 64;
	sprites[2].pos.y = 6 * 64;
	sprites[2].z = -20;
	enemyHealth = 100;
}

float frame1, frame2;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	frame2 = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = (frame2 - frame1);
	frame1 = glutGet(GLUT_ELAPSED_TIME);

	if (timer < 3000) {
		timer += 1 * deltaTime;
		fade = timer / 3000.0;
		if (fade > 1)
			fade = 1;
	}

	if (enemyHealth <= 0) {
		getReadyTimer += 1 * deltaTime;
	}

	if (gameState == GETREADY) {
		getReadyTimer2 += 1 * deltaTime;
	}

	if (gameState == GAMEOVER) {
		gameOverTimer += 1 * deltaTime;
	}

	if (blockHit) {
		blockHitTimer += 1 * deltaTime;
	}

	if (enemyAttacking) {
		enemyAttackingTimer += 1 * deltaTime;
	}

	if (enemyBlock) {
		enemyBlockTimer += 1 * deltaTime;
	}

	frameTimer += 1 * deltaTime;

	if (gameState == GAMEPLAY) {

		playerAttackTimer += 1 * deltaTime;
		enemyAttackTimer += 1 * deltaTime;

		// Look left
		if (mouseDeltaX < 0) {
			playerAngle += (SENSITIVITY * mouseDeltaX) * deltaTime;
			playerAngle = fixAngle(playerAngle);
			playerDelta.x = cos(playerAngle) * 0.15;
			playerDelta.y = sin(playerAngle) * 0.15;
		}

		// Look right
		else if (mouseDeltaX > 0) {
			playerAngle += (SENSITIVITY * mouseDeltaX) * deltaTime;
			playerAngle = fixAngle(playerAngle);
			playerDelta.x = cos(playerAngle) * 0.15;
			playerDelta.y = sin(playerAngle) * 0.15;
		}

		// Calculate perpendicular direction
		strafeX = playerDelta.y;
		strafeY = -playerDelta.x;

		// Normalize the strafe direction
		float length = sqrt(strafeX * strafeX + strafeY * strafeY);
		if (length != 0) {
			strafeX /= length;
			strafeY /= length;
		}

		intPos deltaPosOffset, nDeltaPosOffset, strafeDeltaPosOffset, nStrafeDeltaPosOffset;

		deltaPosOffset.x = (player.x + playerDelta.x * 100) / 64;
		deltaPosOffset.y = (player.y + playerDelta.y * 100) / 64;

		nDeltaPosOffset.x = (player.x - playerDelta.x * 100) / 64;
		nDeltaPosOffset.y = (player.y - playerDelta.y * 100) / 64;

		strafeDeltaPosOffset.x = (player.x + strafeX * 15) / 64;
		strafeDeltaPosOffset.y = (player.y + strafeY * 15) / 64;

		nStrafeDeltaPosOffset.x = (player.x - strafeX * 15) / 64;
		nStrafeDeltaPosOffset.y = (player.y - strafeY * 15) / 64;

		// Movement and collision
		if (key.w) {
			if (mapWalls[deltaPosOffset.y * mapWallsX + deltaPosOffset.x] == 0 && enemyDistance > 20) { // If the x offsets position is not inside wall
				player.x += (playerDelta.x * 0.2) * WALKSPEED * deltaTime / 5;
				player.y += (playerDelta.y * 0.2) * WALKSPEED * deltaTime / 5;
			}
		}

		if (key.s) {
			if (mapWalls[nDeltaPosOffset.y * mapWallsX + nDeltaPosOffset.x] == 0) { // If the x offsets position is not inside wall
				player.x -= (playerDelta.x * 0.2) * WALKSPEED * deltaTime / 5;
				player.y -= (playerDelta.y * 0.2) * WALKSPEED * deltaTime / 5;
			}
		}

		// Strafe left
		if (key.a) {
			if (mapWalls[strafeDeltaPosOffset.y * mapWallsX + strafeDeltaPosOffset.x] == 0) {
				player.x += (strafeX * 0.1) * WALKSPEED * deltaTime / 25;
				player.y += (strafeY * 0.1) * WALKSPEED * deltaTime / 25;
			}
		}

		// Strafe right
		if (key.d) {
			if (mapWalls[nStrafeDeltaPosOffset.y * mapWallsX + nStrafeDeltaPosOffset.x] == 0) {
				player.x -= (strafeX * 0.1) * WALKSPEED * deltaTime / 25;
				player.y -= (strafeY * 0.1) * WALKSPEED * deltaTime / 25;
			}
		}

		if (key.f) {
			blocking = true;
		}
		else
		{
			blocking = false;
		}
	
		// Attack
		if (key.space) {
			playerAttack();
		}

		glClear(GL_COLOR_BUFFER_BIT);
		drawRays();
		drawSprites();
		drawUI();
	}
	glutPostRedisplay();
	gameHandler();
	glutSwapBuffers();
}

void createAudioBuffer(sf::SoundBuffer& buffer, const std::string& filePath) {
	if (!buffer.loadFromFile(filePath)) {
		std::cout << "Failed to load " + filePath << std::endl;
	}
}

void loadAudio() {

	// Play music from file
	if (!music.openFromFile("audio\\music.ogg")) {
		std::cout << "Failed to open music.ogg" << std::endl;
	};

	createAudioBuffer(katanaSwingBuff, "audio\\katanaSwing.ogg");
	katanaSwing.setBuffer(katanaSwingBuff);

	createAudioBuffer(hitDeadBuff, "audio\\hitDead.ogg");
	hitDead.setBuffer(hitDeadBuff);

	createAudioBuffer(hitSlash1Buff, "audio\\hitSlash1.ogg");
	hitSlash4.setBuffer(hitSlash1Buff);

	createAudioBuffer(hitSlash2Buff, "audio\\hitSlash2.ogg");
	hitSlash2.setBuffer(hitSlash2Buff);

	createAudioBuffer(hitSlash3Buff, "audio\\hitSlash3.ogg");
	hitSlash3.setBuffer(hitSlash3Buff);

	createAudioBuffer(slash1Buff, "audio\\slash1.ogg");
	slash1.setBuffer(slash1Buff);

	createAudioBuffer(slash2Buff, "audio\\slash2.ogg");
	slash2.setBuffer(slash2Buff);

	createAudioBuffer(gongBuff, "audio\\gong.ogg");
	gong.setBuffer(gongBuff);

	createAudioBuffer(introBuff, "audio\\intro.ogg");
	intro.setBuffer(introBuff);

	createAudioBuffer(medkitBuff, "audio\\medkit.ogg");
	medkit.setBuffer(medkitBuff);

	createAudioBuffer(enemyHitBuff, "audio\\enemyHit.ogg");
	enemyHit.setBuffer(enemyHitBuff);

	createAudioBuffer(enemyDeathBuff, "audio\\enemyDeath.ogg");
	enemyDeath.setBuffer(enemyDeathBuff);

	createAudioBuffer(swordClashBuff, "audio\\swordClash.ogg");
	swordClash.setBuffer(swordClashBuff);

	intro.play();
}

void resize(int w, int h) {
	glutReshapeWindow(WIDTH, HEIGHT);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	glutInit(&__argc, __argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(glutGet(GLUT_SCREEN_WIDTH) / 2 - WIDTH / 2, glutGet(GLUT_SCREEN_HEIGHT) / 2 - HEIGHT /  2);
	glutCreateWindow("Dojo Duel");

	gluOrtho2D(0, WIDTH, HEIGHT, 0);
	loadAudio();
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(resize);

	// Keyboard
	glutKeyboardFunc(buttonDown);
	glutKeyboardUpFunc(buttonUp);

	// Mouse
	glutPassiveMotionFunc(mouseMovementHandler);

	glutMainLoop();
	return 0;
}