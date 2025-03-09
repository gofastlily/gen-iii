#include "global.h"
#include "event_data.h"
#include "seasons.h"
#include "constants/vars.h"

void AdvanceSeason(void) {
    CurrentSeasonSet(NextSeasonGet(CurrentSeasonGet()));
}

u8 CurrentSeasonGet(void) {
    return VarGet(VAR_CURRENT_SEASON);
}

void CurrentSeasonSet(u8 new_season) {
    VarSet(VAR_CURRENT_SEASON, new_season);
}

u8 DaysInSeasonGet(void) {
    return VarGet(VAR_SEASON_DAYS);
}

void DaysInSeasonIncrement() {
    u8 next_days_in_season = DaysInSeasonGet() + 1;
    if (next_days_in_season < SEASON_LENGTH_DAYS)
        DaysInSeasonSet(next_days_in_season);
    else
        DaysInSeasonSet(0);
        AdvanceSeason();
}

void DaysInSeasonSet(u8 new_days) {
    VarSet(VAR_SEASON_DAYS, new_days);
}

u8 NextSeasonGet(u8 current_season) {
    current_season++;
    if (current_season >= SEASONS_COUNT)
        return SEASON_SPRING;
    return current_season;
}
