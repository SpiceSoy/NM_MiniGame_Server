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
		Vector forward;
		Double speed;
		Double radius;
	public:
		PlayerCharacter();
		void RotateLeft( Double value );
		void RotateRight( Double value );
		void SetSpeed( Double value );

		const Vector& GetLocation() const;
		PlayerCharacter& SetLocation( const Vector& location );

		const Double& GetRadius() const;
		PlayerCharacter& SetRadius( Double radius );

		PlayerCharacter& SetRotation( Double rotation );

		const Vector& GetForward() const;
		PlayerCharacter& SetForward( const Vector& forward );
		void Update( Double deltaTime );
		void OnCollide(PlayerCharacter& other);

	};
};