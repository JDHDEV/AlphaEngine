#pragma once
//========================================================================
// DirectX11Timer.h : Shamelessly stolen from DXUT i hope they forgive me
// but if it aint broke :)
//
// Part of the Alpha Application
//
// Justin Hunt
//========================================================================

//--------------------------------------------------------------------------------------
// Performs timer operations
// Use DXUTGetGlobalTimer() to get the global instance
//--------------------------------------------------------------------------------------
class AlphaTimer
{
public:
    AlphaTimer();

    void            Reset(); // resets the timer
    void            Start(); // starts the timer
    void            Stop();  // stop (or pause) the timer
    void            Advance(); // advance the timer by 0.1 seconds
    double          GetAbsoluteTime() const; // get the absolute system time
    double          GetTime() const; // get the current time
    float           GetElapsedTime(); // get the time that elapsed between Get*ElapsedTime() calls
    void            GetTimeValues(double* pfTime, double* pfAbsoluteTime, float* pfElapsedTime); // get all time values at once
    bool            IsStopped() const { return m_bTimerStopped; } // returns true if timer stopped

    // Limit the current thread to one processor (the current one). This ensures that timing code runs
    // on only one processor, and will not suffer any ill effects from power management.
    void            LimitThreadAffinityToCurrentProc();

protected:
    LARGE_INTEGER   GetAdjustedCurrentTime() const;

    bool m_bUsingQPF;
    bool m_bTimerStopped;
    LONGLONG m_llQPFTicksPerSec;

    LONGLONG m_llStopTime;
    LONGLONG m_llLastElapsedTime;
    LONGLONG m_llBaseTime;
};

AlphaTimer* WINAPI GetGlobalAlphaTimer();