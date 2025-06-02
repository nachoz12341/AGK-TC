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
	agk::SetScreenResolution(1280, 720);
	agk::SetVirtualResolution(640, 360);
	agk::SetClearColor( 151,170,204 ); // light blue
	agk::SetSyncRate(60,0);
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
	world->SetOriginChunk((int)std::floor(player->GetX() / 16.0f / 32.0f), (int)std::floor(player->GetY() / 16.0f / 32.0f));
	world->Update();

	agk::Sync();
	return 0; // return 1 to close app
}


void app::End (void)
{
	delete player;
}
