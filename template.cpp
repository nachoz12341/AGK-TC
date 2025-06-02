// Includes
#include "template.h"
#include "Player.h"
#include "World.h"

#include <cmath>

// Namespace
using namespace AGK;

app App;

Player* player;
World* world;

void app::Begin(void)
{
	agk::SetWindowSize(1280, 720, false);
	agk::SetVirtualResolution(1280, 720);
	agk::SetViewZoomMode(1);
	agk::SetViewZoom(2.0f);
	agk::SetClearColor( 151,170,204 ); // light blue
	agk::SetSyncRate(60,1);
	agk::SetScissor(0,0,0,0);

	agk::SetFolder("/media");

	//Turn off texture filter
	agk::SetDefaultMagFilter(0);
	agk::SetDefaultMinFilter(0);
	agk::SetGenerateMipmaps(false);

	world = new World();
	player = new Player(world, 1280.0f, 1280.0f);
}

int app::Loop (void)
{
	agk::Print( agk::ScreenFPS() );
	player->Update();
	world->SetOriginChunk(World::WorldCoordToChunkX(World::PixelToWorldCoordX(player->GetX())), World::WorldCoordToChunkY(World::PixelToWorldCoordY(player->GetY())));
	world->Update();

	agk::Sync();
	return 0; // return 1 to close app
}


void app::End (void)
{
	delete player;
	delete world;
}
