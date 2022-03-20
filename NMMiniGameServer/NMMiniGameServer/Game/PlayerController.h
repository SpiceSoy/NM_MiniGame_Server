//=================================================================================================
// @file PlayerController.h
//
// @brief �̴ϰ��� ������ �÷��̾��� ���� �Է� ó���� ����� Ŭ�����Դϴ�.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "Define/DataTypes.h"
#include "Define/MapData.h"
#include "Game/Vector.h"
#include "Game/LambdaFSM.h"
#include "Game/Timer.h"
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

    class PlayerController
    {
    public:
    private:
        class PlayerCharacter* character = nullptr;
        Network::Session* session = nullptr;
        class Room* room = nullptr;

        LambdaFSM< EPlayerState > fsm;
        Int32 playerIndex = 0;
    public:
        Int32 GetPlayerIndex() const;
    private:
        std::list< Timer > rushQueue;
        Timer timerRushUse;
        Timer timerRushGen;
        Int32 rushCount = 0;

        Timer timerSpawnStart;
        Timer timerRespawnStart;

        Timer timerLastCollided;
        Int32 lastCollidedPlayerIndex = Constant::NullPlayerIndex;
    public:
        PlayerController();
        ~PlayerController() = default;
        void SetSession( Network::Session* session );
        void SetCharacter( PlayerCharacter* character );
        void SetRoom( Room* room );
        void SetPlayerIndex( Int32 playerIndex );
        void Initialize();

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
    private:
        bool CanRush();
        void UseRush();
        void AddStateFunctions();
        void SendStateChangedPacket( EPlayerState state ) const;
        void SendStateChangedPacket() const;
        void SendRushCountChangedPacket() const;
        void LogLine( const char* format, ... ) const;
    };


    template < class PacketType >
    void PlayerController::SendPacket( const PacketType* buffer ) const
    {
        SendByte( reinterpret_cast< const Byte* >( buffer ), sizeof( PacketType ) );
    }
};
