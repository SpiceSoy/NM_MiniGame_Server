//=================================================================================================
// @file Room.cpp
//
// @brief 미니게임 룸 하나를 시뮬레이션 하기 위한 클래스입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#include "Game/Room.h"


Game::Room::Room()
{

}

Game::Room::Room( Int32 userCount )
{
	players.resize( userCount );
	characters.resize( userCount );
}

Game::Room::~Room()
{

}

Game::PlayerController* Game::Room::GetNewPlayerController( Int32 index, Network::Session* session )
{
	auto& instance = players[index];
	auto& character = characters[index];
	instance.SetSession( session );
	instance.SetCharacter( &character );
	return &instance;
}

void Game::Room::Update( Double deltaTime )
{
	for( PlayerController& player : players )
	{
		player.Update( deltaTime );
	}
}

void Game::Room::BroadcastByte( Byte* data, UInt32 size )
{
	BroadcastByteInternal( data, size, nullptr );
}

void Game::Room::BroadcastByte( Byte* data, UInt32 size, Int32 expectedUserIndex )
{
	BroadcastByteInternal( data, size, &players[expectedUserIndex] );
}

void Game::Room::BroadcastByteInternal( Byte* data, UInt32 size, PlayerController* expectedUser )
{
	for( PlayerController& player : players )
	{
		if( &player == expectedUser ) continue;
		player.SendByte( data, size );
	}
}