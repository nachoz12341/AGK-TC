#include "World.h"
#include "agk.h"
#include "Block.h"

World::World()
{
	Block::LoadImages();

	//Initiate chunk grid
	chunkGrid.resize(WIDTH);

	for (int x = 0; x < WIDTH;x++)
	{
		chunkGrid[x].resize(HEIGHT);

		for (int y = 0; y < HEIGHT;y++)
		{
			chunkGrid[x][y] = new Chunk(x, y);
		}
	}

	originX = WIDTH/2;
	originY = HEIGHT/2;
}

World::~World()
{
	//Free chunk grid
	for (int x = 0; x < WIDTH;x++)
	{
		for (int y = 0; y < HEIGHT;y++)
		{
			delete chunkGrid[x][y];
		}
	}

	Block::UnloadImages();
}

//World tick
void World::Update()
{
	//Tick chunk grid
	for (int x = 0; x < WIDTH;x++)
	{
		for (int y = 0; y < HEIGHT;y++)
		{
			chunkGrid[x][y]->Update();
		}
	}
}

//Should be called after movement in order to determine if we need to generate more chunks
void World::SetOriginChunk(const int x, const int y)
{
	//Only proceed if we changed origin
	if (originX == x && originY == y)
		return;

	//Temporarily store old chunks
	std::vector<std::vector<Chunk*>> tempGrid;

	tempGrid.resize(WIDTH);

	for (int chunk_x = 0; chunk_x < WIDTH; chunk_x++)
	{
		tempGrid[chunk_x].resize(HEIGHT);

		for (int chunk_y = 0; chunk_y < HEIGHT;chunk_y++)
		{
			tempGrid[chunk_x][chunk_y] = chunkGrid[chunk_x][chunk_y];
			chunkGrid[chunk_x][chunk_y] = NULL;
		}
	}

	//Move existing chunks to new position
	int diff_x = x - originX;
	int diff_y = y - originY;

	for (int chunk_x = 0; chunk_x < WIDTH; chunk_x++)
	{
		for (int chunk_y = 0; chunk_y < HEIGHT; chunk_y++)
		{
			if (chunk_x + diff_x >= 0 && chunk_x + diff_x < WIDTH && chunk_y + diff_y >= 0 && chunk_y + diff_y < HEIGHT)
			{
				chunkGrid[chunk_x][chunk_y] = tempGrid[chunk_x + diff_x][chunk_y + diff_y];
				tempGrid[chunk_x + diff_x][chunk_y + diff_y] = NULL;
			}
		}
	}


	//Act on remaining chunks
	for (int chunk_x = 0; chunk_x < WIDTH; chunk_x++)
	{
		for (int chunk_y = 0; chunk_y < HEIGHT; chunk_y++)
		{
			//Delete old chunks
			if (tempGrid[chunk_x][chunk_y] != NULL)
			{
				delete tempGrid[chunk_x][chunk_y];
			}

			//Create new chunks
			if (chunkGrid[chunk_x][chunk_y] == NULL)
			{
				chunkGrid[chunk_x][chunk_y] = new Chunk(x - (WIDTH / 2) + chunk_x, y - (HEIGHT/ 2) + chunk_y);
			}
		}
	}

	originX = x;
	originY = y;
}