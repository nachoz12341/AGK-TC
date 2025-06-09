#ifndef _H_COLLIDER
#define _H_COLLIDER

#include "World.h"

class Collider {
	public:
		Collider(World* world, float width, float height);
		~Collider();

		float GetX() const;
		float GetY() const;
		bool GetOnGround() const;

		void SetPosition(float x, float y);
		void SetXSpeed(float speed);
		void SetYSpeed(float speed);

		void Step();

	private:
		typedef enum CollisionAxis {
			xAxis,
			yAxis
		}CollisionAxis;

		World* world;

		float width;
		float height;

		float x;
		float y;

		float xSpeed;
		float ySpeed;

		float friction;
		float gravity;
		float terminalVelocity;

		bool onGround;

		bool DetectCollision(Chunk* chunk, std::vector<int>& xCoords, std::vector<int>& yCoords, CollisionAxis axis, int& x, int& y, int dir);
};

#endif