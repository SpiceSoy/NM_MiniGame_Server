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
#include "Game/Item.h"
#include "Game/PlayerController.h"
#include "Game/PlayerCharacter.h"
#include "Game/RoomState.h"
#include <vector>
#include <set>


namespace Network
{
    class Session;
};


namespace Game
{

    class Room
    {
    private:
        Int32 currentUserCount = 0;
        const Int32 maxUserCount = 0;
        std::vector< PlayerController > players;
        std::vector< PlayerCharacter > characters;
        std::vector< Network::Session* > sessions;
        std::vector< Int32 > scores;
        std::set< Int32 > prevMaxUsers;
        std::list<Item> items;
        Timer startTime;
        Timer itemSpawnedTime;
        ERoomState state;
        Int32 itemIndex = 0;
        Double currentMapSize = 0;
        Int32 mapPhase = 0;
    public:
        Room( Int32 userCount );
        ~Room() = default;
        void AddSession( Int32 index, Network::Session* session );
        void Update( Double deltaTime );

        void ReadyToGame();

        template < class PacketType >
        void BroadcastPacket( const PacketType* buffer );

        template < class PacketType >
        void BroadcastPacket( const PacketType* buffer, Int32 expectedUserIndex );

        void BroadcastByte( const Byte* data, UInt32 size );
        void BroadcastByte( const Byte* data, UInt32 size, Int32 expectedUserIndex );
        bool CheckCollisionTwoPlayer( PlayerCharacter& firstChr, PlayerController& firstCon, PlayerCharacter& secondChr, PlayerController& secondCon, Double deltaTime );
        void ResolveCollision( PlayerCharacter& firstChr, PlayerCharacter& secondChr, Double deltaTime, Double penetration );
        void ResolveSpawnCollision( PlayerCharacter& spawnCharacter, PlayerCharacter& other, Double deltaTime, Double penetration );
        Vector GetSpawnLocation( UInt32 index ) const;
        Vector GetSpawnForward( UInt32 index ) const;
        ERoomState GetState() const;
        void SetState( ERoomState state );
        void BroadcastKillLogPacket( Int32 playerIndex, Int32 killerIndex );
        void CheckNewKing();
        void OnDiePlayer( const PlayerController* player );
    private:
        void CheckCollision( Double deltaTime );

        static bool IsCollide( const PlayerCharacter& firstChr, const PlayerCharacter& secondChr, Double& resultPenetration );
        static bool IsCollide( const PlayerCharacter& character, const Item& item );

        void BroadcastByteInternal( const Byte* data, UInt32 size, PlayerController* expectedUser );
        void BroadcastStartGame();
        void BroadcastEndGame();
        void BroadcastMapSizeChanged( Int32 mapIndex );
        void BroadcastSpawnItem( const Item& item );
        void BroadcastRemoveItem( const Item& item, bool isEaten );
        void LogLine( const char* format, ... ) const;
        PlayerController* GetNewPlayerController( Int32 index, Network::Session* session );
        void SpawnItem();
        Vector GetRandomItemLocation() const;
        void CheckCollisionItem();

        void UpdatePlayerController( Double deltaTime );
        void UpdateCharacter( Double deltaTime );
        void CheckStateChange();

        void UpdateItem( Double deltaTime );
        void UpdateMap();

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
