#include "Block.h"
#include "agk.h"

std::vector<unsigned int> Block::blockImages;
unsigned int Block::backgroundImage;

const Block::BlockComponents Block::BlockDefinitions[11] = {
/*   NAME				  DrawMode			Collision       L  Light Mode*/
	{(char*)"Air"		, DRAW_NONE		  , COLLIDE_NONE ,  0, LIGHT_TRANSPARENT},
	{(char*)"Grass"		, DRAW_OPAQUE	  , COLLIDE_SOLID,  0, LIGHT_OPAQUE},
	{(char*)"Dirt"		, DRAW_OPAQUE	  , COLLIDE_SOLID,  0, LIGHT_OPAQUE},
	{(char*)"Stone"		, DRAW_OPAQUE	  , COLLIDE_SOLID,  0, LIGHT_OPAQUE},
	{(char*)"Oak Log"	, DRAW_OPAQUE	  , COLLIDE_SOLID,  0, LIGHT_OPAQUE},
	{(char*)"Oak Leaves", DRAW_TRANSPARENT, COLLIDE_NONE ,  0, LIGHT_TRANSPARENT},
	{(char*)"Oak Planks", DRAW_OPAQUE	  , COLLIDE_SOLID,  0, LIGHT_OPAQUE},
	{(char*)"Sand"		, DRAW_OPAQUE	  , COLLIDE_SOLID,  0, LIGHT_OPAQUE},
	{(char*)"Bricks"	, DRAW_OPAQUE	  , COLLIDE_SOLID,  0, LIGHT_OPAQUE},
	{(char*)"Daisy"		, DRAW_TRANSPARENT, COLLIDE_NONE ,  0, LIGHT_TRANSPARENT},
	{(char*)"Torch"		, DRAW_TRANSPARENT, COLLIDE_NONE , 31, LIGHT_TRANSPARENT}
};

float Block::GetSize()
{
	return SIZE;
}

void Block::LoadImages()
{
	blockImages.push_back(agk::LoadImage("air.png"));
	blockImages.push_back(agk::LoadImage("grass_side.png"));
	blockImages.push_back(agk::LoadImage("dirt.png"));
	blockImages.push_back(agk::LoadImage("stone.png"));
	blockImages.push_back(agk::LoadImage("oak_log.png"));
	blockImages.push_back(agk::LoadImage("oak_leaves.png"));
	blockImages.push_back(agk::LoadImage("oak_planks.png"));
	blockImages.push_back(agk::LoadImage("sand.png"));
	blockImages.push_back(agk::LoadImage("bricks.png"));
	blockImages.push_back(agk::LoadImage("oxeye_daisy.png"));
	blockImages.push_back(agk::LoadImage("torch.png"));

	backgroundImage = agk::LoadImage("background.png");
}

void Block::UnloadImages()
{
	for (int image = 0; image < blockImages.size(); image++)
	{
		agk::DeleteImage(blockImages[image]);
	}

	blockImages.clear();

	agk::DeleteImage(backgroundImage);
}

unsigned int Block::GetImage(const BlockID block)
{
	return blockImages[block];
}

unsigned int Block::GetBackgroundImage()
{
	return backgroundImage;
}

char* Block::GetName(const BlockID block)
{
	return BlockDefinitions[block].NAME; 
}

DrawMode Block::GetDrawMode(const BlockID block)
{
	return BlockDefinitions[block].DRAWMODE;
}
Collision Block::GetCollision(const BlockID block)
{
	return BlockDefinitions[block].COLLISION;
}
Light Block::GetLight(const BlockID block)
{
	return BlockDefinitions[block].LIGHT;
}

LightMode Block::GetLightMode(const BlockID block)
{
	return BlockDefinitions[block].LIGHTMODE;
}