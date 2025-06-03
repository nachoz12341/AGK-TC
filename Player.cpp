#include "Player.h"
#include "agk.h"
#include "Block.h"
#include "ScanCodes.h"
#include "Util.h"
#include <cmath>

Player::Player(World* spawn_world, const float spawn_x, const float spawn_y)
{
	world = spawn_world;
	collider = new Collider(world, 28.0f, 44.0f);

	x = spawn_x;
	y = spawn_y;
	collider->SetPosition(x, y);

	playerImage = agk::LoadImage("player.png");
	playerSprite = agk::CreateSprite(playerImage);
	agk::SetSpritePosition(playerSprite, x, y);
	agk::SetSpriteDepth(playerSprite,1);

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
	delete collider;
}

void Player::Update()
{
	float x_speed = 0.0f;

	//Movement Controls
	if (agk::GetRawKeyState(AGKEY_D))	x_speed = 4.0f;
	if (agk::GetRawKeyState(AGKEY_A))	x_speed = -4.0f;
	
	if (x_speed != 0.0f)
		collider->SetXSpeed(x_speed);

	if (agk::GetRawKeyState(AGKEY_SPACE) && collider->GetOnGround())
		collider->SetYSpeed(-5.25f);

	//Zoom
	float mouse_delta = agk::GetRawMouseWheelDelta();

	if (mouse_delta != 0.0f)
	{
		float current_zoom = agk::GetViewZoom();
		agk::SetViewZoom(std::min(std::max(current_zoom + mouse_delta * 0.0625f, 1.0f), 4.0f));
	}

	if (agk::GetRawMouseMiddlePressed())
	{
		agk::SetViewZoom(2.0f);	//Reset to base zoom
	}

	//Aiming
	int block_x = World::PixelToWorldCoordX(agk::ScreenToWorldX(agk::GetRawMouseX()));
	int block_y = World::PixelToWorldCoordY(agk::ScreenToWorldY(agk::GetRawMouseY()));

	if (agk::GetRawMouseLeftState())
	{
		world->SetBlock(block_x, block_y, ID::Air);
	}

	if (agk::GetRawMouseRightState() && world->GetBlock(block_x, block_y)==ID::Air)
	{
		world->SetBlock(block_x, block_y, ID::Stone);
	}
	

	//Movement
	collider->Step();
	x = collider->GetX();
	y = collider->GetY();

	//Drawing
	agk::SetSpritePosition(playerSprite, x - agk::GetImageWidth(playerImage) / 2.0f, y - agk::GetImageHeight(playerImage) / 2.0f);
	agk::SetSpritePosition(highlightSprite, block_x * Block::GetSize(), block_y * Block::GetSize());

	//Camera
	float cam_x = Lerp(agk::GetViewOffsetX(), x - (float)agk::GetVirtualWidth()  / 2.0f, 0.5f);
	float cam_y = Lerp(agk::GetViewOffsetY(), y - (float)agk::GetVirtualHeight() / 2.0f, 0.5f);
	agk::SetViewOffset(cam_x, cam_y);

}


float Player::GetX() const
{
	return x;
}
float Player::GetY() const
{
	return y;
}
