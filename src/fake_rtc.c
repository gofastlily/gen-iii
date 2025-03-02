#include "global.h"
#include "string_util.h"
#include "strings.h"
#include "text.h"
#include "rtc.h"
#include "fake_rtc.h"
#include "event_data.h"
#include "script.h"

struct Time *FakeRtc_GetCurrentTime(void)
{
#if OW_USE_FAKE_RTC
    return &gSaveBlock3Ptr->fakeRTC;
#else
    return NULL;
#endif
}

void FakeRtc_GetRawInfo(struct SiiRtcInfo *rtc)
{
    struct Time* time = FakeRtc_GetCurrentTime();
    rtc->second = time->seconds;
    rtc->minute = time->minutes;
    rtc->hour = time->hours;
    rtc->day = time->days;
}

u16 FakeRtc_GetPreviousTimeRatio(void)
{
    return VarGet(VAR_PREVIOUS_TIME_RATIO);
}

void FakeRtc_SetPreviousTimeRatio(u16 time_ratio)
{
    VarSet(VAR_PREVIOUS_TIME_RATIO, time_ratio);
}

void FakeRtc_ReturnToPreviousTimeRatio(void)
{
    VarSet(VAR_ALTERED_TIME_RATIO, FakeRtc_GetPreviousTimeRatio());
}

u16 FakeRtc_GetAlteredTimeRatio(void)
{
    return VarGet(VAR_ALTERED_TIME_RATIO);
}

void FakeRtc_SetAlteredTimeRatio(u16 time_ratio)
{
    if (VarGet(VAR_ALTERED_TIME_RATIO) == time_ratio)
        return;

    FakeRtc_SetPreviousTimeRatio(FakeRtc_GetAlteredTimeRatio());
    VarSet(VAR_ALTERED_TIME_RATIO, time_ratio);
}

void FakeRtc_SetAlteredTimeRatio_Standard(void)
{
    FakeRtc_SetAlteredTimeRatio(GEN_III_STANDARD);
}

void FakeRtc_SetAlteredTimeRatio_Slow(void)
{
    FakeRtc_SetAlteredTimeRatio(GEN_III_SLOW);
}

void FakeRtc_SetAlteredTimeRatio_Realtime(void)
{
    FakeRtc_SetAlteredTimeRatio(GEN_III_REALTIME);
}

void FakeRtc_Pause(void)
{
    FlagSet(FLAG_PAUSE_TIME);
}

void FakeRtc_Resume(void)
{
    FlagClear(FLAG_PAUSE_TIME);
}

void FakeRtc_Toggle(void)
{
    FlagToggle(FLAG_PAUSE_TIME);
}

bool8 FakeRtc_IsPaused(void)
{
    return FlagGet(FLAG_PAUSE_TIME);
}

void FakeRtc_Init(s32 hour, s32 minute)
{
    RtcInitLocalTimeOffset(hour, minute);
    
    FakeRtc_SetAlteredTimeRatio_Standard();
    FakeRtc_Pause();
}

void FakeRtc_TickTimeForward(void)
{
    if (!OW_USE_FAKE_RTC)
        return;

    if (FakeRtc_IsPaused())
        return;

    FakeRtc_AdvanceTimeBy(0, 0, FakeRtc_GetSecondsRatio());
}

void FakeRtc_AdvanceTimeBy(u32 hours, u32 minutes, u32 seconds)
{
    struct Time* time = FakeRtc_GetCurrentTime();
    seconds += time->seconds;
    minutes += time->minutes;
    hours += time->hours;

    while(seconds >= SECONDS_PER_MINUTE)
    {
        minutes++;
        seconds -= SECONDS_PER_MINUTE;
    }

    while(minutes >= MINUTES_PER_HOUR)
    {
        hours++;
        minutes -= MINUTES_PER_HOUR;
    }

    while(hours >= HOURS_PER_DAY)
    {
        time->days++;
        hours -= HOURS_PER_DAY;
    }

    time->seconds = seconds;
    time->minutes = minutes;
    time->hours = hours;
}

void FakeRtc_ManuallySetTime(u32 hour, u32 minute, u32 second)
{
    struct Time diff, target;
    RtcCalcLocalTime();

    target.hours = hour;
    target.minutes = minute;
    target.seconds = second;
    target.days = gLocalTime.days;

    CalcTimeDifference(&diff, &gLocalTime, &target);
    FakeRtc_AdvanceTimeBy(diff.hours, diff.minutes, diff.seconds);
}

u32 FakeRtc_GetSecondsRatio(void)
{
    switch (FakeRtc_GetAlteredTimeRatio())
    {
        case GEN_8_PLA:
            return 60;
        case GEN_9:
            return 20;
        case GEN_III_STANDARD:
            return 80;
        case GEN_III_SLOW:
            return 24;
    }
    return 1;
}

STATIC_ASSERT((OW_FLAG_PAUSE_TIME == 0 || OW_USE_FAKE_RTC == TRUE), FakeRtcMustBeTrueToPauseTime);

void Script_PauseFakeRtc(void)
{
    Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);

    FakeRtc_Pause();
}

void Script_ResumeFakeRtc(void)
{
    Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);

    FakeRtc_Resume();
}

void Script_ToggleFakeRtc(void)
{
    Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);
    
    FakeRtc_Toggle();
}

void Script_SetAlteredTimeRatioStandard(void)
{
    Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);
    
    FakeRtc_SetAlteredTimeRatio_Standard();
}

void Script_SetAlteredTimeRatioSlow(void)
{
    Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);
    
    FakeRtc_SetAlteredTimeRatio_Slow();
}

void Script_SetAlteredTimeRatioRealtime(void)
{
    Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);
    
    FakeRtc_SetAlteredTimeRatio_Realtime();
}
