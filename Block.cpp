#include "Block.h"
#include "agk.h"

std::vector<unsigned int> Block::blockImages;

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

	for (int image = 0; image < blockImages.size(); image++)
	{
		
	}
}

void Block::UnloadImages()
{
	for (int image = 0; image < blockImages.size(); image++)
	{
		agk::DeleteImage(blockImages[image]);
	}

	blockImages.clear();
}

unsigned int Block::GetImage(const BlockID block)
{
	return blockImages[block];
}