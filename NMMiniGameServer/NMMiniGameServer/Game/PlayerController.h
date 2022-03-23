//=================================================================================================
// @file PlayerController.h
//
// @brief 미니게임 내에서 플레이어의 게임 입력 처리를 담당할 클래스입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "Define/DataTypes.h"
#include "Define/MapData.h"
#include "Game/LambdaFSM.h"
#include "Game/PlayerState.h"
#include "Game/Timer.h"
#include "Game/ItemType.h"
#include <list>


namespace Network
{
    class Session;
};


namespace Packet
{
    struct Header;


    namespace Client
    {
        struct Input;
    };
};


namespace Game
{


    class PlayerController
    {
    private:
        class PlayerCharacter* character = nullptr;
        Network::Session* session = nullptr;
        class Room* room = nullptr;

        LambdaFSM< EPlayerState > fsm;
        Int32 playerIndex = 0;

        EItemType currentItem = EItemType::None;
        Timer timerItemChanged;

        Timer timerRushUse;
        Timer timerRushGen;
        Int32 rushCount = 0;

        Timer timerSpawnStart;
        Timer timerRespawnStart;

        Timer timerLastCollided;
        Int32 lastCollidedPlayerIndex = Constant::NullPlayerIndex;
        Double rushRecastTime = Constant::CharacterRushMinimumRecastSeconds;
    public:
        PlayerController();
        ~PlayerController() = default;
        void SetSession( Network::Session* session );
        void SetCharacter( PlayerCharacter* character );
        void SetRoom( Room* room );
        void SetPlayerIndex( Int32 playerIndex );
        void Initialize();
        Int32 GetPlayerIndex( ) const;
        template < class PacketType >
        void SendPacket( const PacketType* buffer ) const;
        void SendByte( const Byte* data, UInt64 size ) const;

        void Update( Double deltaTime );
        void OnReceivedPacket( const Packet::Header* ptr );
        EPlayerState GetState() const;
        void ChangeState( EPlayerState state );
        void BroadcastObjectLocation( bool isSetHeight ) const;
        void OnCollided( const PlayerController& other );
        Int32 GetLastCollidedPlayerIndex() const;
        void ApplyBuff( EItemType item );
        void ApplyKing();
        void RemoveBuff(  );
        void RemoveKing( );
    private:
        bool IsUseRushStack() const;
        bool CanRush();
        void UseRush();
        void AddStateFunctions();
        void SendStateChangedPacket( EPlayerState state ) const;
        void SendStateChangedPacket() const;
        void SendBuffStartPacket( ) const;
        void SendBuffEndPacket( ) const;
        void SendKingStartPacket( ) const;
        void SendKingEndPacket( ) const;
        void SendRushCountChangedPacket() const;
        void LogLine( const char* format, ... ) const;
        bool IsItemDurationExpired();
    };


    template < class PacketType >
    void PlayerController::SendPacket( const PacketType* buffer ) const
    {
        SendByte( reinterpret_cast< const Byte* >( buffer ), sizeof( PacketType ) );
    }
};
