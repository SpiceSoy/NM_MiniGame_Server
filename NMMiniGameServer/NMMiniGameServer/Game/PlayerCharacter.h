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
#include <set>


namespace Game
{
	class PlayerCharacter
	{

	private:
		Vector location;
		Vector speed;
		Vector forward;
		Double defaultMove;
		Double radius;
		std::set<const void*> collidFillter; // �ѹ��� �浹�ǵ��� ���͸��մϴ�.
	public:
		PlayerCharacter();
		void RotateLeft( Double value );
		void RotateRight( Double value );

		const Vector& GetSpeed() const;
		PlayerCharacter& SetSpeed( const Vector& speed );
		PlayerCharacter& AddSpeed(const Vector& speed);

		const Double& GetMoveSpeed() const;
		PlayerCharacter& SetMoveSpeed(const Double& speed);

		const Vector& GetLocation() const;
		PlayerCharacter& SetLocation( const Vector& location );

		const Double& GetRadius() const;
		PlayerCharacter& SetRadius( Double radius );

		PlayerCharacter& SetRotation( Double rotation );

		const Vector& GetForward() const;
		PlayerCharacter& SetForward( const Vector& forward );
		void Update( Double deltaTime );
		void OnCollide(PlayerCharacter& other);
		void TurnOnColliderFillter(const PlayerCharacter& other);
		void TurnOffColliderFillter(const PlayerCharacter& other);
		bool GetColliderFillter(const PlayerCharacter& other) const;

	};
};