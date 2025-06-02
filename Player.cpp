#include "Player.h"
#include "agk.h"
#include "Block.h"
#include "ScanCodes.h"
#include <cmath>

Player::Player(World* spawn_world, const float spawn_x, const float spawn_y)
{
	world = spawn_world;

	xSpeed = 0.0f;
	ySpeed = 0.0f;

	x = spawn_x;
	y = spawn_y;

	playerImage = agk::LoadImage("player.png");
	playerSprite = agk::CreateSprite(playerImage);
	agk::SetSpritePosition(playerSprite, x, y);
	agk::SetSpriteDepth(playerSprite,1);
	agk::SetSpriteOffset(playerSprite,agk::GetImageWidth(playerImage) / 2.0f, agk::GetImageHeight(playerImage) / 2.0f);

	highlightImage = agk::LoadImage("highlight.png");
	highlightSprite = agk::CreateSprite(highlightImage);
	agk::SetSpriteDepth(highlightSprite, 1);
}

Player::~Player()
{
	agk::DeleteSprite(playerSprite);
	agk::DeleteImage(playerImage);

	agk::DeleteSprite(highlightSprite);
	agk::DeleteImage(highlightImage);
}

void Player::Update()
{
	//Movement Controls
	if (agk::GetRawKeyState(AGKEY_D))	xSpeed =  4.0f;
	if (agk::GetRawKeyState(AGKEY_A))	xSpeed = -4.0f;
	if (agk::GetRawKeyState(AGKEY_S))	ySpeed =  4.0f;
	if (agk::GetRawKeyState(AGKEY_W))	ySpeed = -4.0f;

	//Aiming
	int block_x = (int)std::floorf(agk::ScreenToWorldX(agk::GetRawMouseX()) / Block::GetSize());
	int block_y = (int)std::floorf(agk::ScreenToWorldY(agk::GetRawMouseY()) / Block::GetSize());

	if (agk::GetRawMouseLeftState())
	{
		world->SetBlock(block_x, block_y, ID::Air);
	}

	if (agk::GetRawMouseRightState())
	{
		world->SetBlock(block_x, block_y, ID::Stone);
	}
	

	//Movement
	x += xSpeed;
	y += ySpeed;

	xSpeed = 0.0f;
	ySpeed = 0.0f;

	//Drawing
	agk::SetSpritePosition(playerSprite, x, y);
	agk::SetSpritePosition(highlightSprite, block_x * Block::GetSize(), block_y * Block::GetSize());

	//Camera
	agk::SetViewOffset(x - (float)agk::GetVirtualWidth() / 2.0f, y - (float)agk::GetVirtualHeight() / 2.0f);
}


float Player::GetX()
{
	return x;
}
float Player::GetY()
{
	return y;
}