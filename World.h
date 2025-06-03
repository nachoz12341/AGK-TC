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
		Light GetLight(int x, int y) const;
		
		static int PixelToWorldCoordX(float x);
		static int PixelToWorldCoordY(float y);
		static int WorldCoordToChunkX(int x);
		static int WorldCoordToChunkY(int y);

		void Render();

	private:
		enum class RenderImageType
		{
			Terrain,
			Shadow
		};

		std::vector<std::vector<Chunk*>> chunkGrid;
		std::queue<Chunk*> buildQueue;
		std::queue<Chunk*> updateQueue;
		int WIDTH = 5;	//In chunks
		int HEIGHT = 5;

		int originX; //Controls which chunk is our center chunk
		int originY;

		unsigned int terrainImage;
		unsigned int shadowImage;
		unsigned int worldSprite;
		unsigned int worldShader;

		void GenerateTerrain(Chunk* chunk);
		void FillLightQueue(Chunk* chunk);
		void GenerateLight(Chunk* chunk);
		void RemoveLight(Chunk* chunk);

		template<typename T>
		void RemoveChunkFromQueue(std::queue<T*>& q, T* ptr);
		void SaveChunk(Chunk* chunk);

		void RenderImage(RenderImageType type);	
};


#endif