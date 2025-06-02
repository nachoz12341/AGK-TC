#ifndef _H_WORLD
#define _H_WORLD

#include <vector>
#include <queue>

#include "Chunk.h"

class World {
	public:
		World();
		~World();
		void Update();
		Chunk* GetChunk(int x, int y) const;
		void SetOriginChunk(int x, int y);
		void SetBlock(int x, int y, BlockID block);
		BlockID GetBlock(int x, int y) const;
		
		static int PixelToWorldCoordX(float x);
		static int PixelToWorldCoordY(float y);
		static int WorldCoordToChunkX(int x);
		static int WorldCoordToChunkY(int y);

	private:
		std::vector<std::vector<Chunk*>> chunkGrid;
		std::queue<Chunk*> updateQueue;
		int WIDTH = 3;	//In chunks
		int HEIGHT = 3;

		int originX; //Controls which chunk is our center chunk
		int originY;
};


#endif