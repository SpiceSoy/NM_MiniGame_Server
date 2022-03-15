//=================================================================================================
// @file Room.h
//
// @brief �̴ϰ��� �� �ϳ��� �ùķ��̼� �ϱ� ���� Ŭ�����Դϴ�.
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
	class Room
	{
	private:
		Int32 currentUserCount = 0;
		std::vector<PlayerController> players;
		std::vector<PlayerCharacter> characters;
	public:
		Room();
		Room(Int32 userCount);
		~Room();
		PlayerController* GetNewPlayerController(Int32 index, Network::Session* session );
		void Update( Double deltaTime );

		template <class PacketType>
		void BroadcastPacket( const PacketType* buffer);

		template <class PacketType>
		void BroadcastPacket( const PacketType* buffer, Int32 expectedUserIndex );

		void BroadcastByte( Byte* data, UInt32 size );
		void BroadcastByte( Byte* data, UInt32 size, Int32 expectedUserIndex );
	private:
		void BroadcastByteInternal( Byte* data, UInt32 size, PlayerController* expectedUser);
	};

	// ���ø� �Լ���
	template <class PacketType>
	void Game::Room::BroadcastPacket( const PacketType* buffer )
	{
		BroadcastByte( reinterpret_cast<Byte*>( buffer ), sizeof( PacketType ) );
	}

	template <class PacketType>
	void Game::Room::BroadcastPacket( const PacketType* buffer, Int32 expectedUserIndex )
	{
		BroadcastByte( reinterpret_cast<Byte*>( buffer ), sizeof( PacketType ), expectedUserIndex );
	}
}