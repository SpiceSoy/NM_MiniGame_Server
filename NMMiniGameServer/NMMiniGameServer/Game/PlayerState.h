// =================================================================================================
//  @file PlayerState.h
// 
//  @brief 상세 설명을 입력하시오
//  
//  @date 2022/03/23
// 
//  Copyright 2022 2022 Netmarble Neo, Inc. All Rights Reserved.
// =================================================================================================

#pragma once
#include "Define/DataTypes.h"


namespace Game
{
    enum class EPlayerState : Byte
    {
        Spawn = 0,
        Idle = 1,
        Run = 2,
        Rush = 3,
        Rotate = 4,
        Hit = 5,
        Win = 6,
        Lose = 7,
        Die = 8,
        // not used in client
        RotateLeft = 9,
        RotateRight = 10,
    };

    inline const char* to_string( Game::EPlayerState e )
    {
        using namespace Game;
        switch( e )
        {
        case EPlayerState::Spawn:
            return "Spawn";
        case EPlayerState::Idle:
            return "Idle";
        case EPlayerState::Run:
            return "Run";
        case EPlayerState::Rush:
            return "Rush";
        case EPlayerState::Rotate:
            return "Rotate";
        case EPlayerState::Hit:
            return "Hit";
        case EPlayerState::Win:
            return "Win";
        case EPlayerState::Lose:
            return "Lose";
        case EPlayerState::Die:
            return "Die";
        case EPlayerState::RotateLeft:
            return "RotateLeft";
        case EPlayerState::RotateRight:
            return "RotateRight";
        default:
            return "unknown";
        }
    }

};
