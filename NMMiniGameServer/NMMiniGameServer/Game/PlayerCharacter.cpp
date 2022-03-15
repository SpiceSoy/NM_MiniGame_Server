//=================================================================================================
// @file PlayerController.h
//
// @brief 미니게임 내에서 플레이어의 게임 입력 처리를 담당할 클래스입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#include "Game/PlayerCharacter.h"
#include "Define/DataTypes.h"

void Game::PlayerCharacter::RotateLeft( Double value )
{
	rotation -= value;
}

void Game::PlayerCharacter::RotateRight( Double value )
{
	rotation += value;
}

void Game::PlayerCharacter::MoveForward( Double value )
{
	location += GetForward() * value;
}

const Game::Vector& Game::PlayerCharacter::GetLocation() const
{
	return location;
}

Game::PlayerCharacter& Game::PlayerCharacter::SetLocation( const Vector& location )
{
	this->location = location;
	return *this;
}

const Double& Game::PlayerCharacter::GetRadius() const
{
	return radius;
}

Game::PlayerCharacter& Game::PlayerCharacter::SetRadius( Double radius )
{
	this->radius = radius;
	return *this;
}

const Double& Game::PlayerCharacter::GetRotation() const
{
	return rotation;
}

Game::PlayerCharacter& Game::PlayerCharacter::SetRotation( Double rotation )
{
	this->rotation = rotation;
	return *this;
}

Game::Vector Game::PlayerCharacter::GetForward() const
{
	const Vector defualtForward = Vector(0, 1, 0);
	return defualtForward.Rotated2D(rotation, false);
}
