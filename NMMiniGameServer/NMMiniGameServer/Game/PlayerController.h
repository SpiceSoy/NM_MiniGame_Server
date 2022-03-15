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
		enum class EMoveState
		{
			None,
			MoveForward,
			Rush,
			RotateLeft,
			RotateRight,
		};
	private:
		class PlayerCharacter* character = nullptr;
		Network::Session* session = nullptr;
		EMoveState moveState = EMoveState::None;
	public:
		void SetSession( Network::Session* session );
		void SetCharacter( PlayerCharacter* character );

		template <class PacketType>
		void SendPacket( const PacketType* buffer );
		void SendByte( const Byte* data, UInt64 size );

		void Update( Double deltaTime );
		void OnReceivedPacket( const Packet::Header* ptr );
	private:
		void OnReceivedInputPacket( const Packet::Client::Input& packet);
	};

	template <class PacketType>
	void Game::PlayerController::SendPacket( const PacketType* buffer )
	{
		SendByte( reinterpret_cast<Byte*>( buffer ), sizeof( PacketType ) );
	}

};