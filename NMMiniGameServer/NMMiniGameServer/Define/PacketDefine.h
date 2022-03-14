//=================================================================================================
// @file PacketDefine.h
//
// @brief 클라이언트와 서버 간 사용되는 패킷을 정의하는 헤더파일입니다. 
//		  모든 바이트 정렬은 네트워크 바이트 정렬로 네트워크에 송신해야 합니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "Define/DataTypes.h"
#define SERVER_HEADER(x) {sizeof(x), static_cast<Byte>(EType::Server##x)}
#define CLIENT_HEADER(x) {sizeof(x), static_cast<Byte>(EType::Client##x)}
#pragma pack(push, 1)


namespace Packet
{
	enum class EType : Byte
	{
		ServerTypeStart = 0,
		ServerObjectLocation,
		ClientTypeStart = 0x80,
		ClientInput
	};

	struct Header
	{
		Byte Size;
		Byte Type;
	};

	namespace Server
	{
		struct ObjectLocation
		{
			Header header = SERVER_HEADER( ObjectLocation );
		};
	};

	namespace Client
	{
		struct Input
		{
			Header header = CLIENT_HEADER( Input );
		};
	};
};


#pragma pack(pop)