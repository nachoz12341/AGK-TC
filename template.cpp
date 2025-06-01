// Includes
#include "template.h"
#include "Player.h"
#include "World.h"

// Namespace
using namespace AGK;

app App;
Player* player;
World* world;

void app::Begin(void)
{
	agk::SetScreenResolution(1280, 720);
	agk::SetVirtualResolution(1280, 720);
	agk::SetClearColor( 151,170,204 ); // light blue
	agk::SetSyncRate(60,0);
	agk::SetScissor(0,0,0,0);

	agk::SetFolder("/media");

	world = new World();
	player = new Player(1280.0f, 1280.0f);
}

int app::Loop (void)
{
	agk::Print( agk::ScreenFPS() );
	player->Update();
	world->SetOriginChunk(player->GetX() / 16 / 32, player->GetY() / 16 / 32);
	world->Update();
	agk::Sync();
	return 0; // return 1 to close app
}


void app::End (void)
{
	delete player;
}
