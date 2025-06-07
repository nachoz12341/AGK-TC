#ifndef _H_CHUNK
#define _H_CHUNK

#include "Block.h"

#include <vector>
#include <queue>
#include <array>

class Chunk {
	public:
		Chunk(int x, int y);
		~Chunk();
		void Tick();
		void UpdateImage();
		void UpdateShadow();
		BlockID GetBlock(int x, int y) const;
		Metadata GetMetadata(int x, int y) const;
		Light GetLight(int x, int y) const;
		BackgroundID GetBackground(int x, int y) const;

		void SetBlock(int x, int y, BlockID block);
		void SetMetadata(int x, int y, Metadata data);
		void SetLight(int x, int y, Light light);
		void SetBackground(int x, int y, BackgroundID background);

		bool LightQueueEmpty() const;
		std::array<int, 2> LightQueuePop();
		void LightQueuePush(int x, int y);

		bool RemoveLightQueueEmpty() const;
		std::array<int, 2> RemoveLightQueuePop();
		void RemoveLightQueuePush(int x, int y);

		int GetX() const;
		int GetY() const;

		bool GetEdgeChunk() const;

		static int GetWidth();
		static int GetHeight();

		unsigned int GetTerrainImage() const;
		unsigned int GetShadowImage() const;

		void SetEdgeChunk(bool onEdge);

		unsigned int Encode();
		void Decode(unsigned int memblock);
	private:
		static const int WIDTH = 32;	//In blocks
		static const int HEIGHT = 32;

		bool edgeChunk;

		std::vector<std::vector<BlockID>> blockID;
		std::vector<std::vector<Metadata>> metadata;
		std::vector<std::vector<Light>> light;
		std::vector<std::vector<BackgroundID>> backgroundID;

		std::queue<std::array<int,2>> lightQueue;
		std::queue<std::array<int,2>> removeLightQueue;

		unsigned int terrainImage;
		unsigned int shadowImage;

		int chunkX;	//Chunk coordinates in world
		int chunkY;
		
		void GenerateImage();
		void GenerateShadow();
};

#endif