#include "global.h"
#include "string_util.h"
#include "strings.h"
#include "text.h"
#include "datetime.h"
#include "rtc.h"
#include "fake_rtc.h"
#include "event_data.h"
#include "seasons.h"
#include "script.h"
#include "wallclock.h"

void FakeRtc_Reset(void)
{
#if OW_USE_FAKE_RTC
    memset(&gSaveBlock3Ptr->fakeRTC, 0, sizeof(gSaveBlock3Ptr->fakeRTC));
    gSaveBlock3Ptr->fakeRTC.year = 0; // offset by gGen3Epoch.year
    gSaveBlock3Ptr->fakeRTC.month = gGen3Epoch.month;
    gSaveBlock3Ptr->fakeRTC.day = gGen3Epoch.day;
    gSaveBlock3Ptr->fakeRTC.dayOfWeek = gGen3Epoch.dayOfWeek;
#endif
}

struct SiiRtcInfo *FakeRtc_GetCurrentTime(void)
{
#if OW_USE_FAKE_RTC
    return &gSaveBlock3Ptr->fakeRTC;
#else
    return NULL;
#endif
}

void FakeRtc_GetRawInfo(struct SiiRtcInfo *rtc)
{
    struct SiiRtcInfo *fakeRtc = FakeRtc_GetCurrentTime();
    if (fakeRtc != NULL)
        memcpy(rtc, fakeRtc, sizeof(struct SiiRtcInfo));
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
    u16 current_ratio = FakeRtc_GetAlteredTimeRatio();
    VarSet(VAR_ALTERED_TIME_RATIO, FakeRtc_GetPreviousTimeRatio());
    VarSet(VAR_PREVIOUS_TIME_RATIO, current_ratio);
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

void FakeRtc_SetAlteredTimeRatio_Previous(void)
{
    FakeRtc_ReturnToPreviousTimeRatio();
}

void FakeRtc_ForcePause(void)
{
    FlagSet(FLAG_PAUSE_TIME);
}

void FakeRtc_Pause(void)
{
    if (IsWallClockSet())
        FlagSet(FLAG_PAUSE_TIME);
}

void FakeRtc_Resume(void)
{
    if (IsWallClockSet())
        FlagClear(FLAG_PAUSE_TIME);
}

void FakeRtc_Toggle(void)
{
    if (IsWallClockSet())
        FlagToggle(FLAG_PAUSE_TIME);
}

bool8 FakeRtc_IsPaused(void)
{
    return FlagGet(FLAG_PAUSE_TIME);
}

void FakeRtc_Init(s32 hour, s32 minute)
{
    RtcInitLocalTimeOffset(0, 0);
    FakeRtc_ManuallySetTime(0, hour, minute, 0);
    FakeRtc_SetAlteredTimeRatio_Standard();
    FakeRtc_ForcePause();
}

void FakeRtc_TickTimeForward(void)
{
    if (!OW_USE_FAKE_RTC)
        return;

    if (FakeRtc_IsPaused())
        return;

    FakeRtc_AdvanceTimeBy(0, 0, 0, FakeRtc_GetSecondsRatio());
}

void FakeRtc_AdvanceTimeBy(u32 days, u32 hours, u32 minutes, u32 seconds)
{
    struct DateTime dateTime;
    struct SiiRtcInfo *rtc = FakeRtc_GetCurrentTime();

    ConvertRtcToDateTime(&dateTime, rtc);
    DateTime_AddSeconds(&dateTime, seconds);
    DateTime_AddMinutes(&dateTime, minutes);
    DateTime_AddHours(&dateTime, hours);
    DateTime_AddDays(&dateTime, days);
    ConvertDateTimeToRtc(rtc, &dateTime);
}

void FakeRtc_ManuallySetTime(u32 day, u32 hour, u32 minute, u32 second)
{
    FakeRtc_Reset();
    FakeRtc_AdvanceTimeBy(day, hour, minute, second);
}

void AdvanceScript(void)
{
    FakeRtc_AdvanceTimeBy(300, 0, 0, 0);
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

void Script_SetAlteredTimeRatioPrevious(void)
{
    Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);
    
    FakeRtc_SetAlteredTimeRatio_Previous();
}
