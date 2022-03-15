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

namespace Network
{
	class Session;
};

namespace Game
{
	class PlayerController
	{
	private:
		class PlayerCharacter* character = nullptr;
		Network::Session* session = nullptr;
	public:
		void SetSession( Network::Session* session );
		void OnReceivedPacket( Byte* ptr );

		template <class PacketType>
		void SendPacket( const PacketType* buffer );
		void SendByte( const Byte* data, UInt64 size );

		void Update( Double deltaTime );
	};

	template <class PacketType>
	void Game::PlayerController::SendPacket( const PacketType* buffer )
	{
		SendByte( reinterpret_cast<Byte*>( buffer ), sizeof( PacketType ) );
	}

};