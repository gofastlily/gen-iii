#ifndef GUARD_SEASONS_H
#define GUARD_SEASONS_H

#define SEASON_SPRING 0
#define SEASON_SUMMER 1
#define SEASON_AUTUMN 2
#define SEASON_WINTER 3
#define SEASONS_COUNT 4

#define SEASON_LENGTH_DAYS 10

void AdvanceSeason(void);
u8 CurrentSeasonGet(void);
void CurrentSeasonSet(u8 new_season);
u8 DaysInSeasonGet(void);
void DaysInSeasonIncrement(void);
void DaysInSeasonSet(u8 new_season);
u8 NextSeasonGet(u8 current_season);

#endif //GUARD_SEASONS_H
