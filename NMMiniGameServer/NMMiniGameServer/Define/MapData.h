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
    // Map
    extern Double MapSize;
    extern Double SpawnPointRatio;

    // Character
    //Double CharacterRadius = 100.0;
    extern Double CharacterRadius;
    extern Double CharacterWeight;
    extern Double CharacterSpring;
    extern Double CharacterDefaultSpeed;
    extern Double CharacterRotateSpeed;
    extern Double CharacterRushSpeed;
    extern Double CharacterFriction;
    extern Double CharacterMapOutSpeed;
    extern Double DefaultHeight;
    extern Double RespawnHeight;
    extern Double CollideForceRatio;
    extern Double MaxSpeed;

    // Game
    extern Int32 MaxRushCount;

    //Score 점수 판정 무조건 합산
    extern Int32 KillerScore;
    extern Int32 DieScore;
    extern Int32 SelfDieScore;

    // Timer
    extern Double RushCountRegenTime;
    extern Double RespawnTime;
    extern Double FirstWaitTime;
    extern Double TotalGameTime;
    extern Double RushMinimumRecastTime;
    extern Double KillerJudgeTime;
    // Null
    constexpr Int32 NullPlayerIndex = -1;

    bool LoadMapData( const std::string& mapDir );
};
