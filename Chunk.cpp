#include "Chunk.h"

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
	agk::ResizeImage(chunkImage, WIDTH * 16, HEIGHT * 16);
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
		for (int y = 0; y < HEIGHT;y++)
		{
			if((chunkY*HEIGHT)+y >= (sin(((chunkX*WIDTH)+x)/90.0f)*32)+60)	//Temporary sin wave for rolling hills
				blockID[x][y] = 1;
		}
	}
}

unsigned int Chunk::GenerateImage()
{
	unsigned int blockImage = agk::CreateImageColor(0, 255, 0, 255);
	agk::ResizeImage(blockImage, 16, 16);
	unsigned int blockSprite = agk::CreateSprite(blockImage);
	agk::SetSpriteOffset(blockSprite, 0.0f, 0.0f);

	unsigned int renderImage = agk::CreateRenderImage(512, 512, 0, 0);


	float prev_x = agk::GetViewOffsetX();
	float prev_y = agk::GetViewOffsetY();
	agk::SetViewOffset(0.0f, 0.0f);

	agk::SetRenderToImage(renderImage, 0);
	agk::SetVirtualResolution(512, 512);

	agk::ClearScreen();
	
	for (int x = 0; x < WIDTH;x++)
	{
		for (int y = 0; y < HEIGHT;y++)
		{
			if (blockID[x][y] > 0)
			{
				agk::SetSpritePosition(blockSprite, x * 16.0f, y * 16.0f);
				agk::DrawSprite(blockSprite);
			}
		}
	}

	agk::SetRenderToScreen();
	agk::SetVirtualResolution(1280, 720);
	agk::SetViewOffset(prev_x, prev_y);

	agk::DeleteSprite(blockSprite);
	agk::DeleteImage(blockImage);

	return renderImage;
}