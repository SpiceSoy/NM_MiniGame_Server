//=================================================================================================
// @file Server.h
//
// @brief 미니게임 서버 메인 클래스입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "Define/DataTypes.h"
#include "Define/MapData.h"
#include "Network/Session.h"
#include <array>
#include <list>
#include <memory>
#include <queue>
#include <chrono>


namespace Game
{
    class Room;
    class PlayerController;
};


namespace Network
{
    struct RequestMatch
    {
        std::chrono::system_clock::time_point reqTime;
        Session* requester = nullptr;
    };

    struct ReadyMatch
    {
        std::chrono::system_clock::time_point reqTime;
        Int32 userCount = Constant::MaxUserCount;
        Int32 readyUserCount = 0;
        std::array< Session*, Constant::MaxUserCount > users;
    };

    class Server
    {
    private:
        UInt16 listenPort = 0;
        SocketHandle listenSocketHandle = 0;
        std::list< Game::Room > rooms;
        std::list< Session > sessions;
        std::list< RequestMatch > matchQueue;
        std::list< ReadyMatch > readyMatches;
        bool turnOnMatch = false;
    public:
        Server();
        ~Server();
        Void Initialize( UInt16 Port );
        Void Process();
        void AddRequest( const RequestMatch& req );
        void CancelRequest( Session* requester );
        void PostReadyMatch( Session* requester );
        void PostCancelReadyMatch( Session* requester );
    private:
        void InitializeSocket();
        void CreateListenSocket();
        void BindListenSocket();
        void StartListen();
        void Select();
        void RemoveExpiredSession();
        Session& AddNewSession( SocketHandle socket );
        void QueuingMatch();
        void UpdateRooms( Double deltaTime );
        Game::Room& AddNewRoom( Int32 userCount );

        static void ChangeNoneBlockingOption( SocketHandle Socket, Bool IsNoneBlocking );
    };
};
