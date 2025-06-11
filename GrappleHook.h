#ifndef _H_GRAPPLE_HOOK
#define _H_GRAPPLE_HOOK

#include "Collider.h"
#include "World.h"

class GrappleHook
{
	public:
		GrappleHook(World* world, float x, float y, float angle);
		~GrappleHook();
		void Update();
		void SetPlayerPosition(float x, float y);
		float GetX() const;
		float GetY() const;
		bool isMoving() const;
		bool isHooked() const;

	private:
		Collider* collider;
		World* world;

		float startX;
		float startY;

		float playerX;
		float playerY;

		float x;
		float y;

		float length;
		float speed;

		bool moving;
		bool hooked;
		bool retracting;
		
		unsigned int hookImage;
		unsigned int hookSprite;
};

#endif