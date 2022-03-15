//=================================================================================================
// @file PlayerController.h
//
// @brief �̴ϰ��� ������ �÷��̾��� ���� �Է� ó���� ����� Ŭ�����Դϴ�.
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