#ifndef _H_PLAYER
#define _H_PLAYER

class Player {
	public:
		Player(float spawn_x, float spawn_y);
		~Player();
		void Update();

	private:
		float xSpeed;
		float ySpeed;
		float x;
		float y;

		unsigned int playerImage;
		unsigned int playerSprite;
};

#endif 
