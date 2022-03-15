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
#include "Vector.h"


namespace Game
{
	class PlayerCharacter
	{
	private:
		Vector location;
		Double rotation;
		Double radius;
	public:
		void RotateLeft( Double value );
		void RotateRight( Double value );
		void MoveForward( Double value );

		const Vector& GetLocation() const;
		PlayerCharacter& SetLocation( const Vector& location );

		const Double& GetRadius() const;
		PlayerCharacter& SetRadius( Double radius );

		const Double& GetRotation() const;
		PlayerCharacter& SetRotation( Double rotation );

		Vector GetForward() const;

	};
};