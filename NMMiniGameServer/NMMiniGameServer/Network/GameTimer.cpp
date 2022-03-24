#include "GameTimer.h"
#include <cmath>
#include <WS2tcpip.h>
using namespace Network;


Network::GameTimer::GameTimer( )
{
    this->Reset( );
}

GameTimer::~GameTimer( )
{
}

void GameTimer::Tick( float fLockFPS )
{
    float fTimeElapsed;
    ::QueryPerformanceCounter( (LARGE_INTEGER*)&currentPerformanceCounter );
    fTimeElapsed = float( ( currentPerformanceCounter - lastPerformanceCounter ) * timeScale );
    currentGameTime = float( ( currentPerformanceCounter - basePerformanceCounter ) * timeScale );
    int cnt = 0;
    if( fLockFPS > 0.0f )
    {
        while( fTimeElapsed < ( 1.0f / fLockFPS ) )
        {
            ::QueryPerformanceCounter( (LARGE_INTEGER*)&currentPerformanceCounter );
            fTimeElapsed = float( ( currentPerformanceCounter - lastPerformanceCounter ) * timeScale );
            cnt += 1;
        }
    }

    lastPerformanceCounter = currentPerformanceCounter;



    if( fabsf( fTimeElapsed - timeElapsed ) < 1.0f )
    {
        ::memmove( &frameTime[1], frameTime, ( MAX_SAMPLE_COUNT - 1 ) *
            sizeof( float ) );
        frameTime[0] = fTimeElapsed;
        if( sampleCount < MAX_SAMPLE_COUNT ) sampleCount++;
    }
    this->framePerSecond++;
    fpsTimeElapsed += fTimeElapsed;

    if( fpsTimeElapsed > 1.0f )
    {
        currentFrameRate = framePerSecond;
        framePerSecond = 0;
        fpsTimeElapsed = 0.0f;
    }

    timeElapsed = 0.0f;

    for( ULONG i = 0; i < sampleCount; i++ )
    {
        timeElapsed += frameTime[i];
    }
    if( sampleCount > 0 )
    {
        timeElapsed /= sampleCount;
    }
}

unsigned long GameTimer::GetFrameRate( )
{
    return( currentFrameRate );
}

float GameTimer::GetTimeElapsed( )
{
    return this->timeElapsed;
}

double GameTimer::GetGameTime( ) const
{
    return this->currentGameTime;
}

void GameTimer::Reset( )
{
    ::QueryPerformanceFrequency( (LARGE_INTEGER*)&performanceFrequencyPerSec );
    ::QueryPerformanceCounter( (LARGE_INTEGER*)&lastPerformanceCounter );
    timeScale = 1.0 / (double)performanceFrequencyPerSec;

    basePerformanceCounter = lastPerformanceCounter;
    pausedPerformanceCounter = 0;
    stopPerformanceCounter = 0;

    sampleCount = 0;
    currentFrameRate = 0;
    framePerSecond = 0;
    fpsTimeElapsed = 0.0f;
}
