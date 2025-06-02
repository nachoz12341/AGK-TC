#include "Collider.h"

#include "Chunk.h"
#include "Block.h"

#include <cmath>


Collider::Collider(World* world, const float width, const float height)
{
	this->world = world;

	this->width = width;
	this->height = height;	

	x = 0.0f;
	y = 0.0f;

	xSpeed = 0.0f;
	ySpeed = 0.0f;

	friction = 0.0f;
	gravity = 0.25f;
	terminalVelocity = 7.5f;
	
	onGround = false;
}

Collider::~Collider()
{

}

float Collider::GetX() const
{
	return x;
}

float Collider::GetY() const
{
	return y;
}

bool Collider::GetOnGround() const
{
	return onGround;
}

void Collider::SetPosition(float x, float y)
{
	this->x = x;
	this->y = y;
}

void Collider::SetXSpeed(float speed)
{
	xSpeed = speed;
}
void Collider::SetYSpeed(float speed)
{
	ySpeed = speed;
}

void Collider::Update()
{
	ySpeed += gravity;

	int chunk_x = (int)floor(x / Chunk::GetWidth() / Block::GetSize());
	int chunk_y = (int)floor(y / Chunk::GetHeight() / Block::GetSize());

	Chunk* chunk = world->GetChunk(chunk_x, chunk_y);

	if (chunk == NULL)
	{
		x += xSpeed;
		y += ySpeed;
		return;
	}

	//Subdivide bouding box
	int xBlocks = (int)std::ceil(width / Block::GetSize());
	int yBlocks = (int)std::ceil(height / Block::GetSize());

	//Create vectors for storing subdivided block coordinates    
	std::vector<std::vector<int>> xCoords(2, std::vector<int>(xBlocks + 1));
	std::vector<std::vector<int>> yCoords(2, std::vector<int>(yBlocks + 1));

	//Fill origin positions
	for (int i = 0; i <= xBlocks; i++)
		xCoords[0][i] = (int)floor(((x - (width/ 2.0f) + (width / xBlocks) * i) / Block::GetSize()) - (chunk_x * Chunk::GetWidth()));

	for (int i = 0; i <= yBlocks; i++)
		yCoords[0][i] = (int)floor(((y - (height / 2.0f) + (height / yBlocks) * i) / Block::GetSize()) - (chunk_y * Chunk::GetHeight()));


	//Check x direction   
	float x_step = 0.1f;

	//Check points between where we are now and where we want to be
	while (x_step < abs(xSpeed))
	{
		//Calculate future x positions
		for (int i = 0; i <= xBlocks; i += xBlocks)
			xCoords[1][i] = (int)floor(((x - (width / 2.0f) + ((width / xBlocks) * i) + (x_step * Sign(xSpeed))) / Block::GetSize()) - (chunk_x * Chunk::GetWidth()));

		int iX = 0;
		int iY = 0;

		if (DetectCollision(chunk, xCoords[1], yCoords[0], CollisionAxis::xAxis, iX, iY))
		{
			//No longer moving
			xSpeed = 0.0f;

			//Collision Response
			bool blockSide = (iX / xBlocks) == 0; //Which side of the block did we collide against

			if (blockSide)
				x = ((chunk_x * Chunk::GetWidth()) + xCoords[1][iX] + 1) * Block::GetSize() + (width / 2.0f) + 0.01f;
			else
				x = ((chunk_x * Chunk::GetWidth()) + xCoords[1][iX]) * Block::GetSize() - (width / 2.0f) - 0.01f;

			break;
		}
		else
			x += 0.1f * Sign(xSpeed);   //Move if not touching anything

		x_step += 0.1f;
	}

	//Fill origin positions
	for (int i = 0; i <= xBlocks; i++)
		xCoords[0][i] = (int)floor(((x - (width / 2.0f) + (width / xBlocks) * i) / Block::GetSize()) - (chunk_x * Chunk::GetWidth()));

	//Check Y direction   
	float y_step = 0.1f;

	onGround = false;

	while (y_step < abs(ySpeed))
	{
		//Recalculate future x positions
		for (int i = 0; i <= yBlocks; i += yBlocks)
		{
			yCoords[1][i] = (int)floor(((y - (height / 2.0f) + ((height / yBlocks) * i) + (y_step * Sign(ySpeed))) / Block::GetSize()) - (chunk_y * Chunk::GetHeight()));
		}

		int iX = 0;
		int iY = 0;

		if (DetectCollision(chunk, xCoords[0], yCoords[1], CollisionAxis::yAxis, iX, iY))
		{
			//No longer moving
			ySpeed = 0.0f;

			//Collision Response
			bool blockSide = (iY / yBlocks) == 0; //Which side of the block did we collide against

			if (blockSide)
				y = ((chunk_y * Chunk::GetHeight()) + yCoords[1][iY] + 1) * Block::GetSize() + (height / 2.0f) + 0.01f;
			else
			{
				y = ((chunk_y * Chunk::GetHeight()) + yCoords[1][iY]) * Block::GetSize() - (height / 2.0f) - 0.01f;
				onGround = true;
			}

			break;
		}
		else
			y += 0.1f * Sign(ySpeed);   //Move if not touching anything

		y_step += 0.1f;
	}

	for (int i = 0; i <= yBlocks; i++)
		yCoords[0][i] = (int)floor(((y - (height / 2.0f) + (height / yBlocks) * i) / Block::GetSize()) - (chunk_y * Chunk::GetHeight()));

	xSpeed *= friction;
}

int Collider::Sign(float val)
{
	return (float(0) < val) - (val < float(0));
}

bool Collider::DetectCollision(Chunk* chunk, std::vector<int>& xCoords, std::vector<int>& yCoords, CollisionAxis axis, int& x, int& y)
{
	int xIt = axis == CollisionAxis::xAxis ? (int)xCoords.size() - 1 : 1;
	int yIt = axis == CollisionAxis::yAxis ? (int)yCoords.size() - 1 : 1;

	//Check if there's any blocks in the way
	for (int iX = 0; iX < xCoords.size(); iX += xIt)
		for (int iY = 0; iY < yCoords.size(); iY += yIt)
			{
				if (world->GetBlock((chunk->GetX() * chunk->GetWidth()) + xCoords[iX], (chunk->GetY() * chunk->GetHeight()) + yCoords[iY]) != ID::Air)
				{
					x = iX;
					y = iY;
					return true;
				}
			}
	return false;
}