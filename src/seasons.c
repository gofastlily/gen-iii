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

u8 NextSeasonGet(u8 current_season) {
    current_season++;
    if (current_season >= SEASONS_COUNT)
        return SEASON_SPRING;
    return current_season;
}
