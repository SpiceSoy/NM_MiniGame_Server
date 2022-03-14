//=================================================================================================
// @file Session.h
//
// @brief �÷��̾�� ������ �����ϱ� ���� ���� Ŭ�����Դϴ�.
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
};
namespace Network
{
	class Session
	{
	public:
		enum class EState
		{
			WaitLogin,
			MainMenu,
			InGame,
			Closed,
			Empty
		};
	private:
		SocketHandle socket;

		UInt64 recvBytes = 0;
		std::vector<Byte> readBuffer;

		UInt64 sendBytes = 0;
		std::vector<Byte> sendBuffer;

		std::string id;
		std::string addressText;
		UInt16 port;

		EState state;
		Game::PlayerController* player;

	public:
		Session( SocketHandle socket, Game::PlayerController* player );

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

		template <class PacketType>
		void SendPacket( const PacketType* buffer);
		void SendByte( const Byte* data, UInt64 size );
	};

	template <class PacketType>
	void Network::Session::SendPacket( const PacketType* buffer )
	{
		SendByte(reinterpret_cast<Byte*>(buffer), sizeof(PacketType));
	}

};