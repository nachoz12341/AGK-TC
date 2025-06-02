#ifndef _H_CHUNK
#define _H_CHUNK

#include <vector>
#include "Block.h"

class Chunk {
	public:
		Chunk(int x, int y);
		~Chunk();
		void Update();
		BlockID GetBlock(int x, int y);
		Metadata GetMetadata(int x, int y);
		void SetBlock(int x, int y, BlockID block);
		void SetMetadata(int x, int y, Metadata data);
	private:
		const int WIDTH = 32;	//In blocks
		const int HEIGHT = 32;

		std::vector<std::vector<BlockID>> blockID;
		std::vector<std::vector<Metadata>> metadata;

		unsigned int chunkImage;
		unsigned int chunkSprite;

		int chunkX;	//Chunk coordinates in world
		int chunkY;
		
		void GenerateTerrain();
		unsigned int GenerateImage();
};

#endif