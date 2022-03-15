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
	class Room
	{
	private:
		Int32 currentUserCount = 0;
		const Int32 maxUserCount = 0;
		std::vector<PlayerController> players;
		std::vector<PlayerCharacter> characters;
	public:
		Room(Int32 userCount);
		~Room();
		PlayerController* GetNewPlayerController(Int32 index, Network::Session* session );
		void Update( Double deltaTime );

		void ReadyToGame();

		template <class PacketType>
		void BroadcastPacket( const PacketType* buffer);

		template <class PacketType>
		void BroadcastPacket( const PacketType* buffer, Int32 expectedUserIndex );

		void BroadcastByte( const Byte* data, UInt32 size );
		void BroadcastByte( const Byte* data, UInt32 size, Int32 expectedUserIndex );
	private:
		void CheckCollision();
		void BroadcastByteInternal( const Byte* data, UInt32 size, PlayerController* expectedUser);
		Vector GetSpawnLocation(UInt32 index);
	};

	// 템플릿 함수들
	template <class PacketType>
	void Game::Room::BroadcastPacket( const PacketType* buffer )
	{
		BroadcastByte( reinterpret_cast<const Byte*>( buffer ), sizeof( PacketType ) );
	}

	template <class PacketType>
	void Game::Room::BroadcastPacket( const PacketType* buffer, Int32 expectedUserIndex )
	{
		BroadcastByte( reinterpret_cast<const Byte*>( buffer ), sizeof( PacketType ), expectedUserIndex );
	}
}