#ifndef _H_BLOCK
#define _H_BLOCK

#include <vector>

typedef uint8_t BlockID;
typedef uint8_t Metadata;
typedef uint8_t Light;
typedef uint8_t BackgroundID;

typedef enum DrawMode {
	DRAW_NONE,				//Skip drawing
	DRAW_OPAQUE,			//Draw normally
	DRAW_TRANSPARENT		//Draw with background behind it
}DrawMode;

typedef enum Collision {
	COLLIDE_NONE,
	COLLIDE_SOLID,
	COLLIDE_CLIMB,
	COLLIDE_PLATFORM,
	COLLIDE_LIQUID
}Collision;

typedef enum LightMode {
	LIGHT_OPAQUE,			//Blocks light
	LIGHT_TRANSPARENT	    //Does not block light
}LightMode;

typedef enum ID {
	Air,
	Grass,
	Dirt,
	Stone,
	OakLog,
	OakLeaves,
	OakPlanks,
	Sand,
	Bricks,
	Daisy,
	Torch,
	Platform,
	Count
}ID;

class Block {	
	public:
		static float GetSize();
		static void LoadImages();
		static void UnloadImages();
		static unsigned int GetImage(BlockID block);
		static unsigned int GetBackgroundImage();
		static char* GetName(BlockID block);
		static DrawMode GetDrawMode(BlockID block);
		static Collision GetCollision(BlockID block);
		static Light GetLight(BlockID block);
		static LightMode GetLightMode(BlockID block);

	private:
		static constexpr float SIZE = 16.0f;	//In pixels
		static std::vector<unsigned int> blockImages;
		static unsigned int backgroundImage;

		struct BlockComponents {
            char* NAME;
            DrawMode DRAWMODE;
            Collision COLLISION;
            Light LIGHT;
            LightMode LIGHTMODE;
        };

		static const BlockComponents BlockDefinitions[12];
};

#endif