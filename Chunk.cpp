#include "Chunk.h"
#include "Block.h"
#include"agk.h"

#include <string>

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

	std::string file_path = "world/"+std::to_string(chunkX)+"/"+std::to_string(chunkY)+".chnk";
	if (agk::GetFileExists(file_path.c_str()))
	{
		unsigned int loadData = agk::CreateMemblockFromFile(file_path.c_str());
		Decode(loadData);
		agk::DeleteMemblock(loadData);
	}
	else
		GenerateTerrain();

	chunkImage = GenerateImage();
	chunkSprite = agk::CreateSprite(chunkImage);
	agk::SetSpritePosition(chunkSprite, (float)(chunkX * WIDTH * Block::GetSize()), (float)(chunkY * HEIGHT * Block::GetSize()));
	agk::SetSpriteDepth(chunkSprite, 2);
	agk::SetSpriteOffset(chunkSprite, 0.0f, 0.0f);
}

Chunk::~Chunk()
{
	agk::DeleteSprite(chunkSprite);
	agk::DeleteImage(chunkImage);
}

int Chunk::GetX() const
{
	return chunkX;
}

int Chunk::GetY() const
{
	return chunkY;
}

int Chunk::GetWidth()
{
	return WIDTH;
}

int Chunk::GetHeight()
{
	return HEIGHT;
}

void Chunk::Tick()
{

}

void Chunk::UpdateImage()
{
	int tempImage = GenerateImage();
	agk::SetSpriteImage(chunkSprite,tempImage);
	agk::DeleteImage(chunkImage);
	chunkImage = tempImage;
}

BlockID Chunk::GetBlock(const int x, const int y) const
{
	return blockID[x][y];
}
Metadata Chunk::GetMetadata(const int x, const int y) const
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

unsigned int Chunk::Encode()
{
	unsigned int data = agk::CreateMemblock(WIDTH * HEIGHT * 2);

	for(int block_x = 0; block_x < WIDTH; block_x++)
	{
		for (int block_y = 0; block_y < HEIGHT; block_y++)
		{
			int offset = (block_x * HEIGHT) + block_y;
			agk::SetMemblockByte(data, offset, blockID[block_x][block_y]);
			agk::SetMemblockByte(data, offset + 512, metadata[block_x][block_y]);
		}
	}

	return data;
}

void Chunk::Decode(unsigned int memblock)
{
	for (int block_x = 0; block_x < WIDTH; block_x++)
	{
		for (int block_y = 0; block_y < HEIGHT; block_y++)
		{
			int offset = (block_x * HEIGHT) + block_y;
			blockID[block_x][block_y] = agk::GetMemblockByte(memblock, offset);
			metadata[block_x][block_y] = agk::GetMemblockByte(memblock, offset + 512);
		}
	}
}