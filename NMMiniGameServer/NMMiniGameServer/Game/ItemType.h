// =================================================================================================
//  @file ItemType.h
// 
//  @brief 상세 설명을 입력하시오
//  
//  @date 2022/03/23
// 
//  Copyright 2022 2022 Netmarble Neo, Inc. All Rights Reserved.
// =================================================================================================


#pragma once
#include "Define/DataTypes.h"


namespace Game
{
    enum class EItemType : Byte
    {
        Clover,
        Fortify,
        Ghost,
        StrongWill,
        SwiftMove,
        King,
        None
    };


    inline const char* to_string( EItemType e )
    {
        switch ( e )
        {
            case EItemType::Clover :
                return "Clover";
            case EItemType::Fortify :
                return "Fortify";
            case EItemType::Ghost :
                return "Ghost";
            case EItemType::StrongWill :
                return "StrongWill";
            case EItemType::SwiftMove :
                return "SwiftMove";
            case EItemType::King :
                return "King";
            case EItemType::None :
                return "None";
            default :
                return "unknown";
        }
    }

};