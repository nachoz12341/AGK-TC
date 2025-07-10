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
		void SyncFastEncode(std::vector<uint8_t>& outData) const override;
		void SyncFastDecode(std::vector<uint8_t>& inData) override;
		void SyncFastUpdate() override;
		void SyncRPCUpdate() override;
		SyncObjectID GetSyncObjectID() const override;

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

		//Online sync
		float targetX = 0.0f;
		float targetY = 0.0f;
};

#endif 
