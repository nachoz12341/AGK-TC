#ifndef _H_WORLD
#define _H_WORLD

#include <vector>

#include "Chunk.h"

class World {
	public:
		World();
		~World();
		void Update();
		void SetOriginChunk(int x, int y);

	private:
		std::vector<std::vector<Chunk*>> chunkGrid;
		int width = 5;	//In chunks
		int height = 5;

		int originX; //Controls which chunk is our center chunk
		int originY;
};


#endif