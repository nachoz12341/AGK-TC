#include "Player.h"
#include "agk.h"


Player::Player()
{
	Player(0.0f, 0.0f);
}

Player::Player(float spawn_x, float spawn_y)
{
	xAccel = 0.01f;
	yAccel = 0.0f;

	xSpeed = 0.0f;
	ySpeed = 0.0f;

	x = spawn_x;
	y = spawn_y;

	playerImage = agk::LoadImage("player.png");
	playerSprite = agk::CreateSprite(playerImage);
	agk::SetSpritePosition(playerSprite, x, y);
}

Player::~Player()
{
	agk::DeleteSprite(playerSprite);
	agk::DeleteImage(playerImage);
}

void Player::Update()
{
	xSpeed += xAccel;
	ySpeed += yAccel;

	x += xSpeed;
	y += ySpeed;

	agk::SetSpritePosition(playerSprite, x, y);
}