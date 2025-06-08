#ifndef _H_BLOCK
#define _H_BLOCK

#include <vector>

typedef uint8_t BlockID;
typedef uint8_t Metadata;
typedef uint8_t Light;
typedef uint8_t BackgroundID;


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
	Count
}ID;

class Block {	
	public:
		static float GetSize();
		static void LoadImages();
		static void UnloadImages();
		static unsigned int GetImage(BlockID block);
		static unsigned int GetBackgroundImage();

	private:
		static constexpr float SIZE = 16.0f;	//In pixels
		static std::vector<unsigned int> blockImages;
		static unsigned int backgroundImage;
};

#endif