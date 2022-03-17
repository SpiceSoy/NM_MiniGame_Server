//=================================================================================================
// @file PlayerController.h
//
// @brief 미니게임 내에서 플레이어의 게임 입력 처리를 담당할 클래스입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#include "Game/PlayerController.h"
#include "Define/DataTypes.h"
#include "Define/PacketDefine.h"
#include "Define/MapData.h"
#include "Network/Session.h"
#include "Game/PlayerCharacter.h"

using namespace std::literals::chrono_literals;


Game::PlayerController::PlayerController( )
{
	auto now = SystemClock::now( );
	allowedRushTime = now;
	RushGenTime = now;
}

void Game::PlayerController::SetSession( Network::Session* session )
{
	this->session = session;
}

void Game::PlayerController::SetCharacter( PlayerCharacter* character )
{
	this->character = character;
}

void Game::PlayerController::SendByte( const Byte* data, UInt64 size )
{
	if( !session ) return;
	session->SendByte( data, size );
}

void Game::PlayerController::Update( Double deltaTime )
{
	if( !character ) return;
	UpdateMove( deltaTime );
}

void Game::PlayerController::OnReceivedPacket( const Packet::Header* ptr )
{
	if( !ptr ) return;
	switch( ptr->Type )
	{
	case Packet::EType::ClientInput:
		OnReceivedInputPacket( *reinterpret_cast<const Packet::Client::Input*>( ptr ) );
		return;
	}
}

void Game::PlayerController::OnReceivedInputPacket( const Packet::Client::Input& packet )
{
	if( packet.left == Packet::EInputState::Click )
	{
		session->LogInput( "Click Left\n" );
		character->SetMoveSpeed( 0 );
		moveState = EMoveState::RotateLeft;
	}
	else if( packet.right == Packet::EInputState::Click )
	{
		session->LogInput( "Click Right\n" );
		character->SetMoveSpeed( 0 );
		moveState = EMoveState::RotateRight;
	}
	else
	{
		character->SetMoveSpeed( Constant::CharacterDefaultSpeed );
		moveState = EMoveState::MoveForward;
	}
	if( packet.rush == Packet::EInputState::Click )
	{
		session->LogInput( "Click Rush\n" );
		UseRush();
	}

}

void Game::PlayerController::UpdateMove( Double deltaTime )
{
	switch( moveState )
	{
	case Game::PlayerController::EMoveState::None:
		break;
	case Game::PlayerController::EMoveState::MoveForward:
	{
	}
	break;
	case Game::PlayerController::EMoveState::Rush:

		break;
	case Game::PlayerController::EMoveState::RotateLeft:
	{
		character->RotateLeft( Constant::CharacterRotateSpeed * deltaTime );
	}
	break;
	case Game::PlayerController::EMoveState::RotateRight:
	{
		character->RotateRight( Constant::CharacterRotateSpeed * deltaTime );
	}
	break;
	}
}

void Game::PlayerController::UseRush( )
{
	auto now = SystemClock::now( );
	if( CanRush() &&  (now > allowedRushTime) )
	{
		allowedRushTime = now + 1s;
		character->AddSpeed( character->GetForward( ) * Constant::CharacterRushSpeed );
	}
}

bool Game::PlayerController::CanRush( )
{
	auto now = SystemClock::now( );
	return (RushGenTime < now);
}

