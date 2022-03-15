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
#include "Network/Session.h"
#include "Game/PlayerCharacter.h"


void Game::PlayerController::SetSession( Network::Session* session )
{
	this->session = session;
}

void Game::PlayerController::OnReceivedPacket( Byte* ptr )
{

}

void Game::PlayerController::SendByte( const Byte* data, UInt64 size )
{
	if(!session) return;
	session->SendByte(data, size);
}

void Game::PlayerController::Update( Double deltaTime )
{
	if(character) character->Update(deltaTime);
}
