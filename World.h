#ifndef _H_WORLD
#define _H_WORLD

#include <vector>

#include "Chunk.h"

class World {
	public:
		World();
		~World();
		void Update();
	
	private:
		std::vector<std::vector<Chunk*>> chunkGrid;
		int width = 5;	//In chunks
		int height = 5;
};


#endif