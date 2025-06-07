#include "Chunk.h"
#include "Block.h"
#include"agk.h"

#include <string>

Chunk::Chunk(const int x, const int y)
{
	chunkX = x;
	chunkY = y;

	edgeChunk = false;

	//Initiate chunk datastructures
	blockID.resize(WIDTH);
	metadata.resize(WIDTH);
	light.resize(WIDTH);
	backgroundID.resize(WIDTH);

	for (int x = 0; x < WIDTH; x++)
	{
		blockID[x].resize(HEIGHT);
		metadata[x].resize(HEIGHT);
		light[x].resize(HEIGHT);
		backgroundID[x].resize(HEIGHT);

		for (int y = 0; y < HEIGHT; y++)
		{
			blockID[x][y] = 0;	//Zero out on create
			metadata[x][y] = 0;
			light[x][y] = 0;
			backgroundID[x][y] = 0;
		}
	}
	
	terrainImage = agk::CreateRenderImage(512, 512, 0, 0);
	shadowImage = agk::CreateRenderImage(512, 512, 0, 0);
}

Chunk::~Chunk()
{
	agk::DeleteImage(terrainImage);
	agk::DeleteImage(shadowImage);
}

int Chunk::GetX() const
{
	return chunkX;
}

int Chunk::GetY() const
{
	return chunkY;
}

bool Chunk::GetEdgeChunk() const
{
	return edgeChunk;
}

int Chunk::GetWidth()
{
	return WIDTH;
}

int Chunk::GetHeight()
{
	return HEIGHT;
}

void Chunk::SetEdgeChunk(bool onEdge)
{
	edgeChunk = onEdge;
}

bool Chunk::LightQueueEmpty() const
{
	return lightQueue.empty();
}

std::array<int, 2> Chunk::LightQueuePop()
{
	std::array<int, 2> coord = lightQueue.front();
	lightQueue.pop();
	return coord;
}

void Chunk::LightQueuePush(const int x, const int y)
{
	lightQueue.push({ x,y });
}

bool Chunk::RemoveLightQueueEmpty() const
{
	return removeLightQueue.empty();
}

std::array<int, 2> Chunk::RemoveLightQueuePop()
{
	std::array<int, 2> coord = removeLightQueue.front();
	removeLightQueue.pop();
	return coord;
}

void Chunk::RemoveLightQueuePush(const int x, const int y)
{
	removeLightQueue.push({ x,y });
}


void Chunk::Tick()
{
	unsigned int color = agk::MakeColor(255,255,255);
	agk::DrawBox(chunkX * WIDTH *Block::GetSize() + agk::GetViewOffsetX(), chunkY * HEIGHT * Block::GetSize(), (chunkX + 1) * WIDTH * Block::GetSize() + agk::GetViewOffsetX(), (chunkY + 1) * HEIGHT * Block::GetSize(), color, color, color, color, false);
}

void Chunk::UpdateImage()
{
	GenerateImage();
	UpdateShadow();
}

void Chunk::UpdateShadow()
{
	GenerateShadow();
}

BlockID Chunk::GetBlock(const int x, const int y) const
{
	return blockID[x][y];
}

Metadata Chunk::GetMetadata(const int x, const int y) const
{
	return metadata[x][y];
}

Light Chunk::GetLight(const int x, const int y) const
{
	return light[x][y];
}

BackgroundID Chunk::GetBackground(const int x, const int y) const
{
	return backgroundID[x][y];
}

unsigned int Chunk::GetTerrainImage() const
{
	return terrainImage;
}

unsigned int Chunk::GetShadowImage() const
{
	return shadowImage;
}

void Chunk::SetBlock(const int x, const int y, const BlockID block)
{
	blockID[x][y] = block;
}

void Chunk::SetMetadata(const int x, const int y, const Metadata data)
{
	metadata[x][y] = data;
}

void Chunk::SetLight(const int x, const int y, const Light light)
{
	this->light[x][y] = light;
}

void Chunk::SetBackground(const int x, const int y, const BackgroundID background)
{
	backgroundID[x][y] = background;
}

void Chunk::GenerateImage()
{
	//Create background image
	unsigned int backgroundImage = agk::CreateImageColor(96, 96, 96, 255);//77, 63, 56, 255);
	agk::ResizeImage(backgroundImage, 16, 16);
	
	//Create dummy sprite
	unsigned int blockSprite = agk::CreateSprite(0);
	agk::SetSpriteSize(blockSprite, 16.0f, 16.0f);
	agk::SetSpriteOffset(blockSprite, 0.0f, 0.0f);

	//store previous settings
	int prev_vres_x = agk::GetVirtualWidth();
	int prev_vres_y = agk::GetVirtualHeight();

	float prev_x = agk::GetViewOffsetX();
	float prev_y = agk::GetViewOffsetY();
	agk::SetViewOffset(0.0f, 0.0f);

	float prev_zoom = agk::GetViewZoom();
	agk::SetViewZoom(1.0f);

	//Draw to render image
	agk::SetRenderToImage(terrainImage, 0);
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
			else if (backgroundID[x][y] != 0)
			{
				agk::SetSpriteImage(blockSprite, backgroundImage);
				agk::SetSpritePosition(blockSprite, x * 16.0f, y * 16.0f);
				agk::DrawSprite(blockSprite);
			}
			
		}
	}

	//Reset screen settings
	agk::SetRenderToScreen();
	agk::SetVirtualResolution(prev_vres_x, prev_vres_y);
	agk::SetViewOffset(prev_x, prev_y);
	agk::SetViewZoom(prev_zoom);

	//Cleanup
	agk::DeleteSprite(blockSprite);
	agk::DeleteImage(backgroundImage);
}

void Chunk::GenerateShadow()
{
	//Create dummy sprite
	unsigned int blockSprite = agk::CreateSprite(0);
	agk::SetSpriteSize(blockSprite, 16.0f, 16.0f);
	agk::SetSpriteOffset(blockSprite, 0.0f, 0.0f);

	//store previous settings
	int prev_vres_x = agk::GetVirtualWidth();
	int prev_vres_y = agk::GetVirtualHeight();

	float prev_x = agk::GetViewOffsetX();
	float prev_y = agk::GetViewOffsetY();
	agk::SetViewOffset(0.0f, 0.0f);

	float prev_zoom = agk::GetViewZoom();
	agk::SetViewZoom(1.0f);

	//Draw to render image
	agk::SetRenderToImage(shadowImage, 0);
	agk::SetVirtualResolution(512, 512);

	agk::ClearScreen();

	for (int x = 0; x < WIDTH;x++)
	{
		for (int y = 0; y < HEIGHT;y++)
		{
			agk::SetSpriteColor(blockSprite, 255, 255, 255, 255);	//Light

			if (blockID[x][y] != ID::Air || backgroundID[x][y]!=0)
			{
				Light light_level = light[x][y];
				agk::SetSpriteColor(blockSprite, light_level*17, light_level * 17, light_level * 17, 255);	//Shadow
			}


			agk::SetSpritePosition(blockSprite, x * 16.0f, y * 16.0f);
			agk::DrawSprite(blockSprite);
		}
	}

	//Reset screen settings
	agk::SetRenderToScreen();
	agk::SetVirtualResolution(prev_vres_x, prev_vres_y);
	agk::SetViewOffset(prev_x, prev_y);
	agk::SetViewZoom(prev_zoom);

	//Cleanup
	agk::DeleteSprite(blockSprite);
}

unsigned int Chunk::Encode()
{
	int size = WIDTH * HEIGHT * 4;
	size += 4; // lightQueue size
	size += lightQueue.size() * 2 * 4;
	size += 4; // removeLightQueue size
	size += removeLightQueue.size() * 2 * 4;
	unsigned int data = agk::CreateMemblock(size);

	for(int block_x = 0; block_x < WIDTH; block_x++)
	{
		for (int block_y = 0; block_y < HEIGHT; block_y++)
		{
			int offset = (block_x * HEIGHT) + block_y;
			agk::SetMemblockByte(data, offset, blockID[block_x][block_y]);
			agk::SetMemblockByte(data, offset + 1024, metadata[block_x][block_y]);
			agk::SetMemblockByte(data, offset + 2048, light[block_x][block_y]);
			agk::SetMemblockByte(data, offset + 3072, backgroundID[block_x][block_y]);
		}
	}

	int offset = WIDTH * HEIGHT * 4; //Start after the block data
	agk::SetMemblockInt(data, offset, lightQueue.size());
	offset += 4;

	std::queue<std::array<int, 2>> tempLightQueue = lightQueue;
	while (!tempLightQueue.empty())
	{
		std::array<int, 2> coord = tempLightQueue.front();
		tempLightQueue.pop();

		agk::SetMemblockInt(data, offset, coord[0]);
		offset += 4;
		agk::SetMemblockInt(data, offset, coord[1]);
		offset += 4;
	}

	agk::SetMemblockInt(data, offset, removeLightQueue.size());
	offset += 4;

	std::queue<std::array<int, 2>> tempRemoveLightQueue = removeLightQueue;
	while (!tempRemoveLightQueue.empty())
	{
		std::array<int, 2> coord = tempRemoveLightQueue.front();
		tempRemoveLightQueue.pop();

		agk::SetMemblockInt(data, offset, coord[0]);
		offset += 4;
		agk::SetMemblockInt(data, offset, coord[1]);
		offset += 4;
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
			metadata[block_x][block_y] = agk::GetMemblockByte(memblock, offset + 1024);
			light[block_x][block_y] = agk::GetMemblockByte(memblock, offset + 2048);
			backgroundID[block_x][block_y] = agk::GetMemblockByte(memblock, offset + 3072);
		}
	}

	int offset = WIDTH * HEIGHT * 4;

	// LightQueue
	int lightQueueSize = agk::GetMemblockInt(memblock, offset);
	offset += 4;

	for (int i = 0; i < lightQueueSize; i++) 
	{
		int block_x = agk::GetMemblockInt(memblock, offset); 
		offset += 4;
		int block_y = agk::GetMemblockInt(memblock, offset); 
		offset += 4;

		LightQueuePush(block_x, block_y);
	}

	// RemoveLightQueue
	int removeLightQueueSize = agk::GetMemblockInt(memblock, offset);
	offset += 4;

	for (int i = 0; i < removeLightQueueSize; i++) 
	{
		int block_x = agk::GetMemblockInt(memblock, offset); 
		offset += 4;
		int block_y = agk::GetMemblockInt(memblock, offset); 
		offset += 4;

		RemoveLightQueuePush(block_x, block_y);
	}
}