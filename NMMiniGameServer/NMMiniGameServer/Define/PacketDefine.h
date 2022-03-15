//=================================================================================================
// @file PacketDefine.h
//
// @brief Ŭ���̾�Ʈ�� ���� �� ���Ǵ� ��Ŷ�� �����ϴ� ��������Դϴ�. 
//		  ��� ����Ʈ ������ ��Ʈ��ũ ����Ʈ ���ķ� ��Ʈ��ũ�� �۽��ؾ� �մϴ�.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "Define/DataTypes.h"
#define SERVER_HEADER(x) {sizeof(x), (EType::Server##x)}
#define CLIENT_HEADER(x) {sizeof(x), (EType::Client##x)}
#pragma pack(push, 1)


namespace Packet
{
	enum class EType : Byte
	{
		ServerTypeStart = 0,
		ServerStartMatch,
		ServerObjectLocation,
		ClientTypeStart = 0x80,
		ClientRequestFindMatch,
		ClientRequestCancelMatch,
		ClientInput
	};
	enum class EInputState : Byte
	{
		None,
		Click,
		Press,
		Release,
	};

	struct Header
	{
		Byte Size;
		EType Type;
	};

	namespace Server
	{
		struct StartMatch
		{
			Header header = SERVER_HEADER( StartMatch );
			Int32 playerIndex;
			Int32 userCount;
		};
		struct ObjectLocation
		{
			Header header = SERVER_HEADER( ObjectLocation );
			Int32 targetIndex;
			Single locationX;
			Single locationY;
			Single locationZ;

			Single speedX;
			Single speedY;
			Single speedZ;
		};
	};

	namespace Client
	{
		struct RequestFindMatch
		{
			Header header = CLIENT_HEADER( RequestFindMatch );
		};
		struct RequestCancelMatch
		{
			Header header = CLIENT_HEADER( RequestCancelMatch );
		};
		struct Input
		{
			Header header = CLIENT_HEADER( Input );
			EInputState left;
			EInputState right;
			EInputState rush;
		};
	};
};


#pragma pack(pop)