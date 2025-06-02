#include "Chunk.h"
#include "Block.h"
#include"agk.h"

Chunk::Chunk(const int x, const int y)
{
	chunkX = x;
	chunkY = y;

	//Initiate chunk datastructures
	blockID.resize(WIDTH);
	metadata.resize(WIDTH);

	for (int x = 0; x < WIDTH; x++)
	{
		blockID[x].resize(HEIGHT);
		metadata[x].resize(HEIGHT);

		for (int y = 0; y < WIDTH; y++)
		{
			blockID[x][y] = 0;	//Zero out on create
			metadata[x][y] = 0;
		}
	}

	GenerateTerrain();
	chunkImage = GenerateImage();
	chunkSprite = agk::CreateSprite(chunkImage);
	agk::SetSpritePosition(chunkSprite, (float)(chunkX * WIDTH * 16), (float)(chunkY * HEIGHT * 16));
	agk::SetSpriteDepth(chunkSprite, 2);
	agk::SetSpriteOffset(chunkSprite, 0.0f, 0.0f);
}

Chunk::~Chunk()
{
	agk::DeleteSprite(chunkSprite);
	agk::DeleteImage(chunkImage);
}

void Chunk::Update()
{

}

BlockID Chunk::GetBlock(const int x, const int y)
{
	return blockID[x][y];
}
Metadata Chunk::GetMetadata(const int x, const int y)
{
	return metadata[x][y];
}

void Chunk::SetBlock(const int x, const int y, const BlockID block)
{
	blockID[x][y] = block;
}
void Chunk::SetMetadata(const int x, const int y, const Metadata data)
{
	metadata[x][y] = data;
}

//Fill with random values rn
void Chunk::GenerateTerrain()
{
	for (int x = 0; x < WIDTH;x++)
	{
		int threshhold = (int)(sin(((chunkX * WIDTH) + x) / 90.0f) * 32.0f) + 60; //Temporary sin wave for rolling hills

		for (int y = 0; y < HEIGHT;y++)
		{
			int position = (chunkY * HEIGHT) + y;

			if(position >= threshhold+17)	
				blockID[x][y] = ID::Stone;
			else if (position >= threshhold + 1)
				blockID[x][y] = ID::Dirt;
			else if (position >= threshhold)
				blockID[x][y] = ID::Grass;

		}
	}
}

unsigned int Chunk::GenerateImage()
{
	//Create dummy sprite
	unsigned int blockSprite = agk::CreateSprite(0);
	agk::SetSpriteSize(blockSprite, 16.0f, 16.0f);
	agk::SetSpriteOffset(blockSprite, 0.0f, 0.0f);

	//Stores final image
	unsigned int renderImage = agk::CreateRenderImage(512, 512, 0, 0);


	//store previous settings
	float prev_x = agk::GetViewOffsetX();
	float prev_y = agk::GetViewOffsetY();
	agk::SetViewOffset(0.0f, 0.0f);

	//Draw to render image
	agk::SetRenderToImage(renderImage, 0);
	agk::SetVirtualResolution(512, 512);

	agk::ClearScreen();
	
	for (int x = 0; x < WIDTH;x++)
	{
		for (int y = 0; y < HEIGHT;y++)
		{
			if (blockID[x][y] != ID::Air)
			{
				agk::SetSpriteImage(blockSprite, Block::GetImage(blockID[x][y]));
				agk::SetSpritePosition(blockSprite, x * 16.0f, y * 16.0f);
				agk::DrawSprite(blockSprite);
			}
		}
	}

	//Reset screen settings
	agk::SetRenderToScreen();
	agk::SetVirtualResolution(640, 360);
	agk::SetViewOffset(prev_x, prev_y);

	//Cleanup
	agk::DeleteSprite(blockSprite);

	return renderImage;
}