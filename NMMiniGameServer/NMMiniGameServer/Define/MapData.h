//=================================================================================================
// @file MapData.h
//
// @brief Ŭ���̾�Ʈ �� ũ�� ���Ǹ� ���� ����Դϴ�.
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

    //Score ���� ���� ������ �ջ�
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
