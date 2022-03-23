// =================================================================================================
//  @file RoomState.h
// 
//  @brief 상세 설명을 입력하시오
//  
//  @date 2022/03/23
// 
//  Copyright 2022 2022 Netmarble Neo, Inc. All Rights Reserved.
// =================================================================================================

#pragma once
namespace Game
{
    enum class ERoomState
    {
        Opened,
        Waited,
        Doing,
        End,
        Closed
    };

    inline const char* to_string( Game::ERoomState e )
    {
        using namespace Game;
        switch( e )
        {
        case ERoomState::Opened:
            return "Opened";
        case ERoomState::Waited:
            return "Waited";
        case ERoomState::Doing:
            return "Doing";
        case ERoomState::End:
            return "End";
        case ERoomState::Closed:
            return "Closed";
        default:
            return "unknown";
        }
    }


};