// =================================================================================================
//  @file Item.h
// 
//  @brief 상세 설명을 입력하시오
//  
//  @date 2022/03/22
// 
//  Copyright 2022 2022 Netmarble Neo, Inc. All Rights Reserved.
// =================================================================================================


#pragma once
#include "Game/ItemType.h"
#include "Game/Timer.h"
#include "Game/Vector.h"


namespace Game
{

    class Item
    {
        Timer spawnTime;
        Vector location;
        Double radius;
        EItemType type;
        Int32 index;
    public:
        Item( Int32 index, Vector location, EItemType type );

        EItemType GetType() const;
        void SetType( EItemType type );

        Vector GetLocation() const;
        void SetLocation( const Vector& location );

        Double GetRadius() const;
        void SetRadius( Double radius );

        bool IsExpired() const;

        Int32 GetIndex() const;
    };
}
    
