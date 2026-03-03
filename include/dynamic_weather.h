#ifndef GUARD_DYNAMIC_WEATHER_H
#define GUARD_DYNAMIC_WEATHER_H

#include "global.h"

void DynamicWeather_InitForCurrentMap(void);
void DynamicWeather_UpdatePerMinute(u16 minutes);
void DynamicWeather_ApplyOverride(void);

#endif // GUARD_DYNAMIC_WEATHER_H
