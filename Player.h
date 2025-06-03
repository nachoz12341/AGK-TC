#ifndef _H_PLAYER
#define _H_PLAYER

#include "World.h"
#include "Collider.h"

class Player {
	public:
		Player(World* spawn_world, float spawn_x, float spawn_y);
		~Player();
		void Update();
		float GetX() const;
		float GetY() const;

	private:
		World* world;
		Collider* collider;

		float x;
		float y;

		unsigned int playerImage;
		unsigned int playerSprite;
		unsigned int playerShader;

		unsigned int highlightImage;
		unsigned int highlightSprite;		
};

#endif 
