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
        Double weight;
        bool isMove = false;
        bool isInfiniteWeight = false;
        std::set< const void* > collidFillter; // 한번만 충돌되도록 필터링합니다.
    public:
        PlayerCharacter();
        void RotateLeft( Double value );
        void RotateRight( Double value );

        const Vector& GetSpeed() const;
        PlayerCharacter& SetSpeed( const Vector& speed );
        PlayerCharacter& AddSpeed( const Vector& speed );

        const Double& GetMoveSpeed() const;
        PlayerCharacter& SetMoveSpeed( const Double& speed );

        const Double& GetWeight( ) const;
        PlayerCharacter& SetWeight( const Double& weight );
        PlayerCharacter& SetInfiniteWeight( bool isInfiniteWeight );

        const Vector& GetLocation() const;
        PlayerCharacter& SetLocation( const Vector& location );

        const Double& GetRadius() const;
        PlayerCharacter& SetRadius( Double radius );

        PlayerCharacter& SetRotation( Double rotation );

        void StartMove();
        void StopMove();

        const Vector& GetForward() const;
        Vector GetFinalSpeed() const;
        PlayerCharacter& SetForward( const Vector& forward );
        void Update( Double deltaTime );
        void TurnOnColliderFillter( const PlayerCharacter& other );
        void TurnOffColliderFillter( const PlayerCharacter& other );
        bool GetColliderFillter( const PlayerCharacter& other ) const;
    };
};
