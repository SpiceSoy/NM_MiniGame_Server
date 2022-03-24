// =================================================================================================
//  @file GameTimer.h
// 
//  @brief 상세 설명을 입력하시오
//  
//  @date 2022/03/25
// 
//  Copyright 2022 2022 Netmarble Neo, Inc. All Rights Reserved.
// =================================================================================================

#pragma once


namespace Network
{
    constexpr unsigned long MAX_SAMPLE_COUNT = 50;
    class GameTimer
    {
    public:
        GameTimer( );
        virtual ~GameTimer( );
        void Tick( float fLockFPS = 0.0f );
        unsigned long GetFrameRate( );
        float GetTimeElapsed( );
        double GetGameTime( ) const;
        void Reset( );
    private:
        double currentGameTime;
        double timeScale;
        float timeElapsed;

        __int64 basePerformanceCounter;
        __int64 pausedPerformanceCounter;
        __int64 stopPerformanceCounter;

        union
        {
            __int64 currentPerformanceCounter;
            __int64 currentTime;
        };
        union
        {
            __int64 lastPerformanceCounter;
            __int64 lastTime;
        };

        __int64 performanceFrequencyPerSec;



        float frameTime[MAX_SAMPLE_COUNT];
        unsigned long sampleCount;
        unsigned long currentFrameRate;
        unsigned long framePerSecond;
        float fpsTimeElapsed;
    };

};
