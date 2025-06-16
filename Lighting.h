#pragma once
#include <vector>

struct ColorData {
    float Red;
    float Green;
    float Blue;
    float Time;
};

class Lighting {
    public:
        Lighting();

        void AddColor(float red, float geen, float blue, float time);
        void Update(float frameTime);
        void SetSkyColor(float red, float geen, float blue);

    private:
        std::vector<ColorData> colorCycle;
        float cycleTime;
        float prevTime;
        int prevCycleID;
        int nextCycleID;
};
