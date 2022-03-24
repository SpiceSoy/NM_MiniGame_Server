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
        ServerKillLog,
        ServerRushUsed,
        ServerStartGame,
        ServerMatchCanceled,
        ServerReadyMatching,
        ServerCancelReadyMatching,
        ServerMapSizeChanged,

        ServerItemSpawn,
        ServerItemRemove,
        ServerBuffStart,
        ServerBuffRemove,

        ClientTypeStart = 0x80,
        ClientRequestFindMatch,
        ClientRequestCancelMatch,
        ClientInput,
        ClientRequestReadyMatch,
        ClientRequestCancelReadyMatch,
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

        struct ReadyMatching
        {
            Header header = SERVER_HEADER( ReadyMatching );
            Int32 playerIndex;
            Int32 maxUser;
        };

        struct CancelReadyMatching
        {
            Header header = SERVER_HEADER( CancelReadyMatching );
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

        struct MatchCanceled
        {
            Header header = SERVER_HEADER( MatchCanceled );
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
            Int32 maxPlayer;
            Int32 scores[ 4 ];
        };

        struct RushUsed
        {
            Header header = SERVER_HEADER( RushUsed );
            Int32 currentRushStack;
        };

        struct KillLog
        {
            Header header = SERVER_HEADER( KillLog );
            Int32 victimIndex;
            Int32 killerIndex;
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


        struct ItemSpawn
        {
            Header header = SERVER_HEADER( ItemSpawn );
            Single locationX;
            Single locationY;
            Single locationZ;
            Byte itemType;
            Int32 itemIndex;
        };

        struct ItemRemove
        {
            Header header = SERVER_HEADER( ItemRemove );
            bool isEaten;
            Int32 itemIndex;
        };

        struct BuffStart
        {
            Header header = SERVER_HEADER ( BuffStart );
            Int32 playerIndex;
            Byte buffType;
        };

        struct BuffRemove
        {
            Header header = SERVER_HEADER( BuffRemove );
            Int32 playerIndex;
            Byte buffType;
        };

        struct MapSizeChanged
        {
            Header header = SERVER_HEADER( MapSizeChanged );
            Int32 mapIndex;
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

        struct RequestReadyMatch
        {
            Header header = CLIENT_HEADER( RequestReadyMatch );
        };

        struct RequestCancelReadyMatch
        {
            Header header = CLIENT_HEADER( RequestCancelReadyMatch );
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
