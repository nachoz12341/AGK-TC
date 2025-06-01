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

	//Temporary drawing
	chunkImage = agk::CreateImageColor(rand()%256, rand() % 256, rand() % 256, 255);
	agk::ResizeImage(chunkImage, WIDTH * 16, HEIGHT * 16);
	chunkSprite = agk::CreateSprite(chunkImage);
	agk::SetSpritePosition(chunkSprite, chunkX * WIDTH * 16, chunkY * HEIGHT * 16);
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