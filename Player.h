#ifndef _H_PLAYER
#define _H_PLAYER

#include "SyncObj.h"
#include "World.h"
#include "Collider.h"

class Player : public SyncObj {
	public:
		Player(World* spawn_world, float spawn_x, float spawn_y);
		~Player();
		void Update();
		float GetX() const;
		float GetY() const;

		//UDP Sync Interface
		void SyncEncode(std::vector<uint8_t>& outData) const override;
		void SyncDecode(std::vector<uint8_t>& inData) override;
		void SyncUpdate() override;

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
