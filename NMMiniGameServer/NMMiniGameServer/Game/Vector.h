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
#include <math.h>


namespace Game
{
	class Vector
	{
		using NumType = Double;
		NumType x = 0.0;
		NumType y = 0.0;
		NumType z = 0.0;

		Vector() {}

		template<typename ScalarTy, bool isValid = std::is_scalar<ScalarTy>::value >
		Vector( ScalarTy value )
			: x( (NumType)value ), y( (NumType)value ), z( (NumType)value )
		{
			static_assert( isValid == true, "ScalarTy is not Scalar" );
		}

		template<typename ScalarTy, bool isValid = std::is_scalar<ScalarTy>::value >
		Vector( ScalarTy x, ScalarTy y, ScalarTy z )
			: x( (NumType)x ), y( (NumType)y ), z( (NumType)z )
		{
			static_assert( isValid == true, "ScalarTy is not Scalar" );
		}

		template<typename ScalarTy, bool isValid = std::is_scalar<ScalarTy>::value >
		Vector operator*( ScalarTy value ) const
		{
			static_assert( isValid == true, "ScalarTy is not Scalar" );
			return Vector( x * value, y * value, z * value );
		}

		template<typename ScalarTy, bool isValid = std::is_scalar<ScalarTy>::value >
		Vector& operator*=( ScalarTy value )
		{
			static_assert( isValid == true, "ScalarTy is not Scalar" );
			*this = *this * value;
			return *this;
		}

		Vector operator+( Vector v ) const
		{
			return Vector( x + v.x, y * v.y, z * v.z);
		}

		Vector& operator+=( Vector v )
		{
			*this = *this + v;
			return *this;
		}

		Vector operator*( Vector v ) const
		{
			return Vector( x * v.x, y * v.y, z * v.z );
		}

		Vector& operator*=( Vector v )
		{
			*this = *this * v;
			return *this;
		}

		Vector operator-( Vector v ) const
		{
			return Vector( x - v.x, y - v.y, z - v.z );
		}

		Vector& operator-=( Vector v )
		{
			*this = *this - v;
			return *this;
		}

		Vector operator/( Vector v ) const
		{
			return Vector( x / v.x, y / v.y, z / v.z );
		}

		Vector& operator/=( Vector v )
		{
			*this = *this / v;
			return *this;
		}

		Vector operator-() const
		{
			return *this * -1.0f;
		}

		NumType GetSqr() const
		{
			return x * x + y * y + z * z;
		}

		NumType GetLength() const
		{
			return sqrt(GetSqr());
		}

		Vector Normalized() const
		{
			return *this / GetLength();
		}

		Vector& Normalize()
		{
			*this = this->Normalized();
			return *this;
		}

		NumType AtanYX() const
		{
			return atan2(y, x);
		}
	};
};