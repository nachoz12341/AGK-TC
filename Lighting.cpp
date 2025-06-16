#include "Util.h"
#include "Lighting.h"
#include "AGK.h"
#include <string>

using namespace AGK;

Lighting::Lighting()
{
    AddColor(30, 30, 60, 30);
    AddColor(83, 148, 252, 90);
    AddColor(141, 185, 224, 180);
    AddColor(192, 220, 190, 270);
    AddColor(30, 30, 30, 330);

    prevTime = colorCycle[0].Time;
    prevCycleID = 0;
    nextCycleID = 1;
    cycleTime = prevTime - 0.1f;
}

void Lighting::AddColor(float red, float green, float blue, float time)
{
    ColorData color = { red, green, blue, time };
    colorCycle.push_back(color);
}

void Lighting::Update(float frameTime)
{
    cycleTime += frameTime * 5.5f;
    // cycleTime += agk::GetRawMouseWheelDelta() / 2.0f

    // Optional sun direction logic (kept from AGK version)
    //float sunX = sin(cycleTime);
    //float sunY = cos(cycleTime);
    //float sunZ = -0.5f;

    const ColorData& prev = colorCycle[prevCycleID];
    const ColorData& next = colorCycle[nextCycleID];
    float deltaTime = colorCycle[nextCycleID].Time - colorCycle[prevCycleID].Time;
    float colorTime = (cycleTime - prevTime) / deltaTime;

    float red = Lerp(colorCycle[prevCycleID].Red, colorCycle[nextCycleID].Red, colorTime);
    float green = Lerp(colorCycle[prevCycleID].Green, colorCycle[nextCycleID].Green, colorTime);
    float blue = Lerp(colorCycle[prevCycleID].Blue, colorCycle[nextCycleID].Blue, colorTime);

    if (cycleTime > colorCycle[nextCycleID].Time)
    {
        prevTime = colorCycle[nextCycleID].Time;
        prevCycleID = nextCycleID;
        nextCycleID++;

        if (nextCycleID >= static_cast<int>(colorCycle.size()))
        {
            nextCycleID = 0;
            cycleTime = prevTime - 360.0f;
        }
    }

    SetSkyColor(red, green, blue);

    // Optional debug
    agk::Print(std::to_string(cycleTime).c_str());
    agk::Print(std::to_string(colorTime).c_str());
    agk::Print(std::to_string(prevCycleID).c_str());
    agk::Print(std::to_string(nextCycleID).c_str());
}


void Lighting::SetSkyColor(float red, float green, float blue)
{
    agk::SetClearColor(red, green, blue);
}
