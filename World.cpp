#include "World.h"

World::World()
{
	//Initiate chunk grid
	chunkGrid.resize(width);

	for (int x = 0; x < width;x++)
	{
		chunkGrid[x].resize(height);

		for (int y = 0; y < height;y++)
		{
			chunkGrid[x][y] = new Chunk(x, y);
		}
	}
}

World::~World()
{
	//Free chunk grid
	for (int x = 0; x < width;x++)
	{
		for (int y = 0; y < height;y++)
		{
			delete chunkGrid[x][y];
		}
	}
}

void World::Update()
{
	//Tick chunk grid
	for (int x = 0; x < width;x++)
	{
		for (int y = 0; y < height;y++)
		{
			chunkGrid[x][y]->Update();
		}
	}
}
