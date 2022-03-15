//=================================================================================================
// @file PlayerController.h
//
// @brief �̴ϰ��� ������ �÷��̾��� ���� �Է� ó���� ����� Ŭ�����Դϴ�.
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
	if(packet.left == Packet::EInputState::Click)
	{
		session->LogInput("Click Left\n");
		character->SetSpeed( 0 );
		moveState = EMoveState::RotateLeft;
	}
	else if(packet.right == Packet::EInputState::Click)
	{
		session->LogInput( "Click Right\n" );
		character->SetSpeed( 0 );
		moveState = EMoveState::RotateRight;
	}
	else
	{
		character->SetSpeed( Constant::CharacterDefaultSpeed );
		moveState = EMoveState::MoveForward;
	}

}

