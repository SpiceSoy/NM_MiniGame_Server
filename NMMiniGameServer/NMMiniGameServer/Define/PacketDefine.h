//=================================================================================================
// @file PacketDefine.h
//
// @brief 클라이언트와 서버 간 사용되는 패킷을 정의하는 헤더파일입니다. 
//		  모든 바이트 정렬은 네트워크 바이트 정렬로 네트워크에 송신해야 합니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "Define/DataTypes.h"
#define SERVER_HEADER(x) {sizeof(x), (EType::Server##x)}
#define CLIENT_HEADER(x) {sizeof(x), (EType::Client##x)}
#pragma pack(push, 1)


namespace Packet
{
    enum class EType : Byte
    {
        ServerTypeStart = 0,
        ServerStartMatch,
        ServerObjectLocation,
        ServerPlayerRushCountChanged,
        ServerObjectStateChanged,
        ServerChangeMatchingInfo,
        ServerEndOfMatching,
        ServerEndGame,
        ServerRushUsed,
        ServerStartGame,
        ClientTypeStart = 0x80,
        ClientRequestFindMatch,
        ClientRequestCancelMatch,
        ClientInput,
    };

    enum class EInputState : Byte
    {
        None,
        Click,
        Press,
        Release,
    };

    struct Header
    {
        Byte Size;
        EType Type;
    };


    namespace Server
    {
        struct ChangeMatchingInfo
        {
            Header header = SERVER_HEADER( ChangeMatchingInfo );
            Int32 currentUser;
            Int32 maxUser;
        };

        struct EndOfMatching
        {
            Header header = SERVER_HEADER( EndOfMatching );
            Int32 yourIndex;
            Int32 maxUser;
        };

        struct StartMatch
        {
            Header header = SERVER_HEADER( StartMatch );
            Int32 playerIndex;
            Int32 userCount;
        };

        struct PlayerRushCountChanged
        {
            Header header = SERVER_HEADER( PlayerRushCountChanged );
            Int32 count;
        };

        struct ObjectStateChanged
        {
            Header header = SERVER_HEADER( ObjectStateChanged );
            Int32 targetIndex;
            Int32 chracterState;
        };

        struct StartGame
        {
            Header header = SERVER_HEADER( StartGame );
            Int32 GameTime;
        };

        struct EndGame
        {
            Header header = SERVER_HEADER( EndGame );
            Int32 scores[ 6 ];
        };

        struct RushUsed
        {
            Header header = SERVER_HEADER( RushUsed );
            Int32 currentRushStack;
        };

        struct ObjectLocation
        {
            Header header = SERVER_HEADER( ObjectLocation );
            Int32 targetIndex;
            Int32 chracterState;
            bool isSetHeight;

            Single locationX;
            Single locationY;
            Single locationZ;

            Single forwardX;
            Single forwardY;
            Single forwardZ;

            Single velocityX;
            Single velocityY;
            Single velocityZ;
        };
    };


    namespace Client
    {
        struct RequestFindMatch
        {
            Header header = CLIENT_HEADER( RequestFindMatch );
        };

        struct RequestCancelMatch
        {
            Header header = CLIENT_HEADER( RequestCancelMatch );
        };

        struct AnswerMatching
        {
            Header header = CLIENT_HEADER( RequestCancelMatch );
            bool isReady;
        };

        struct Input
        {
            Header header = CLIENT_HEADER( Input );
            EInputState left;
            EInputState right;
            EInputState rush;
        };
    };
};


#pragma pack(pop)
