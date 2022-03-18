//=================================================================================================
// @file MapData.h
//
// @brief 클라이언트 맵 크기 정의를 위한 헤더입니다.
// 
// @date 2022/03/15
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "Define/DataTypes.h"
#include <chrono>

using namespace std::chrono_literals;
using TimeSecond = std::chrono::duration<double>;

namespace Constant
{
	// Server
	constexpr UInt32 TickTerm = 1000 / 60;
	constexpr UInt32 MaxUserCount = 2;
	// Map
	constexpr Double MapSize = 1300;
	constexpr Double SpawnPointRatio = 0.75;

	// Character
	constexpr Double CharacterRadius = 100.0;
	constexpr Double CharacterWeight = 10.0f;
	constexpr Double CharacterSpring = 1.0f;
	constexpr Double CharacterDefaultSpeed = 600.0;
	constexpr Double CharacterRotateSpeed = 360.0;
	constexpr Double CharacterRushSpeed = 1000.0;
	constexpr Double CharacterFriction = 1000.0f;
	constexpr Double CharacterMapOutSpeed = 300.0f;
	constexpr Double DefaultHeight = -84.787506;
	constexpr Double RespawnHeight = -84.787506;
	constexpr Double CollideForceRatio = 2.0;

	// Game
	constexpr Int32 MaxRushCount = 3;
	constexpr TimeSecond RushRegenSeconds = 7.0s;
	constexpr TimeSecond RespawnSeconds = 1.5s;
	constexpr TimeSecond FirstSpawnWaitSeconds = 1.5s;
	constexpr TimeSecond GameLengthSeconds = 60s;
	constexpr TimeSecond RushRecastTime = 1s;


};


