#include "global.h"
#include "event_data.h"
#include "event_scripts.h"
#include "field_weather.h"
#include "overworld.h"
#include "random.h"
#include "script.h"
#include "follower_weather.h"
#include "constants/vars.h"
#include "constants/weather.h"
#include "constants/pokemon.h"
#include "data/text/follower_weather_messages.h"

#define FOLLOWER_WEATHER_COOLDOWN_STEPS 120

struct WeatherTextList
{
    u8 weather;
    const u8 *const *texts;
    u8 count;
};

static const struct WeatherTextList sGenericWeatherTexts[] =
{
    // Despejado
    {WEATHER_NONE,              sFollowerWeatherClear,  ARRAY_COUNT(sFollowerWeatherClear)},
    {WEATHER_SUNNY_CLOUDS,      sFollowerWeatherClear,  ARRAY_COUNT(sFollowerWeatherClear)},
    // Soleado
    {WEATHER_DROUGHT,           sFollowerWeatherSunny,  ARRAY_COUNT(sFollowerWeatherSunny)},
    // Nublado
    {WEATHER_SHADE,             sFollowerWeatherCloudy, ARRAY_COUNT(sFollowerWeatherCloudy)},
    // Lluvia / Tormenta
    {WEATHER_RAIN,              sFollowerWeatherRain,   ARRAY_COUNT(sFollowerWeatherRain)},
    {WEATHER_RAIN_THUNDERSTORM, sFollowerWeatherStorm,  ARRAY_COUNT(sFollowerWeatherStorm)},
    {WEATHER_DOWNPOUR,          sFollowerWeatherStorm,  ARRAY_COUNT(sFollowerWeatherStorm)},
    // Arena
    {WEATHER_SANDSTORM,         sFollowerWeatherSand,   ARRAY_COUNT(sFollowerWeatherSand)},
    // Niebla
    {WEATHER_FOG_HORIZONTAL,    sFollowerWeatherFog,    ARRAY_COUNT(sFollowerWeatherFog)},
    {WEATHER_FOG_DIAGONAL,      sFollowerWeatherFog,    ARRAY_COUNT(sFollowerWeatherFog)},
    // Nieve / Granizo (sin weather de granizo, se reutiliza WEATHER_SNOW)
    {WEATHER_SNOW,              sFollowerWeatherSnow,   ARRAY_COUNT(sFollowerWeatherSnow)},
    {WEATHER_SNOW,              sFollowerWeatherHail,   ARRAY_COUNT(sFollowerWeatherHail)},
    // Ceniza
    {WEATHER_VOLCANIC_ASH,      sFollowerWeatherAsh,    ARRAY_COUNT(sFollowerWeatherAsh)},
};

static const u8 *GetRandomText(const u8 *const *texts, u8 count)
{
    if (count == 0 || texts == NULL)
        return NULL;
    return texts[Random() % count];
}

static const u8 *GetFollowerWeatherReactionText(u8 weather)
{
    for (u32 i = 0; i < ARRAY_COUNT(sGenericWeatherTexts); i++)
    {
        if (sGenericWeatherTexts[i].weather == weather)
            return GetRandomText(sGenericWeatherTexts[i].texts, sGenericWeatherTexts[i].count);
    }
    return NULL;
}

bool8 TryShowFollowerWeatherReaction(struct ScriptContext *ctx)
{
    if (!IsMapTypeOutdoors(gMapHeader.mapType))
        return FALSE;

    u8 weather = GetCurrentWeather();
    const u8 *text = GetFollowerWeatherReactionText(weather);
    if (text == NULL)
        return FALSE;
    ctx->data[0] = (u32)text;
    ScriptCall(ctx, EventScript_FollowerGeneric);
    return TRUE;
}
