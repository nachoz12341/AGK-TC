#ifndef _H_PLAYER
#define _H_PLAYER

#include "World.h"

class Player {
	public:
		Player(World* spawn_world, float spawn_x, float spawn_y);
		~Player();
		void Update();
		float GetX();
		float GetY();

	private:
		World* world;

		float xSpeed;
		float ySpeed;
		float x;
		float y;

		unsigned int playerImage;
		unsigned int playerSprite;

		unsigned int highlightImage;
		unsigned int highlightSprite;
};

#endif 
