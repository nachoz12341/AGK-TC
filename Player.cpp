#include "Player.h"
#include "agk.h"


Player::Player(const float spawn_x, const float spawn_y)
{
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
	//Controls
	if (agk::GetRawKeyState(68))	xSpeed =  4.0f;
	if (agk::GetRawKeyState(65))	xSpeed = -4.0f;
	if (agk::GetRawKeyState(83))	ySpeed =  4.0f;
	if (agk::GetRawKeyState(87))	ySpeed = -4.0f;

	//Movement
	x += xSpeed;
	y += ySpeed;

	xSpeed = 0.0f;
	ySpeed = 0.0f;


	//Drawing
	agk::SetSpritePosition(playerSprite, x, y);

	//Camera
	agk::SetViewOffset(x - agk::GetWindowWidth() / 2, y - agk::GetWindowHeight() / 2);
}
