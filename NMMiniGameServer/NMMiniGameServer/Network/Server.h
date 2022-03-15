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
#include <queue>
#include <chrono>

namespace Game
{
	class Room;
	class PlayerController;
};

namespace Network
{
	struct RequestMatch
	{
		std::chrono::system_clock::time_point reqTime;
		Session* requester = nullptr;
	};
	class Server
	{
	private:
		UInt16 listenPort = 0;
		SocketHandle listenSocketHandle = 0;
		std::list<Game::Room> rooms;
		std::list<Session> sessions;
		std::queue<RequestMatch> matchQueue;
	public:
		Server();
		~Server();
		Void Initialize( UInt16 Port );
		Void Process();

	private:
		void InitializeSocket();
		void CreateListenSocket();
		void BindListenSocket();
		void StartListen();
		void Select();
		void RemoveExpiredSession();
		Session& AddNewSession( SocketHandle socket );
		void QueuingMatch();
		void UpdateRooms(Double deltaTime);
		Game::Room& AddNewRoom( Int32 userCount );

		static void ChangeNoneBlockingOption( SocketHandle Socket, Bool IsNoneBlocking );
	};
};