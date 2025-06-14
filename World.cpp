#include "World.h"
#include "agk.h"
#include "Block.h"
#include <cmath>
#include <string>

World::World()
{
	Block::LoadImages();

	//Set up noise generator
	noiseGenerator.SetSeed(10110101);	//Set a fixed seed for consistent world generation
	noiseGenerator.SetFrequency(0.01f);	
	noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);	

	//Initiate chunk grid
	chunkGrid.resize(WIDTH);

	for (int chunk_x = 0; chunk_x < WIDTH; chunk_x++)
	{
		chunkGrid[chunk_x].resize(HEIGHT);

		for (int chunk_y = 0; chunk_y < HEIGHT; chunk_y++)
		{
			chunkGrid[chunk_x][chunk_y] = new Chunk(chunk_x, chunk_y);
			buildQueue.push(chunkGrid[chunk_x][chunk_y]);

			if (chunk_x == 0 || chunk_y == 0 || chunk_x == WIDTH - 1 || chunk_y == HEIGHT - 1)
				chunkGrid[chunk_x][chunk_y]->SetEdgeChunk(true);
			else
				chunkGrid[chunk_x][chunk_y]->SetEdgeChunk(false);
		}
	}

	originX = WIDTH/2;
	originY = HEIGHT/2;

	terrainImage = agk::CreateRenderImage(agk::GetVirtualWidth(), agk::GetVirtualHeight(), 0, 0);
	agk::SetRenderToImage(terrainImage, 0);	//Clear the render image on create
	agk::ClearScreen();
	agk::SetRenderToScreen();

	shadowImage = agk::CreateRenderImage(agk::GetVirtualWidth(), agk::GetVirtualHeight(), 0, 0);
	agk::SetRenderToImage(shadowImage, 0);	//Clear the render image on create
	agk::ClearScreen();
	agk::SetRenderToScreen();

	worldSprite = agk::CreateSprite(terrainImage);
	agk::SetSpritePhysicsOff(worldSprite);	//Don't need built in physics

	worldShader = agk::LoadSpriteShader("World.ps");
	agk::SetShaderConstantByName(worldShader,"blurSize",2.0,0,0,0);

	agk::SetSpriteShader(worldSprite, worldShader);
	agk::SetSpriteAdditionalImage(worldSprite, shadowImage, 1);
}

World::~World()
{
	//Free chunk grid
	for (int chunk_x = 0; chunk_x < WIDTH;chunk_x++)
	{
		for (int chunk_y = 0; chunk_y < HEIGHT;chunk_y++)
		{
			RemoveChunkFromQueue(buildQueue, chunkGrid[chunk_x][chunk_y]);
			RemoveChunkFromQueue(renderQueue, chunkGrid[chunk_x][chunk_y]);
			delete chunkGrid[chunk_x][chunk_y];
		}
	}

	Block::UnloadImages();
	
	agk::DeleteSprite(worldSprite);
	agk::DeleteImage(terrainImage);
	agk::DeleteImage(shadowImage);
	agk::DeleteShader(worldShader);
}

//World tick
void World::Update()
{
	ProcessShaderChanges();
	ProcessBuildQueue();
	ProcessChunkTicks();
	ProcessRenderQueue();
}

void World::ProcessShaderChanges()
{
	if (agk::GetRawKeyState(AGKEY_PLUS))
		blurSize = blurSize + 0.2;

	if (agk::GetRawKeyState(AGKEY_SUBTRACT))
		blurSize = blurSize - 0.2;

	std::string s = "Blur Size: " + std::to_string(blurSize) + " (Key: +/-)";
	agk::Print(s.c_str());

	agk::SetShaderConstantByName(worldShader, "blurSize", blurSize, 0, 0, 0);
}

void World::ProcessBuildQueue()
{
	//Check queue if a chunk needs to be built
	while (!buildQueue.empty())
	{
		Chunk* chunk = buildQueue.front();
		buildQueue.pop();

		std::string file_path = "world/" + std::to_string(chunk->GetX()) + "/" + std::to_string(chunk->GetY()) + ".chnk";
		if (agk::GetFileExists(file_path.c_str()))
		{
			unsigned int loadData = agk::CreateMemblockFromFile(file_path.c_str());
			chunk->Decode(loadData);
			agk::DeleteMemblock(loadData);
		}
		else
		{
			GenerateTerrain(chunk);
			FillLightQueue(chunk);
		}

		if (!chunk->GetEdgeChunk())
		{
			renderQueue.push(chunk);	//Only add to render queue if not on edge of world
		}
	}
}

void World::ProcessRenderQueue()
{
	while (!renderQueue.empty())
	{
		Chunk* chunk = renderQueue.front();
		renderQueue.pop();
		RemoveLight(chunk);
		GenerateLight(chunk);
		chunk->UpdateImage();
	}
}

void World::ProcessChunkTicks()
{
	//Tick chunk grid
	for (int x = 0; x < WIDTH;x++)
	{
		for (int y = 0; y < HEIGHT;y++)
		{
			chunkGrid[x][y]->Tick();
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
				//Save data
				SaveChunk(tempGrid[chunk_x][chunk_y]);
				RemoveChunkFromQueue(buildQueue, tempGrid[chunk_x][chunk_y]);
				RemoveChunkFromQueue(renderQueue, tempGrid[chunk_x][chunk_y]);
				delete tempGrid[chunk_x][chunk_y];
			}

			//Create new chunks
			Chunk* chunk = chunkGrid[chunk_x][chunk_y];
			if (chunk == NULL)
			{
				chunkGrid[chunk_x][chunk_y] = new Chunk(x - (WIDTH / 2) + chunk_x, y - (HEIGHT/ 2) + chunk_y);
				chunk = chunkGrid[chunk_x][chunk_y];
				buildQueue.push(chunk);
			}

			//Set edge chunk
			if (chunk_x == 0 || chunk_x == WIDTH - 1 || chunk_y == 0 || chunk_y == HEIGHT - 1)
			{
				RemoveChunkFromQueue(renderQueue, chunk);	//Don't render chunks on edge
				chunk->SetEdgeChunk(true);	//Set edge chunk if we are on the edge of the world
			}
			else
			{
				//If was an edge chunk, but now is not, push to render queue
				if (chunk->GetEdgeChunk())
					renderQueue.push(chunk);

				chunk->SetEdgeChunk(false);
			}
		}
	}

	originX = x;
	originY = y;
}

void World::SaveChunk(Chunk* chunk)
{
	//Save data
	unsigned int chunkData = chunk->Encode();
	std::string path = "world/" + std::to_string(chunk->GetX()) + "/" + std::to_string(chunk->GetY()) + ".chnk";
	agk::CreateFileFromMemblock(path.c_str(), chunkData);
	agk::DeleteMemblock(chunkData);
}

Chunk* World::GetChunk(const int x, const int y) const
{
	int array_x = x - chunkGrid[0][0]->GetX();
	int array_y = y - chunkGrid[0][0]->GetY();

	Chunk* chunk = NULL;

	if (array_x >= 0 && array_y >= 0 && array_x < WIDTH && array_y < HEIGHT)
		chunk = chunkGrid[array_x][array_y];
	return chunk;
}

//Takes world space coordinates then puts the chunk in the update queue
void World::SetBlock(const int x, const int y, const BlockID block)
{
	int chunk_x = (int)std::floorf((float)x / (float)Chunk::GetWidth());
	int chunk_y = (int)std::floorf((float)y / (float)Chunk::GetHeight());

	Chunk* chunk = GetChunk(chunk_x, chunk_y);

	//Only act if we found a chunk
	if (chunk != NULL)
	{
		int block_x = x - (chunk_x * Chunk::GetWidth());
		int block_y = y - (chunk_y * Chunk::GetHeight());

		BlockID block_prev = chunk->GetBlock(block_x, block_y);	//Get previous block
		chunk->SetBlock(block_x, block_y, block);

		//If placing a light source
		if (Block::GetLight(block)!=0)
		{
			chunk->SetLight(block_x, block_y, Block::GetLight(block));
			chunk->LightQueuePush(x, y);
		}
		else if (block != ID::Air || Block::GetLight(block_prev) != 0)	//Placing a regular block
		{
			chunk->RemoveLightQueuePush(x, y);
		}
		else //breaking a block
		{
			if (chunk->GetBackground(block_x, block_y) == 0)
				chunk->SetLight(block_x, block_y, 31);
			else
			{
				Light new_light = std::max(GetLight(x + 1, y), std::max(GetLight(x - 1, y), std::max(GetLight(x , y + 1), GetLight(x, y - 1))));
				new_light = std::max(new_light - 1, 0);
				chunk->SetLight(block_x, block_y, new_light);
			}

			chunk->LightQueuePush(x, y);
		}

		renderQueue.push(chunk);
	}
}

BlockID World::GetBlock(const int x, const int y) const
{
	int chunk_x = (int)std::floorf((float)x / (float)Chunk::GetWidth());
	int chunk_y = (int)std::floorf((float)y / (float)Chunk::GetHeight());

	Chunk* chunk = GetChunk(chunk_x, chunk_y);
	BlockID block = 0;

	//Only act if we found a chunk
	if (chunk != NULL)
	{
		block = chunk->GetBlock(x - (chunk_x * Chunk::GetWidth()), y - (chunk_y * Chunk::GetHeight()));
	}

	return block;
}

BlockID World::GetBackground(const int x, const int y) const
{
	int chunk_x = (int)std::floorf((float)x / (float)Chunk::GetWidth());
	int chunk_y = (int)std::floorf((float)y / (float)Chunk::GetHeight());

	Chunk* chunk = GetChunk(chunk_x, chunk_y);
	BlockID block = 0;

	//Only act if we found a chunk
	if (chunk != NULL)
	{
		block = chunk->GetBackground(x - (chunk_x * Chunk::GetWidth()), y - (chunk_y * Chunk::GetHeight()));
	}

	return block;
}

Light World::GetLight(const int x, const int y) const
{
	int chunk_x = (int)std::floorf((float)x / (float)Chunk::GetWidth());
	int chunk_y = (int)std::floorf((float)y / (float)Chunk::GetHeight());

	Chunk* chunk = GetChunk(chunk_x, chunk_y);
	Light light = 0;

	//Only act if we found a chunk
	if (chunk != NULL)
	{
		light = chunk->GetLight(x - (chunk_x * Chunk::GetWidth()), y - (chunk_y * Chunk::GetHeight()));
	}

	return light;
}

void World::SetLight(const int x, const int y, const Light light)
{
	int chunk_x = (int)std::floorf((float)x / (float)Chunk::GetWidth());
	int chunk_y = (int)std::floorf((float)y / (float)Chunk::GetHeight());

	Chunk* chunk = GetChunk(chunk_x, chunk_y);

	//Only act if we found a chunk
	if (chunk != NULL)
	{
		chunk->SetLight(x - (chunk_x * Chunk::GetWidth()), y - (chunk_y * Chunk::GetHeight()), light);
	}

}

int World::PixelToWorldCoordX(const float x)
{
	return (int)std::floorf(x / Block::GetSize());
}

int World::PixelToWorldCoordY(const float y)
{
	return (int)std::floorf(y / Block::GetSize());
}

int World::WorldCoordToChunkX(int x)
{
	return (int)floor((float)x / Chunk::GetWidth());	//Required casts to get proper negative values
}

int World::WorldCoordToChunkY(int y)
{
	return (int)floor((float)y / Chunk::GetHeight());	//Required casts to get proper negative values
}

//Chunk generation steps
void World::GenerateTerrain(Chunk* chunk)
{
	for (int x = 0; x < Chunk::GetWidth();x++)
	{
		int world_x = (chunk->GetX() * Chunk::GetWidth()) + x;
		int threshhold = (int)(sin(world_x / 90.0f) * 32.0f) + 92; //Temporary sin wave for rolling hills

		for (int y = 0; y < Chunk::GetHeight();y++)
		{
			int world_y = (chunk->GetY() * Chunk::GetHeight()) + y;

			BlockID block = ID::Air;

			if (world_y == threshhold-1 && rand() % 100 <= 5)
				block = ID::Daisy;

			if (world_y >= threshhold + 17)
				block = ID::Stone;
			else if (world_y >= threshhold + 1)
				block = ID::Dirt;
			else if (world_y >= threshhold)
				block = ID::Grass;

			if (block != ID::Air)
			{
				noiseGenerator.SetFractalType(FastNoiseLite::FractalType_FBm);
				noiseGenerator.SetFrequency(0.0075f);
				noiseGenerator.SetFractalLacunarity(2.0f);
				float connectingNoise = noiseGenerator.GetNoise((float)(world_x), (float)(world_y));

				noiseGenerator.SetFractalType(FastNoiseLite::FractalType_None);
				noiseGenerator.SetFrequency(0.025f);
				float bigCaveNoise = noiseGenerator.GetNoise((float)(world_x) / 8.0f, (float)(world_y));

				if (abs(connectingNoise) < 0.125f || bigCaveNoise >= .75f)
					block = ID::Air;

				chunk->SetBlock(x, y, block);

				if(world_y>=threshhold)
					chunk->SetBackground(x, y, 1);
			}

		}
	}
}

void World::FillLightQueue(Chunk* chunk)
{
	int world_x = chunk->GetX() * Chunk::GetWidth();
	int world_y = chunk->GetY() * Chunk::GetHeight();

	//Iterate over chunk and add light to all sources
	for (int block_x = 0; block_x < Chunk::GetWidth(); block_x++)
	{
		for (int block_y = 0; block_y < Chunk::GetHeight(); block_y++)
		{
			if (chunk->GetBlock(block_x, block_y) == ID::Air && chunk->GetBackground(block_x, block_y) == 0)
			{
				chunk->SetLight(block_x, block_y, 31);
				chunk->LightQueuePush(world_x+block_x, world_y+block_y);
			}
		}
	}
}

void World::GenerateLight(Chunk* chunk)
{
	// Direction offsets: right, left, down, up
	const int dx[4] = { 1, -1, 0, 0 };
	const int dy[4] = { 0, 0, 1, -1 };

	while (!chunk->LightQueueEmpty())
	{
		std::array<int, 2> coord = chunk->LightQueuePop();
		int block_x = coord[0];
		int block_y = coord[1];

		Light current_light = GetLight(block_x, block_y);

		if (current_light == 0)
			continue;

		int origin_x = chunk->GetX() * Chunk::GetWidth();
		int origin_y = chunk->GetY() * Chunk::GetHeight();

		for (int dir = 0; dir < 4; ++dir)
		{
			int nx = block_x + dx[dir];
			int ny = block_y + dy[dir];

			Chunk* updateChunk = chunk;

			if (nx < origin_x || nx >= origin_x + Chunk::GetWidth() || ny < origin_y || ny >= origin_y + Chunk::GetHeight())
			{
				updateChunk = GetChunk(WorldCoordToChunkX(nx), WorldCoordToChunkY(ny));
			}

			Light new_light = current_light;

			if (Block::GetLightMode(GetBlock(nx, ny)) == LIGHT_OPAQUE)
				new_light = std::max(0, new_light - 2);
			else if (GetBackground(nx, ny) != 0)
				new_light = std::max(0, new_light - 1);

			if (GetLight(nx, ny) < new_light)
			{
				SetLight(nx, ny, new_light);
				updateChunk->LightQueuePush(nx, ny);

				// If we are not in the same chunk, we need to update the render queue
				if (chunk != updateChunk && !updateChunk->GetEdgeChunk())
				{
					renderQueue.push(updateChunk);
				}
			}
		}
	}
}

void World::RemoveLight(Chunk* chunk)
{
	// Direction offsets: right, left, down, up
	const int dx[4] = { 1, -1, 0, 0 };
	const int dy[4] = { 0, 0, 1, -1 };

	while (!chunk->RemoveLightQueueEmpty())
	{
		std::array<int, 2> coord = chunk->RemoveLightQueuePop();
		int block_x = coord[0];
		int block_y = coord[1];

		Light current_light = GetLight(block_x, block_y);
		SetLight(block_x, block_y, 0);

		if (current_light == 0)
			continue;

		int origin_x = chunk->GetX() * Chunk::GetWidth();
		int origin_y = chunk->GetY() * Chunk::GetHeight();

		for (int dir = 0; dir < 4; ++dir)
		{
			int nx = block_x + dx[dir];
			int ny = block_y + dy[dir];

			Chunk* updateChunk = chunk;

			if (nx < origin_x || nx >= origin_x + Chunk::GetWidth() || ny < origin_y || ny >= origin_y + Chunk::GetHeight())
			{
				updateChunk = GetChunk(WorldCoordToChunkX(nx), WorldCoordToChunkY(ny));
			}

			Light neighbor_light = GetLight(nx, ny);

			// Continue removing
			if (neighbor_light != 0 && neighbor_light < current_light)
			{
				updateChunk->RemoveLightQueuePush(nx, ny);
			}
			else if (neighbor_light >= current_light)
			{
				updateChunk->LightQueuePush(nx, ny); // Stop removing and re-spread light
			}

			// If we are not in the same chunk, we need to update the render queue
			if (chunk != updateChunk && !updateChunk->GetEdgeChunk())
				renderQueue.push(updateChunk);
		}
	}
}

template<typename T>
void World::RemoveChunkFromQueue(std::queue<T*>& q, T* ptr)
{
	std::queue<T*> temp;
	while (!q.empty()) {
		if (q.front() != ptr)
			temp.push(q.front());
		q.pop();
	}
	std::swap(q, temp);
}

void World::Render()
{
	RenderChunksToImage(RenderImageType::Terrain);
	RenderChunksToImage(RenderImageType::Shadow);

	agk::SetSpritePosition(worldSprite, agk::GetViewOffsetX(), agk::GetViewOffsetY());
}

void World::RenderChunksToImage(RenderImageType type)
{
	//Create dummy sprite
	unsigned int renderSprite = agk::CreateSprite(0);
	agk::SetSpriteSize(renderSprite, Chunk::GetWidth() * Block::GetSize(), Chunk::GetHeight() * Block::GetSize());
	agk::SetSpriteOffset(renderSprite, 0.0f, 0.0f);

	float prev_x = agk::GetViewOffsetX();
	float prev_y = agk::GetViewOffsetY();
	agk::SetViewOffset(0.0f, 0.0f);

	float prev_zoom = agk::GetViewZoom();
	agk::SetViewZoom(1.0f);

	//Draw to render image
	unsigned int renderImage = (type == RenderImageType::Terrain) ? terrainImage : shadowImage;
	agk::SetRenderToImage(renderImage, 0);

	agk::ClearScreen();

	for (int x = 0; x < WIDTH;x++)
	{
		for (int y = 0; y < HEIGHT;y++)
		{
			Chunk* chunk = chunkGrid[x][y];
			agk::SetSpritePosition(renderSprite, (float)(chunk->GetX() * Chunk::GetWidth() * Block::GetSize())-prev_x, (float)(chunk->GetY() * Chunk::GetHeight() * Block::GetSize())-prev_y);

			unsigned int img = (type == RenderImageType::Terrain) ? chunk->GetTerrainImage() : chunk->GetShadowImage();
			agk::SetSpriteImage(renderSprite, img);
			agk::DrawSprite(renderSprite);			
		}
	}

	//Reset screen settings
	agk::SetRenderToScreen();
	agk::SetViewOffset(prev_x, prev_y);
	agk::SetViewZoom(prev_zoom);

	//Cleanup
	agk::DeleteSprite(renderSprite);
}
