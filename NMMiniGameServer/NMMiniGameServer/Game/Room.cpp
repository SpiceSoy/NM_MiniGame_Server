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
#include "Define/PacketDefine.h"
#include "Define/MapData.h"
#include <iostream>


Game::Room::Room( Int32 userCount )
	: maxUserCount( userCount )
{
	players.resize( userCount );
	characters.resize( userCount );
}

Game::Room::~Room( )
{

}

Game::PlayerController* Game::Room::GetNewPlayerController( Int32 index, Network::Session* session )
{
	auto& instance = players[index];
	auto& character = characters[index];
	instance.SetSession( session );
	instance.SetCharacter( &character );
	instance.SetRoom( this );
	instance.SetPlayerIndex( index );
	character.SetLocation( GetSpawnLocation( index ) );
	character.SetForward( GetSpawnForward( index ) );

	return &instance;
}

void Game::Room::Update( Double deltaTime )
{
	for( PlayerController& player : players )
	{
		player.Update( deltaTime );
	}
	CheckCollision( deltaTime );
	for( Int32 i = 0; i < maxUserCount; i++ )
	{
		PlayerCharacter& character = characters[i];
		PlayerController& controller = players[i];
		character.Update( deltaTime );
		controller.BroadcastObjectLocation( false );
	}
}

void Game::Room::ReadyToGame( )
{
	Packet::Server::StartMatch packet;
	packet.userCount = maxUserCount;
	for( Int32 i = 0; i < maxUserCount; i++ )
	{
		packet.playerIndex = i; // 임시
		players[i].SendPacket( &packet );
	}
	for( Int32 i = 0; i < maxUserCount; i++ )
	{
		players[i].BroadcastObjectLocation( true );
		players[i].Initialize( );
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

void Game::Room::CheckCollisionTwoPlayer( Game::PlayerCharacter& firstChr, Game::PlayerCharacter& secondChr )
{
	bool isCollide = IsCollide( firstChr, secondChr );
	bool isLastCollided = firstChr.GetColliderFillter( secondChr ) || secondChr.GetColliderFillter( firstChr );
	if( isCollide && !isLastCollided )
	{
		firstChr.TurnOnColliderFillter( secondChr );
		secondChr.TurnOnColliderFillter( firstChr );

		//std::cout << "Collide!" << std::endl;

		Vector normal = firstChr.GetLocation( ) - secondChr.GetLocation( );
		normal.Normalize( );
		Vector firstForward = firstChr.GetForward( );
		Vector firstReflected = Vector::Reflect( normal, firstForward ).Normalized( );
		Vector secondForward = secondChr.GetForward( );
		Vector secondReflected = Vector::Reflect( -normal, secondForward ).Normalized( );

		firstChr.SetSpeed( firstReflected * Constant::CharacterDefaultSpeed * 5 );
		secondChr.SetSpeed( secondReflected * Constant::CharacterDefaultSpeed * 5 );

		while( IsCollide( firstChr, secondChr ) )
		{
			firstChr.SetLocation( firstChr.GetLocation( ) + firstReflected * 0.1f );
			secondChr.SetLocation( secondChr.GetLocation( ) + secondReflected * 0.1f );
		}
		firstChr.OnCollide( secondChr );
		secondChr.OnCollide( firstChr );
	}
	else
	{
		firstChr.TurnOffColliderFillter( secondChr );
		secondChr.TurnOffColliderFillter( firstChr );
	}
}

void Game::Room::CheckCollision( Double deltaTime )
{
	//std::cout << "CollideFrame" << std::endl;
	for( Int32 first = 0; first < maxUserCount; first++ )
	{
		PlayerCharacter& firstChr = characters[first];
		PlayerController& firstCon = players[first];
		for( Int32 second = first + 1; second < maxUserCount; second++ )
		{
			PlayerCharacter& secondChr = characters[second];
			CheckCollisionTwoPlayer( firstChr, secondChr );
		}
		bool isOutOfMap = firstChr.GetLocation( ).GetLength( ) > Constant::MapSize;
		if( isOutOfMap && firstCon.GetState( ) != PlayerController::EState::Die )
		{
			firstCon.ChangeState( PlayerController::EState::Die );
		}
	}
}

bool Game::Room::IsCollide( Game::PlayerCharacter& firstChr, Game::PlayerCharacter& secondChr )
{
	Double dist = Vector::Distance( firstChr.GetLocation( ), secondChr.GetLocation( ) );
	Double sumRadius = firstChr.GetRadius( ) + secondChr.GetRadius( );
	return sumRadius > dist;
}

void Game::Room::BroadcastByteInternal( const Byte* data, UInt32 size, PlayerController* expectedUser )
{
	for( PlayerController& player : players )
	{
		if( &player == expectedUser ) continue;
		player.SendByte( data, size );
	}
}

Game::Vector Game::Room::GetSpawnLocation( UInt32 index ) const
{
	Double angle = 360.0 * ( static_cast<Double>( index + 1 ) / static_cast<Double>( maxUserCount ) );
	Double spawnLength = Constant::SpawnPointRatio * Constant::MapSize;
	Vector spawnPoint = Vector( 0.0, -spawnLength, 0.0 ).Rotated2D( angle );
	return spawnPoint;
}

Game::Vector Game::Room::GetSpawnForward( UInt32 index ) const
{
	Vector start = GetSpawnLocation( index );
	Vector end = Vector( 0 );
	return ( end - start ).Normalized( );
}
