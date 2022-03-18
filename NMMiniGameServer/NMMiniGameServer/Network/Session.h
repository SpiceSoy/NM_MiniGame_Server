//=================================================================================================
// @file Session.h
//
// @brief 플레이어와 연결을 유지하기 위한 세션 클래스입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "Define/DataTypes.h"
#include <vector>
#include <string>
#include <stdarg.h>


namespace Game
{
    class PlayerController;
    class Room;
};


namespace Packet
{
    struct Header;
};


namespace Network
{
    class Session
    {
    public:
        enum class EState
        {
            Wait,
            QueueingMatch,
            InRoom,
            Closed,
            Empty
        };

    private:
        SocketHandle socket;

        UInt64 recvBytes = 0;
        std::vector< Byte > readBuffer;

        UInt64 sendBytes = 0;
        std::vector< Byte > sendBuffer;

        std::string id;
        std::string addressText;
        UInt16 port;

        EState state = EState::Wait;
        Game::PlayerController* contoller = nullptr;
        Game::Room* room = nullptr;
        class Server* server = nullptr; // 매칭용, 이후에 매치메이커로 바꿔야함.

    public:
        Session( SocketHandle socket, class Server* server );

        SocketHandle GetSocket() const;
        Bool HasSendBytes() const;
        const std::string& GetId() const;
        const std::string& GetAddress() const;
        UInt16 GetPort() const;
        EState GetState() const;
        Bool IsClosed() const;
    public:
        void SetState( EState state );
        void ProcessSend();
        void ProcessReceive();

        void Close();
        void SetAddress( const Char* address, UInt16 port );
        void LogInput( const Char* input ) const;
        void SendByte( const Byte* data, UInt64 size );
        void OnReceivedPacketInWaitting( const Packet::Header* data );

        void SetRoom( Game::Room* room );
        void SetController( Game::PlayerController* contoller );
    };
};
