//=================================================================================================
// @file Vector.h
//
// @brief 3차원 위치 좌표를 갖는 클래스입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "Define/DataTypes.h"
#include <type_traits>
#include <chrono>
#include <cmath>

using namespace std::literals::chrono_literals;


namespace Game
{
    struct Timer
    {
        using SystemClock = std::chrono::system_clock;
        using TimePoint = std::chrono::time_point<
            std::chrono::system_clock, std::chrono::duration< double, std::ratio< 1, 10000000 > > >;
        using Duration = std::chrono::duration< std::chrono::milliseconds >;
        TimePoint point;


        Timer& SetNow()
        {
            point = SystemClock::now();
            return *this;
        }


        template < typename DurationType >
        Timer& Add( const DurationType& duration )
        {
            point = point + duration;
            return *this;
        }


        template < typename DurationType >
        bool IsOver( const DurationType& duration ) const
        {
            auto newTime = point + duration;
            return newTime < Now().point;
        }


        bool IsOverNow() const 
        {
            return point < Now().point;
        }


        static Timer Now()
        {
            Timer inst;
            inst.SetNow();
            return inst;
        }
    };
};
