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
	static const Double moveSpeed = 10.0f;
	static const Double rotateSpeed = 10.0f;
	if( !character ) return;

	switch( moveState )
	{
	case Game::PlayerController::EMoveState::None:
		break;
	case Game::PlayerController::EMoveState::MoveForward:
		character->MoveForward( moveSpeed * deltaTime);
		break;
	case Game::PlayerController::EMoveState::Rush:
		break;
	case Game::PlayerController::EMoveState::RotateLeft:
		character->RotateLeft( moveSpeed * deltaTime );
		break;
	case Game::PlayerController::EMoveState::RotateRight:
		character->RotateRight( moveSpeed * deltaTime );
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
		moveState = EMoveState::RotateLeft;
	}
	if(packet.right == Packet::EInputState::Click)
	{
		moveState = EMoveState::RotateRight;
	}
	else
	{
		moveState = EMoveState::MoveForward;
	}

}

