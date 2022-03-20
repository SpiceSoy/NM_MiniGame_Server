//=================================================================================================
// @file Room.h
//
// @brief 미니게임 룸 하나를 시뮬레이션 하기 위한 클래스입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "Game/PlayerController.h"
#include "Game/PlayerCharacter.h"
#include <vector>


namespace Network
{
    class Session;
};


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

    class Room
    {
    private:
        Int32 currentUserCount = 0;
        const Int32 maxUserCount = 0;
        std::vector< PlayerController > players;
        std::vector< PlayerCharacter > characters;
        std::vector< Int32 > scores;
        Timer startTime;
        ERoomState state;
    public:
        Room( Int32 userCount );
        ~Room();
        PlayerController* GetNewPlayerController( Int32 index, Network::Session* session );
        void Update( Double deltaTime );

        void ReadyToGame();

        template < class PacketType >
        void BroadcastPacket( const PacketType* buffer );

        template < class PacketType >
        void BroadcastPacket( const PacketType* buffer, Int32 expectedUserIndex );

        void BroadcastByte( const Byte* data, UInt32 size );
        void BroadcastByte( const Byte* data, UInt32 size, Int32 expectedUserIndex );
        bool CheckCollisionTwoPlayer( PlayerCharacter& firstChr, PlayerCharacter& secondChr );
        Vector GetSpawnLocation( UInt32 index ) const;
        Vector GetSpawnForward( UInt32 index ) const;
        ERoomState GetState() const;
        void SetState( ERoomState state );
        void BroadcastKillLogPacket( Int32 playerIndex, Int32 killerIndex );
        void OnDiePlayer( const PlayerController* player );
    private:
        void CheckCollision( Double deltaTime );

        bool IsCollide( PlayerCharacter& firstChr, PlayerCharacter& secondChr );

        void BroadcastByteInternal( const Byte* data, UInt32 size, PlayerController* expectedUser );
        void BroadcastStartGame();
        void BroadcastEndGame();
        void LogLine( const char* format, ... ) const;
    };

    // 템플릿 함수들
    template < class PacketType >
    void Room::BroadcastPacket( const PacketType* buffer )
    {
        BroadcastByte( reinterpret_cast< const Byte* >( buffer ), sizeof( PacketType ) );
    }


    template < class PacketType >
    void Room::BroadcastPacket( const PacketType* buffer, Int32 expectedUserIndex )
    {
        BroadcastByte( reinterpret_cast< const Byte* >( buffer ), sizeof( PacketType ), expectedUserIndex );
    }
}
