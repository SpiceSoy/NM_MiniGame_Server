//=================================================================================================
// @file World.cpp
//
// @brief �̴ϰ����� �ùķ��̼� �ϱ� ���� Ŭ�����Դϴ�.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#include "Game/World.h"

Game::World::World()
{

}

Game::World::~World()
{

}

Game::PlayerController* Game::World::GetNewPlayerController()
{
	players.emplace_back();
	return &players.back();
}

void Game::World::Update( Double deltaTime )
{
	for (PlayerController& player : players)
	{
		player.Update(deltaTime);
	}
}
