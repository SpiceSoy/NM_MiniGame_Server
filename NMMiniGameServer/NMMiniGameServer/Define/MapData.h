//=================================================================================================
// @file MapData.h
//
// @brief 클라이언트 맵 크기 정의를 위한 헤더입니다.
// 
// @date 2022/03/15
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "Define/DataTypes.h"
#include <chrono>
#include <string>

using TimeSecond = std::chrono::duration< double >;


namespace Constant
{
    using namespace std::chrono_literals;
    // Server
    extern Int32 TickTerm;
    extern Int32 MaxUserCount;

    extern Double GameFirstWaitSeconds;
    extern Double GameTotalTimeSeconds;

    // Map
    extern Double MapSize;
    extern Double MapSpawnPointRatio;
    extern Double MapCharacterDefaultHeight;
    extern Double MapSpawnRespawnHeight;

    // Character
    extern Double CharacterRadius;
    extern Double CharacterWeight;
    extern Double CharacterDefaultSpeed;
    extern Double CharacterRotateSpeed;
    extern Double CharacterRushSpeed;
    extern Double CharacterFriction;
    extern Double CharacterMapOutSpeed;
    extern Double CharacterElasticity;
    extern Double CharacterMaxSpeed;
    extern Int32 CharacterMaxRushCount;
    extern Double CharacterRushCountRegenSeconds;
    extern Double CharacterRespawnSeconds;
    extern Double CharacterRushMinimumRecastSeconds;

    //Score 점수 판정 무조건 합산
    extern Int32 ScoreKillPlayer;
    extern Int32 ScoreDiePlayer;
    extern Int32 ScoreSelfDiePlayer;
    extern Double ScoreKillerJudgeTime;

    constexpr Int32 NullPlayerIndex = -1;

    bool LoadMapData( const std::string& mapDir );
    void SaveMapData( const std::string& mapDir );
};
