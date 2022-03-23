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

    extern Double GameFirstWaitSeconds;
    extern Double GameTotalTimeSeconds;

    // Map
    extern Double MapSize;
    extern Double MapSpawnPointRatio;
    extern Double MapCharacterDefaultHeight;
    extern Double MapSpawnRespawnHeight;

    extern Double MapFirstDisableSeconds;
    extern Double MapSecondDisableSeconds;
    extern Double MapFirstDisableSize;
    extern Double MapSecondDisableSize;



    //
    extern Double ItemRadius;
    extern Double ItemRegenMinSeconds;
    extern Double ItemRegenMaxSeconds;
    extern Double ItemSpawnLocationMapSizeRatio;
    extern Double ItemCloverSpawnStartTime;
    extern Double ItemLifeMaxSeconds;
    extern Double ItemSameTimeMaxSpawnCount;

    //���� ������ ȿ�� �� ���ӽð�
    extern Double ItemFortifyWeight;
    extern Double ItemFortifyDurationSeconds;

    extern Double ItemSwiftMoveSpeed;
    extern Double ItemSwiftMoveDurationSeconds;
    \
    extern Double ItemStrongWillRecastSeconds;
    extern Double ItemStrongWillDurationSeconds;

    extern Double ItemGhostDurationSeconds;

    extern Double ItemCloverWeight;
    extern Double ItemCloverSpeed;
    extern Double ItemCloverDurationSeconds;

    extern Double CharacterKingRadius; // ĳ���� �浹 ���� ũ��

    // Character
    extern Double CharacterRadius;
    extern Double CharacterWeight;
    extern Double CharacterInfiniteWeight;
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

    //Score ���� ���� ������ �ջ�
    extern Int32 ScoreKillPlayer;
    extern Int32 ScoreDiePlayer;
    extern Int32 ScoreSelfDiePlayer;
    extern Double ScoreKillerJudgeTime;

    constexpr Int32 NullPlayerIndex = -1;

    bool LoadMapData( const std::string& mapDir );
    void SaveMapData( const std::string& mapDir );
};
