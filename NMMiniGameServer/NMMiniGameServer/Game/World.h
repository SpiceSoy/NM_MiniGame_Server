//=================================================================================================
// @file World.h
//
// @brief �̴ϰ����� �ùķ��̼� �ϱ� ���� Ŭ�����Դϴ�.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "PlayerController.h"
#include <vector>


namespace Game
{
	class World
	{
	private:
		Int32 currentUserCount = 0;
		std::vector<PlayerController> players;
	public:
		World();
		~World();
		PlayerController* GetNewPlayerController();
		void Update( Double deltaTime );
	};
}