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
#include "Network/Session.h"
#include "Game/PlayerCharacter.h"


void Game::PlayerController::SetSession( Network::Session* session )
{
	this->session = session;
}

void Game::PlayerController::OnReceivedPacket( Byte* ptr )
{

}

void Game::PlayerController::Update( Double deltaTime )
{
	if(character) character->Update(deltaTime);
}
