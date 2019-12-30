
#include <algorithm>  // std::max
#include <math.h>     // pow
#include <cmath>      // fmod

#include "Constants.hpp"


class DayNightCycle
{
public:
    // light center + radius + period
    constexpr static const float lightX = 0.0;
    constexpr static const float lightY = 0.0;
    constexpr static const float lightZ = 0.0;
    constexpr static const float lightRadius = 500.0;
    constexpr static const float lightPeriod = 25.0;
    
    constexpr static const float transitionSmoothness = 0.25;
    constexpr static const float redSmoothness = 0.7;
    constexpr static const float moonIntensity = 0.3;
    
    static glm::vec3 getSunPos(float timeValue) {
        float angle = timeValue*2*M_PI/lightPeriod;
        float posX = lightX;
        float posY = lightY + lightRadius*sin(angle);
        float posZ = lightZ + lightRadius*cos(angle);
        return glm::vec3(posX, posY, posZ);
    }
    static glm::vec3 getSunColor(float timeValue) {
        float c1 = 0.5;
        float c2 = 1.0;
        float angle = fmod(timeValue*2*M_PI/lightPeriod, 2*M_PI);
        float intensity = pow(sin(angle), transitionSmoothness);
        intensity = std::max(intensity, 0.0f);
        float redValue = 1 - pow(sin(angle), redSmoothness);
        return intensity*glm::vec3(1.0, 1.0-redValue*c1, 1.0-redValue*c2);
    }
    
    static glm::vec3 getMoonPos(float timeValue) {
        float angle = timeValue*2*M_PI/lightPeriod + M_PI;
        float posX = lightX;
        float posY = lightY + lightRadius*sin(angle);
        float posZ = lightZ + lightRadius*cos(angle);
        return glm::vec3(posX, posY, posZ);
    }
    static glm::vec3 getMoonColor(float timeValue) {
        float angle = fmod(timeValue*2*M_PI/lightPeriod + M_PI, 2*M_PI);
        float intensity = moonIntensity*pow(sin(angle), transitionSmoothness);
        intensity = std::max(intensity, 0.0f);
        return intensity*glm::vec3(1.0, 1.0, 1.0);
    }
    
};
