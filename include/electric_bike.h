#ifndef GUARD_ELECTRIC_BIKE_H
#define GUARD_ELECTRIC_BIKE_H

#include "global.h"

bool8 ElectricBike_IsActive(void);
void ElectricBike_SetActive(bool8 active);
void ElectricBike_OnStep(void);

#endif // GUARD_ELECTRIC_BIKE_H
