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
		int WIDTH = 3;	//In chunks
		int HEIGHT = 3;

		int originX; //Controls which chunk is our center chunk
		int originY;
};


#endif