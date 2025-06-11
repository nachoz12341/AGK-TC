#include "GrappleHook.h"

#include "agk.h"

GrappleHook::GrappleHook(World* world, float x, float y, float angle)
{
	this->world = world;
	
	startX = x;
	startY = y;

	playerX = x;
	playerY = y;

	this->x = x;
	this->y = y;

	length = 256.0f;
	speed = 10.0f;

	moving = true;
	hooked = false;
	retracting = false;

	collider = new Collider(world, 2.0f, 2.0f);
	collider->SetPosition(this->x, this->y);
	collider->SetXSpeed(10.0f * cosf(angle));
	collider->SetYSpeed(10.0f * sinf(angle));
	collider->SetFriction(1.0f); // No friction for the hook

	hookImage = agk::LoadImage("grapple_hook.png");
	hookSprite = agk::CreateSprite(hookImage);
	agk::SetSpriteAngleRad(hookSprite, angle);
}

GrappleHook::~GrappleHook()
{
	delete collider;
	agk::DeleteSprite(hookSprite);
	agk::DeleteImage(hookImage);
}

void GrappleHook::Update()
{
	//Grab latest position
	x = collider->GetX();
	y = collider->GetY();

	//Update drawing
	agk::SetSpritePosition(hookSprite, x - agk::GetSpriteWidth(hookSprite)/2.0f, y - agk::GetSpriteHeight(hookSprite)/2.0f);
	agk::DrawLine(agk::WorldToScreenX(playerX), agk::WorldToScreenY(playerY), agk::WorldToScreenX(x), agk::WorldToScreenY(y), 0, 0, 0); // Draw a line from the start position to the hook position

	//Check if we reached our distance limit
	float distance = sqrtf(powf(x - startX, 2) + powf(y - startY, 2));

	if (distance >= length || retracting)
	{
		float angle = atan2f(playerY - y, playerX - x); // Calculate angle to player
		collider->SetXSpeed(speed * cosf(angle)); // Move towards player
		collider->SetYSpeed(speed * sinf(angle)); 
		retracting = true;
	}

	//Stop retracting if close enough to the player
	distance = sqrtf(powf(playerX - x, 2) + powf(playerY - y, 2));
	if (distance <= 16.0f && retracting)
	{
		moving = false; 
	}

	//Don't update collider if not moving
	if (!moving) 
		return;
	
	collider->Step();

	if (collider->GetCollided())
	{
		moving = false; // Deactivate the hook if it collides with something
		hooked = true; 
	}	
}

void GrappleHook::SetPlayerPosition(float x, float y)
{
	playerX = x;
	playerY = y;
}

float GrappleHook::GetX() const
{
	return x;
}

float GrappleHook::GetY() const
{
	return y;
}

bool GrappleHook::isMoving() const
{
	return moving;
}

bool GrappleHook::isHooked() const
{
	return hooked;
}