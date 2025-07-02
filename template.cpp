// Includes
#include "template.h"
#include "Client.h"
#include "Player.h"
#include "World.h"

#include <cmath>

// Namespace
using namespace AGK;

app App;

Player* player;
World* world;
Client* client;

void app::Begin(void)
{
	agk::SetWindowSize(1280, 720, false);
	agk::SetVirtualResolution(1280, 720);
	agk::SetViewZoomMode(1);
	agk::SetViewZoom(2.0f);
	agk::SetClearColor( 151,170,204 ); // light blue
	agk::SetSyncRate(60,0);
	agk::SetScissor(0,0,0,0);
	agk::SetPrintSize(16.0f);
	agk::EnableClearDepth(false);	//Don't need it for 2D

	agk::SetFolder("/media");

	//Turn off texture filter
	agk::SetDefaultMagFilter(0);
	agk::SetDefaultMinFilter(0);
	agk::SetGenerateMipmaps(false);

	world = new World();
	player = new Player(world, 1792.0f, 1792.0f);
	client = new Client("192.168.1.12", 30000);
	client->RegisterObject(player);
}

int app::Loop (void)
{
	agk::Print( agk::ScreenFPS() );

	if (client->IsConnected())
	{
		GameLoop();
	}

	client->Update();

	//Call manually instead of using sync to avoid unnecessary 3d updates
	agk::Update2D();
	agk::Render2DFront();
	agk::Swap();
	return 0; // return 1 to close app
}

int app::GameLoop(void)
{
	player->Update();
	world->SetOriginChunk(World::WorldCoordToChunkX(World::PixelToWorldCoordX(player->GetX())), World::WorldCoordToChunkY(World::PixelToWorldCoordY(player->GetY())));
	world->Update();
	world->Render();

	return 0;
}


void app::End (void)
{
	delete player;
	delete world;
}
