#include "Block.h"
#include "agk.h"

std::vector<unsigned int> Block::blockImages;
unsigned int Block::backgroundImage;

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