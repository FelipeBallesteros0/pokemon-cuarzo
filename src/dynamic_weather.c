#include "global.h"
#include "dynamic_weather.h"
#include "event_data.h"
#include "field_weather.h"
#include "overworld.h"
#include "random.h"
#include "rtc.h"
#include "constants/vars.h"
#include "constants/weather.h"
#include "config/overworld.h"

#define DYNAMIC_WEATHER_OVERRIDE_AUTO 0xFFFF

struct WeatherWeight
{
    u8 weather;
    u8 weight;
};

static bool32 IsDynamicWeatherActive(void)
{
    return OW_DYNAMIC_WEATHER;
}

static bool32 IsOutdoorMapForDynamicWeather(void)
{
    return (IsMapTypeOutdoors(gMapHeader.mapType) && gMapHeader.mapType != MAP_TYPE_UNDERWATER);
}

static void EnsureDynamicWeatherVarsInit(void)
{
    if (VarGet(VAR_DYNAMIC_WEATHER_OVERRIDE) == 0
     && VarGet(VAR_DYNAMIC_WEATHER_TIMER) == 0
     && VarGet(VAR_DYNAMIC_WEATHER_MAPSEC) == 0
     && VarGet(VAR_DYNAMIC_WEATHER) == 0)
        VarSet(VAR_DYNAMIC_WEATHER_OVERRIDE, DYNAMIC_WEATHER_OVERRIDE_AUTO);
}

static u16 GetCurrentMapSec(void)
{
    return gMapHeader.regionMapSectionId;
}

static u16 RandomWeatherDurationMinutes(void)
{
    u16 min = OW_DYNAMIC_WEATHER_MINUTES_MIN;
    u16 max = OW_DYNAMIC_WEATHER_MINUTES_MAX;
    if (max < min)
        max = min;
    return min + (Random() % (max - min + 1));
}

static u8 PickWeatherForCurrentMap(void)
{
    // Simple profiles: ocean routes favor rain, others favor clear/cloudy
    static const struct WeatherWeight sWeightsLand[] =
    {
        {WEATHER_NONE,              30},
        {WEATHER_SUNNY_CLOUDS,      25},
        {WEATHER_SUNNY,             20},
        {WEATHER_RAIN,              15},
        {WEATHER_RAIN_THUNDERSTORM, 5},
        {WEATHER_FOG_HORIZONTAL,    5},
    };
    static const struct WeatherWeight sWeightsOcean[] =
    {
        {WEATHER_NONE,              20},
        {WEATHER_SUNNY_CLOUDS,      20},
        {WEATHER_SUNNY,             5},
        {WEATHER_RAIN,              30},
        {WEATHER_RAIN_THUNDERSTORM, 15},
        {WEATHER_FOG_HORIZONTAL,    10},
    };

    const struct WeatherWeight *weights = sWeightsLand;
    u32 count = ARRAY_COUNT(sWeightsLand);
    if (gMapHeader.mapType == MAP_TYPE_OCEAN_ROUTE)
    {
        weights = sWeightsOcean;
        count = ARRAY_COUNT(sWeightsOcean);
    }

    u32 total = 0;
    for (u32 i = 0; i < count; i++)
        total += weights[i].weight;

    if (total == 0)
        return WEATHER_NONE;

    u32 roll = Random() % total;
    for (u32 i = 0; i < count; i++)
    {
        if (roll < weights[i].weight)
            return weights[i].weather;
        roll -= weights[i].weight;
    }

    return WEATHER_NONE;
}

static void RollNewDynamicWeather(void)
{
    VarSet(VAR_DYNAMIC_WEATHER, PickWeatherForCurrentMap());
    VarSet(VAR_DYNAMIC_WEATHER_TIMER, RandomWeatherDurationMinutes());
    VarSet(VAR_DYNAMIC_WEATHER_MAPSEC, GetCurrentMapSec());
}

static void ApplyWeatherFromVars(bool32 immediate)
{
    u16 weather = VarGet(VAR_DYNAMIC_WEATHER);
    if (weather >= WEATHER_COUNT)
        weather = WEATHER_NONE;
    SetWeather(weather);
    if (immediate)
        DoCurrentWeather();
}

void DynamicWeather_InitForCurrentMap(void)
{
    if (!IsDynamicWeatherActive())
        return;

    EnsureDynamicWeatherVarsInit();

    u16 override = VarGet(VAR_DYNAMIC_WEATHER_OVERRIDE);
    if (override != DYNAMIC_WEATHER_OVERRIDE_AUTO)
    {
        // Only apply forced weather outdoors
        if (IsOutdoorMapForDynamicWeather())
        {
            VarSet(VAR_DYNAMIC_WEATHER, override);
            ApplyWeatherFromVars(FALSE);
        }
        return;
    }

    if (!IsOutdoorMapForDynamicWeather())
        return;

    u16 mapsec = GetCurrentMapSec();
    u16 curMapsec = VarGet(VAR_DYNAMIC_WEATHER_MAPSEC);
    u16 timer = VarGet(VAR_DYNAMIC_WEATHER_TIMER);
    u16 weather = VarGet(VAR_DYNAMIC_WEATHER);

    if (timer == 0 || curMapsec != mapsec || weather >= WEATHER_COUNT)
        RollNewDynamicWeather();

    ApplyWeatherFromVars(FALSE);
}

void DynamicWeather_UpdatePerMinute(u16 minutes)
{
    if (!IsDynamicWeatherActive())
        return;

    EnsureDynamicWeatherVarsInit();

    if (VarGet(VAR_DYNAMIC_WEATHER_OVERRIDE) != DYNAMIC_WEATHER_OVERRIDE_AUTO)
        return;

    if (minutes == 0)
        return;

    s32 timer = VarGet(VAR_DYNAMIC_WEATHER_TIMER);
    timer -= minutes;
    if (timer <= 0)
    {
        RollNewDynamicWeather();
        timer = VarGet(VAR_DYNAMIC_WEATHER_TIMER);
        if (IsOutdoorMapForDynamicWeather())
            ApplyWeatherFromVars(TRUE);
    }
    else
    {
        VarSet(VAR_DYNAMIC_WEATHER_TIMER, timer);
    }
}

void DynamicWeather_ApplyOverride(void)
{
    if (!IsDynamicWeatherActive())
        return;

    EnsureDynamicWeatherVarsInit();
    u16 override = VarGet(VAR_DYNAMIC_WEATHER_OVERRIDE);
    if (override == DYNAMIC_WEATHER_OVERRIDE_AUTO)
    {
        VarSet(VAR_DYNAMIC_WEATHER_TIMER, 0);
        DynamicWeather_InitForCurrentMap();
        if (IsOutdoorMapForDynamicWeather())
            DoCurrentWeather();
    }
    else
    {
        if (!IsOutdoorMapForDynamicWeather())
            return;
        VarSet(VAR_DYNAMIC_WEATHER, override);
        VarSet(VAR_DYNAMIC_WEATHER_TIMER, 1);
        VarSet(VAR_DYNAMIC_WEATHER_MAPSEC, GetCurrentMapSec());
        ApplyWeatherFromVars(TRUE);
    }
}
