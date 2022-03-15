//=================================================================================================
// @file PlayerController.h
//
// @brief 미니게임 내에서 플레이어의 게임 입력 처리를 담당할 클래스입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "Define/DataTypes.h"


namespace Game
{
	class PlayerCharacter
	{
	private:
		Double x;
		Double y;
		Double z;
	public:
		void Update( Double deltaTime );
		void RotateLeft( Double value );
		void RotateRight( Double value );
		void Rush( Double value );
	};
};