#include "Player.h"
#include "agk.h"
#include "Block.h"
#include "ScanCodes.h"
#include "Util.h"
#include <cmath>
#include <string>

Player::Player(World* spawn_world, const float spawn_x, const float spawn_y)
{
	world = spawn_world;
	collider = new Collider(world, 28.0f, 44.0f);
	grappleHook = nullptr;

	x = spawn_x;
	y = spawn_y;
	collider->SetPosition(x, y);
	collider->SetGravity(0.25f);

	playerImage = agk::LoadImage("player.png");
	playerSprite = agk::CreateSprite(playerImage);
	playerShader = agk::LoadSpriteShader("Player.ps");
	agk::SetSpritePhysicsOff(playerSprite);	//Don't need built in physics
	agk::SetSpritePosition(playerSprite, x, y);
	agk::SetSpriteDepth(playerSprite,1);
	agk::SetSpriteShader(playerSprite, playerShader);

	highlightImage = agk::LoadImage("highlight.png");
	highlightSprite = agk::CreateSprite(highlightImage);
	agk::SetSpriteDepth(highlightSprite, 1);
}

Player::~Player()
{
	agk::DeleteSprite(playerSprite);
	agk::DeleteImage(playerImage);
	agk::DeleteShader(playerShader);
	agk::DeleteSprite(highlightSprite);
	agk::DeleteImage(highlightImage);
	delete collider;
}

void Player::Update()
{
	float x_speed = 0.0f;
	bool hooked = false;

	//Movement Controls
	if (agk::GetRawKeyState(AGKEY_D))	x_speed = 4.0f;
	if (agk::GetRawKeyState(AGKEY_A))	x_speed = -4.0f;

	//Grappling
	if (grappleHook == nullptr)
	{
		//Create hook
		if (agk::GetRawKeyPressed(AGKEY_Q))
		{
			float angle = atan2f(agk::ScreenToWorldY(agk::GetRawMouseY()) - y, agk::ScreenToWorldX(agk::GetRawMouseX()) - x);
			grappleHook = new GrappleHook(world, x, y, angle);
		}
	}
	else
	{
		grappleHook->SetPlayerPosition(x, y);
		grappleHook->Update();

		if (agk::GetRawKeyPressed(AGKEY_Q))
		{
			delete grappleHook; // Release the hook if Q is pressed again
			grappleHook = nullptr; // Set to nullptr to indicate no active hook	

			float angle = atan2f(agk::ScreenToWorldY(agk::GetRawMouseY()) - y, agk::ScreenToWorldX(agk::GetRawMouseX()) - x);
			grappleHook = new GrappleHook(world, x, y, angle);
		}

		if (grappleHook->isHooked())
		{
			float angle = atan2f(grappleHook->GetY() - y, grappleHook->GetX() - x); // Calculate angle to the hook
			collider->SetXSpeed(10.0f * cosf(angle)); // Apply the hook's position to the player
			collider->SetYSpeed(10.0f * sinf(angle)); // Apply the hook's position to the player
			x_speed = 0.0f; // Stop horizontal movement while hooked
			hooked = true;

			if(collider->GetCollided())
				doubleJumped = false;

			if (agk::GetRawKeyPressed(AGKEY_SPACE))
			{
				delete grappleHook; // Release the hook if Q is pressed again
				grappleHook = nullptr; // Set to nullptr to indicate no active hook	
			}
		}
		else if (!grappleHook->isMoving())
		{
			// If the hook is not moving, it means it has either reached its maximum length or hit something
			delete grappleHook; // Release the hook if it stops moving
			grappleHook = nullptr; // Set to nullptr to indicate no active hook	
		}
	}
	
	if (x_speed != 0.0f)
		collider->SetXSpeed(x_speed);

	if (collider->GetOnGround() && doubleJumped)
		doubleJumped = false;	//Reset double jump when on ground

	if (agk::GetRawKeyPressed(AGKEY_SPACE) && (collider->GetOnGround() || !doubleJumped))
	{
		if (!collider->GetOnGround() && !hooked)
			doubleJumped = true;	//Allow double jump only once

		collider->SetYSpeed(-6.0f);
	}

	

	//Zoom
	float mouse_delta = agk::GetRawMouseWheelDelta();

	if (mouse_delta != 0.0f)
	{
		float current_zoom = agk::GetViewZoom();
		agk::SetViewZoom(std::min(std::max(current_zoom + mouse_delta * 0.0625f, 1.0f), 4.0f));
	}

	if (agk::GetRawKeyPressed(AGKEY_EQUAL))
	{
		agk::SetViewZoom(2.0f);	//Reset to base zoom
	}

	//Aiming
	int block_x = World::PixelToWorldCoordX(agk::ScreenToWorldX(agk::GetRawMouseX()));
	int block_y = World::PixelToWorldCoordY(agk::ScreenToWorldY(agk::GetRawMouseY()));

	if (agk::GetRawMouseLeftState() && world->GetBlock(block_x, block_y) != ID::Air)
	{
		world->SetBlock(block_x, block_y, ID::Air);
	}

	if (agk::GetRawMouseRightState() && world->GetBlock(block_x, block_y) == ID::Air)
	{
		world->SetBlock(block_x, block_y, ID::Torch);
	}

	if (agk::GetRawMouseMiddlePressed() && world->GetBlock(block_x, block_y) == ID::Air)
	{
		world->SetBlock(block_x, block_y, ID::Platform);
	}

	if (agk::GetRawKeyPressed(AGKEY_UP))
	{
		collider->SetPosition(x, y-32*16);	//Update collider position
	}
	


	//Movement
	collider->Step();
	x = collider->GetX();
	y = collider->GetY();

	std::string s = "Player Position: " + std::to_string(World::PixelToWorldCoordX(x)) + ", " + std::to_string(World::PixelToWorldCoordY(y));
	agk::Print(s.c_str());

	//Drawing
	agk::SetSpritePosition(playerSprite, x - agk::GetImageWidth(playerImage) / 2.0f, y - agk::GetImageHeight(playerImage) / 2.0f);
	agk::SetSpritePosition(highlightSprite, block_x * Block::GetSize(), block_y * Block::GetSize());

	//Camera
	float cam_x = Lerp(agk::GetViewOffsetX(), x - (float)agk::GetVirtualWidth()  / 2.0f, 0.5f);
	float cam_y = Lerp(agk::GetViewOffsetY(), y - (float)agk::GetVirtualHeight() / 2.0f, 0.5f);
	agk::SetViewOffset(cam_x, cam_y);
	
	//Get the shadowmap to darken the player
	agk::SetSpriteAdditionalImage(playerSprite, world->GetShadowImage(), 1);
	agk::SetShaderConstantByName(playerShader, "playerOffset", agk::WorldToScreenX(x) - agk::GetSpriteOffsetX(playerSprite), agk::WorldToScreenY(y) - agk::GetSpriteOffsetY(playerSprite), 0, 0);
}


float Player::GetX() const
{
	return x;
}
float Player::GetY() const
{
	return y;
}
