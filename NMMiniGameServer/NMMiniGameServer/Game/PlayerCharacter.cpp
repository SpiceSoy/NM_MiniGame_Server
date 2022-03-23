//=================================================================================================
// @file PlayerController.h
//
// @brief �̴ϰ��� ������ �÷��̾��� ���� �Է� ó���� ����� Ŭ�����Դϴ�.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#include "Game/PlayerCharacter.h"
#include "Define/DataTypes.h"
#include "Define/MapData.h"
#include <cassert>
#include <iostream>


Game::PlayerCharacter::PlayerCharacter()
    : location( 0 ), speed( 0 ), forward( 0, 1, 0 ), defaultMove( Constant::CharacterDefaultSpeed ), radius( Constant::CharacterRadius ), weight( Constant::CharacterWeight )
{
}


void Game::PlayerCharacter::RotateLeft( Double value )
{
    forward.Rotate2D( -value );
}


void Game::PlayerCharacter::RotateRight( Double value )
{
    forward.Rotate2D( value );
}


const Game::Vector& Game::PlayerCharacter::GetSpeed() const
{
    return speed;
}


Game::PlayerCharacter& Game::PlayerCharacter::SetSpeed( const Vector& speed )
{
    this->speed = speed;
    return *this;
}


Game::PlayerCharacter& Game::PlayerCharacter::AddSpeed( const Vector& speed )
{
    this->speed += speed;
    return *this;
}


const Double& Game::PlayerCharacter::GetMoveSpeed() const
{
    return defaultMove;
}


Game::PlayerCharacter& Game::PlayerCharacter::SetMoveSpeed( const Double& speed )
{
    defaultMove = speed;
    return *this;
}


const Double& Game::PlayerCharacter::GetWeight() const
{
    return  isInfiniteWeight ? Constant::CharacterInfiniteWeight : weight;
}


Game::PlayerCharacter& Game::PlayerCharacter::SetWeight( const Double& weight )
{
    this->weight = weight;
    return *this;
}


Game::PlayerCharacter& Game::PlayerCharacter::SetInfiniteWeight( bool isInfiniteWeight )
{
    this->isInfiniteWeight = isInfiniteWeight;
    return *this;
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


Game::PlayerCharacter& Game::PlayerCharacter::SetRotation( Double rotation )
{
    const auto defualtForward = Vector( 0, 1, 0 );
    forward = defualtForward.Rotated2D( rotation, false );
    return *this;
}


void Game::PlayerCharacter::StartMove()
{
    this->isMove = true;
}


void Game::PlayerCharacter::StopMove()
{
    this->isMove = false;
}


const Game::Vector& Game::PlayerCharacter::GetForward() const
{
    return forward;
}


Game::Vector Game::PlayerCharacter::GetFinalSpeed() const
{
    return speed + forward * ( isMove ? defaultMove : 0.0 );
}


Game::PlayerCharacter& Game::PlayerCharacter::SetForward( const Vector& forward )
{
    this->forward = forward;
    return *this;
}


void Game::PlayerCharacter::Update( Double deltaTime )
{
    location += GetFinalSpeed() * deltaTime;
    assert( location.IsNan() == false );
    // ������
    if ( !speed.IsZero() )
    {
        Vector Friction = -speed.Normalized() * Constant::CharacterFriction * deltaTime * 2.0f;
        if ( speed.GetSqr() < Friction.GetSqr() ) speed = Vector::Zero();
        else speed = speed + Friction;
    }
}


void Game::PlayerCharacter::TurnOnColliderFillter( const PlayerCharacter& other )
{
    collidFillter.insert( &other );
}


void Game::PlayerCharacter::TurnOffColliderFillter( const PlayerCharacter& other )
{
    collidFillter.erase( &other );
}


bool Game::PlayerCharacter::GetColliderFillter( const PlayerCharacter& other ) const
{
    return collidFillter.count( &other );
}
