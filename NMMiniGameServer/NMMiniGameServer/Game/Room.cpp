//=================================================================================================
// @file Room.cpp
//
// @brief �̴ϰ��� �� �ϳ��� �ùķ��̼� �ϱ� ���� Ŭ�����Դϴ�.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#include "Game/Room.h"
#include "Define/PacketDefine.h"
#include "Define/MapData.h"


Game::Room::Room( Int32 userCount )
	: maxUserCount( userCount )
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


	character.SetLocation( GetSpawnLocation( index ) );

	return &instance;
}

void Game::Room::Update( Double deltaTime )
{
	for( PlayerController& player : players )
	{
		player.Update( deltaTime );
	}
	CheckCollision();
	for( Int32 i = 0; i < maxUserCount; i++ )
	{
		auto& character = characters[i];
		character.Update( deltaTime );
		Packet::Server::ObjectLocation packet;
		packet.targetIndex = i;

		Vector location = character.GetLocation();
		packet.locationX = location.x;
		packet.locationY = location.y;
		//packet.locationZ = location.z;
		packet.locationZ = Constant::DefaultHeight;
		packet.rotation = character.GetRotation();
		BroadcastPacket( &packet );
	}
}

void Game::Room::ReadyToGame()
{
	Packet::Server::StartMatch packet;
	packet.userCount = maxUserCount;
	for( Int32 i = 0; i < maxUserCount; i++ )
	{
		packet.playerIndex = i; // �ӽ�
		players[i].SendPacket( &packet );
	}
}

void Game::Room::BroadcastByte( const Byte* data, UInt32 size )
{
	BroadcastByteInternal( data, size, nullptr );
}

void Game::Room::BroadcastByte( const Byte* data, UInt32 size, Int32 expectedUserIndex )
{
	BroadcastByteInternal( data, size, &players[expectedUserIndex] );
}

void Game::Room::CheckCollision()
{
	for( Int32 first = 0; first < maxUserCount; first++ )
	{
		auto& firstChr = characters[first];
		for( Int32 second = first + 1; second < maxUserCount; second++ )
		{
			auto& secondChr = characters[second];
			if( firstChr.GetRadius() + secondChr.GetRadius() < Vector::Distance( firstChr.GetLocation(), secondChr.GetLocation() ) )
			{
				//Hit
			}
		}
		if( firstChr.GetLocation().GetLength() > Constant::MapSize )
		{
			firstChr.SetLocation( GetSpawnLocation( first ) );
		}
	}
}

void Game::Room::BroadcastByteInternal( const Byte* data, UInt32 size, PlayerController* expectedUser )
{
	for( PlayerController& player : players )
	{
		if( &player == expectedUser ) continue;
		player.SendByte( data, size );
	}
}

Game::Vector Game::Room::GetSpawnLocation( UInt32 index )
{
	Double angle = 360.0 * ( (Double)(index + 1) / (Double)maxUserCount );
	Double spawnLength = Constant::SpawnPointRatio * Constant::MapSize;
	Vector spawnPoint = Vector( 0.0, -spawnLength, 0.0 ).Rotated2D( angle );
	return spawnPoint;
}
