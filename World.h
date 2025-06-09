#ifndef _H_WORLD
#define _H_WORLD

#include <vector>
#include <queue>

#include "Chunk.h"
#include "FastNoiseLite.h"

class World {
	public:
		World();
		~World();
		void Update();
		Chunk* GetChunk(int x, int y) const;
		void SetOriginChunk(int x, int y);
		void SetBlock(int x, int y, BlockID block);
		
		BlockID GetBlock(int x, int y) const;
		BlockID GetBackground(int x, int y) const;
		Light GetLight(int x, int y) const;

		void SetLight(int x, int y, Light light);
		
		unsigned int GetTerrainImage() const { return terrainImage; }
		unsigned int GetShadowImage() const { return shadowImage; }

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
		std::queue<Chunk*> renderQueue;

		FastNoiseLite noiseGenerator;

		int WIDTH = 7;	//In chunks
		int HEIGHT = 7;

		int originX; //Controls which chunk is our center chunk
		int originY;

		unsigned int terrainImage;
		unsigned int shadowImage;
		unsigned int worldSprite;
		unsigned int worldShader;

		void ProcessBuildQueue();
		void ProcessRenderQueue();
		void ProcessChunkTicks();

		void GenerateTerrain(Chunk* chunk);
		void FillLightQueue(Chunk* chunk);
		void GenerateLight(Chunk* chunk);
		void RemoveLight(Chunk* chunk);

		template<typename T>
		void RemoveChunkFromQueue(std::queue<T*>& q, T* ptr);
		void SaveChunk(Chunk* chunk);

		void RenderChunksToImage(RenderImageType type);	
};


#endif