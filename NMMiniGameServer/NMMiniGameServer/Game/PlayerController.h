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
#include "Game/Vector.h"
#include "Game/LambdaFSM.h"
#include "Game/Timer.h"
#include <chrono>


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
	public:
		enum class EMoveState
		{
			None,
			MoveForward,
			Rush,
			RotateLeft,
			RotateRight,
		};

		enum class EState : Byte
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

	private:
		class PlayerCharacter* character = nullptr;
		Network::Session* session = nullptr;
		class Room* room = nullptr;
		Int32 playerIndex = 0;

		Timer timerRushUse;
		Timer timerRushGen;
		Timer timerSpawnStart;
		Timer timerRespawnStart;

		EMoveState moveState = EMoveState::None;
		Int32 rushStack = 3;
		LambdaFSM<EState> fsm;
	public:
		PlayerController( );
		~PlayerController( ) = default;
		void SetSession( Network::Session* session );
		void SetCharacter( PlayerCharacter* character );
		void SetRoom( Room* room );
		void SetPlayerIndex( Int32 playerIndex );
		void Initialize( );

		template <class PacketType>
		void SendPacket( const PacketType* buffer );
		void SendByte( const Byte* data, UInt64 size );

		void Update( Double deltaTime );
		void OnReceivedPacket( const Packet::Header* ptr );
		EState GetState( ) const;
		void ChangeState( EState state );
		void BroadcastObjectLocation(bool isSetHeight) const;
	private:
		bool CanRush( );
		void UseRush( );
		void AddStateFunctions( );
		void SendStateChangedPacket( EState state ) const;
		void SendStateChangedPacket( ) const;
	};

	template <class PacketType>
	void Game::PlayerController::SendPacket( const PacketType* buffer )
	{
		SendByte( reinterpret_cast<const Byte*>( buffer ), sizeof( PacketType ) );
	}

};