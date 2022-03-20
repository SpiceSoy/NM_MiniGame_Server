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

using TimeSecond = std::chrono::duration< double >;

namespace Constant
{
    using namespace std::chrono_literals;
    // Server
    constexpr UInt32 TickTerm = 1000 / 60;
    constexpr UInt32 MaxUserCount = 2;
    // Map
    constexpr Double MapSize = 1300;
    constexpr Double SpawnPointRatio = 0.75;

    // Character
    constexpr Double CharacterRadius = 100.0;
    constexpr Double CharacterWeight = 10.0;
    constexpr Double CharacterSpring = 1.0;
    constexpr Double CharacterDefaultSpeed = 600.0;
    constexpr Double CharacterRotateSpeed = 360.0;
    constexpr Double CharacterRushSpeed = 1000.0;
    constexpr Double CharacterFriction = 1000.0;
    constexpr Double CharacterMapOutSpeed = 300.0;
    constexpr Double DefaultHeight = -84.787506;
    constexpr Double RespawnHeight = -84.787506;
    constexpr Double CollideForceRatio = 2.0;

    // Game
    constexpr Int32 MaxRushCount = 3;

    //Score 점수 판정 무조건 합산
    constexpr Int32 KillerScore = 1; 
    constexpr Int32 DieScore = -1;
    constexpr Int32 SelfDieScore = DieScore;

    // Timer
    constexpr TimeSecond RushCountRegenTime = 7.0s;
    constexpr TimeSecond RespawnTime = 1.5s;
    constexpr TimeSecond FirstWaitTime = 1.5s;
    constexpr TimeSecond TotalGameTime = 60s;
    constexpr TimeSecond RushMinimumRecastTime = 1s;
    constexpr TimeSecond KillerJudgeTime = 1s; // 공격 판정 남는 시간

    // Null
    constexpr Int32 NullPlayerIndex = -1;
};
