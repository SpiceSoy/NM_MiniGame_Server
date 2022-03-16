//=================================================================================================
// @file Server.h
//
// @brief 미니게임 서버 메인 클래스입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#include "Network/Server.h"
#include "Network/Session.h"
#include "Network/UtillFuntions.h"
#include "Define/MapData.h"
#include "Game/Room.h"
#include "Game/PlayerController.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")


Network::Server::Server()
{

}

Network::Server::~Server()
{

}

Void Network::Server::Initialize( UInt16 Port )
{
	listenPort = Port;
	InitializeSocket();
	CreateListenSocket();
	BindListenSocket();
}

Void Network::Server::Process()
{
	std::cout << "Start Server Process\n";
	StartListen();
	std::cout << "Start chat server / port : " << listenPort << "\n";
	auto start = std::chrono::system_clock::now();
	auto prev = start;
	while( true )
	{
		Select();
		auto now = std::chrono::system_clock::now();
		auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( now - prev );
		auto expected = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::milliseconds( Constant::TickTerm ) );
		if( expected > delta )
		{
			std::this_thread::sleep_for( expected - delta );
			now = std::chrono::system_clock::now();
			delta = std::chrono::duration_cast<std::chrono::milliseconds>( now - prev );
		}
		prev = now;
		UpdateRooms( delta.count() / 1000.0f );
	}
	return;
}

void Network::Server::AddRequest( const RequestMatch& req )
{
	matchQueue.push( req );
}

void Network::Server::InitializeSocket()
{
	std::cout << "Initialize Socket\n";
	WORD version = MAKEWORD( 2, 2 );
	WSADATA wsaData;
	ZeroMemory( &wsaData, sizeof( wsaData ) );
	ResultCode initResult = WSAStartup( version, &wsaData );
	if( initResult != 0 )
	{
		PrintLastErrorMessageInFile( "Init" );
		exit( 0 );
	}
}

void Network::Server::CreateListenSocket()
{
	std::cout << "Create Listen Socket\n";
	listenSocketHandle = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( listenSocketHandle == INVALID_SOCKET )
	{
		PrintLastErrorMessageInFile( "CreateListen" );
		exit( 0 );
	}
}

void Network::Server::BindListenSocket()
{
	std::cout << "Bind Listen Socket\n";
	SOCKADDR_IN serverAddress;
	ZeroMemory( &serverAddress, sizeof( SOCKADDR_IN ) );
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl( INADDR_ANY );
	serverAddress.sin_port = htons( listenPort );
	ResultCode bindResult = bind( listenSocketHandle, (SOCKADDR*)&serverAddress, sizeof( serverAddress ) );
	if( bindResult == SOCKET_ERROR )
	{
		PrintLastErrorMessageInFile( "Bind" );
		exit( 0 );
	}
}

void Network::Server::StartListen()
{
	std::cout << "Start Listen\n";
	ResultCode listenResult = listen( listenSocketHandle, SOMAXCONN );
	if( listenResult == SOCKET_ERROR )
	{
		PrintLastErrorMessageInFile( "StartListen" );
		exit( 0 );
	}
	ChangeNoneBlockingOption( listenSocketHandle, true );
}

void Network::Server::Select()
{
	fd_set read;
	fd_set write;
	fd_set except;
	FD_ZERO( &read );
	FD_ZERO( &write );
	FD_ZERO( &except );

	FD_SET( listenSocketHandle, &read );

	for( Session& session : sessions )
	{
		FD_SET( session.GetSocket(), &read );
		FD_SET( session.GetSocket(), &except );
		if( session.HasSendBytes() ) FD_SET( session.GetSocket(), &write );
	}
	timeval val;
	val.tv_sec = 0;
	val.tv_usec = 0;
	ResultCode selectResult = select( NULL, &read, &write, &except, &val ); // time == NULL : 무한히 기다림
	if( selectResult == SOCKET_ERROR ) PrintLastErrorMessageInFile( "Select" );

	//Accept
	if( FD_ISSET( listenSocketHandle, &read ) )
	{
		SOCKET clientSocket;
		SOCKADDR_IN clientAddr;
		INT32 addrLength = sizeof( clientAddr );
		clientSocket = accept( listenSocketHandle, (SOCKADDR*)&clientAddr, &addrLength );
		if( clientSocket == INVALID_SOCKET ) PrintLastErrorMessageInFile( "Accept" );
		else
		{
			char addressStringBuffer[512];
			ZeroMemory( addressStringBuffer, sizeof( addressStringBuffer ) );
			const char* addrString = inet_ntop( AF_INET, (SOCKADDR*)&clientAddr.sin_addr, addressStringBuffer, sizeof( addressStringBuffer ) );

			UInt16 port = ntohs( clientAddr.sin_port );

			Session& clientSession = AddNewSession( clientSocket );
			clientSession.SetAddress( addrString, port );
			clientSession.SetState( Session::EState::Wait );
			clientSession.LogInput( "connected\n" );
		}
	}
	//Recv
	for( Session& session : sessions )
	{
		if( !FD_ISSET( session.GetSocket(), &read ) ) continue;
		session.ProcessReceive();
	}
	//Send
	for( Session& session : sessions )
	{
		if( !FD_ISSET( session.GetSocket(), &write ) ) continue;
		session.ProcessSend();
	}
	//Except
	for( Session& session : sessions )
	{
		if( !FD_ISSET( session.GetSocket(), &except ) ) continue;
		session.Close();
		session.LogInput( "connection error" );
	}
	RemoveExpiredSession();
	QueuingMatch();
}

void Network::Server::RemoveExpiredSession()
{
	auto it = std::remove_if( sessions.begin(), sessions.end(), [] ( Session& session ) { return session.IsClosed(); } );
	sessions.erase( it, sessions.end() );
}

Network::Session& Network::Server::AddNewSession( SocketHandle socket )
{
	sessions.emplace_back( socket, this );
	return sessions.back();
}

void Network::Server::QueuingMatch()
{
	if( matchQueue.empty() ) return;
	// 임시용 한명마다 방 생성
	while( matchQueue.size() >= Constant::MaxUserCount )
	{
		auto& room = AddNewRoom( Constant::MaxUserCount );
		std::cout << "Queuing Request Matches\n";
		for ( Int32 i = 0; i < Constant::MaxUserCount; i++ )
		{
			Network::RequestMatch& request = matchQueue.front();
			request.requester->SetRoom( &room );
			request.requester->SetController( room.GetNewPlayerController( i, request.requester ) );
			matchQueue.pop();
		}
		room.ReadyToGame();
	}
}

void Network::Server::UpdateRooms( Double deltaTime )
{
	for( Game::Room& room : rooms )
	{
		room.Update( deltaTime );
	}
}

Game::Room& Network::Server::AddNewRoom( Int32 userCount )
{
	rooms.emplace_back( userCount );
	return rooms.back();
}

void Network::Server::ChangeNoneBlockingOption( SocketHandle Socket, Bool IsNoneBlocking )
{
	u_long on = IsNoneBlocking;
	ResultCode optionResult = ioctlsocket( Socket, FIONBIO, &on );
	if( optionResult == SOCKET_ERROR ) PrintLastErrorMessageInFile( "ioctlsocket" );
}
