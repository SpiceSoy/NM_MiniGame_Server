//=================================================================================================
// @file Server.h
//
// @brief 미니게임 서버 메인 클래스입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "Define/DataTypes.h"
#include "Network/Session.h"
#include <list>
#include <memory>

namespace Game
{
	class World;
	class PlayerController;
};

namespace Network
{
	class Server
	{
	private:
		UInt16 listenPort = 0;
		SocketHandle listenSocketHandle = 0;
		std::unique_ptr<Game::World> world = nullptr;
		std::list<Session> sessions;
	public:
		Server();
		~Server();
		Void Initialize( UInt16 Port );
		Void Process();
		template <class PacketType>
		void BroadcastPacket( const PacketType* buffer )
		{
			BroadcastByte( reinterpret_cast<Byte*>( buffer ), sizeof( PacketType ) );
		}
		void BroadcastByte( Byte* data, UInt32 size);

	private:
		void InitializeSocket();
		void CreateListenSocket();
		void BindListenSocket();
		void StartListen();
		void Select();
		void RemoveExpiredSession();
		Session& AddNewSession( SocketHandle socket, Game::PlayerController* controller );

		static void ChangeNoneBlockingOption( SocketHandle Socket, Bool IsNoneBlocking );
	};
};