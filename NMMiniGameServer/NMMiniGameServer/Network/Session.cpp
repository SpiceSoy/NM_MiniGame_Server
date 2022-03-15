//=================================================================================================
// @file Session.cpp
//
// @brief 플레이어와 연결을 유지하기 위한 세션 클래스입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#include "Network/Session.h"
#include "Network/UtillFuntions.h"
#include "Define/PacketDefine.h"
#include "Game/PlayerController.h"
#include <WinSock2.h>
#include <iostream>
#include <array>

Network::Session::Session( SocketHandle socket )
	: socket( socket ), port( 0 )
{
	readBuffer.resize( 1024 );
	sendBuffer.resize( 1024 );
};

SocketHandle Network::Session::GetSocket() const
{
	return socket;
}

Bool Network::Session::HasSendBytes() const
{
	return sendBytes;
}

void Network::Session::SetState( EState state )
{
	this->state = state;
}

void Network::Session::ProcessSend()
{
	Int32 sentBytes = send( socket, (const char*)sendBuffer.data(), sendBytes, 0 );
	if( sentBytes == SOCKET_ERROR )
	{
		PrintLastErrorMessage( "send", __FILE__, __LINE__ );
		Close();
	}
	else
	{
		//메모리 땡기기
		memcpy_s( sendBuffer.data(), sendBytes - sentBytes, sendBuffer.data() + sentBytes, sendBytes - sentBytes );
		sendBytes -= sentBytes;
	}
}

void Network::Session::ProcessReceive()
{
	// 이름 겹쳐서 로컬변수 전문 작성
	Int32 receivedBytes = recv( socket, ( (char*)readBuffer.data() ) + recvBytes, readBuffer.size() * sizeof( Byte ), 0 );
	Char* expectEnd = reinterpret_cast<Char*>( readBuffer.data() + recvBytes + receivedBytes - 1 );
	if( receivedBytes == 0 || receivedBytes == SOCKET_ERROR )
	{
		Close();
	}
	else
	{
		recvBytes += receivedBytes;

		Byte* dataBegin = readBuffer.data();
		Byte* csr = dataBegin;
		Byte* dataEnd = readBuffer.data() + recvBytes;

		while( csr != dataEnd )
		{
			Packet::Header* headerPtr = reinterpret_cast<Packet::Header*>(csr);
			if(headerPtr->Size < recvBytes) // 받은 데이터가 완전하지 않으면
			{
				break;
			}
			else // 완전하면
			{
				//패킷 처리
				if(player) player->OnReceivedPacket(csr);
				else OnReceivedPacketInWaitting(csr);

				//커서 이동
				csr += headerPtr->Size;
				recvBytes -= headerPtr->Size;
			}

		}
		if(csr != dataBegin && csr != dataEnd)
		{
			Int32 remainDatas = dataEnd - csr;
			memcpy_s(dataBegin, remainDatas, csr, remainDatas );
		}

	}
}

void Network::Session::Close()
{
	if( socket == 0 ) return;
	closesocket( this->socket );
	SetState( EState::Closed );
}

const std::string& Network::Session::GetId() const
{
	return id;
}

const std::string& Network::Session::GetAddress() const
{
	return addressText;
}

UInt16 Network::Session::GetPort() const
{
	return port;
}

Network::Session::EState Network::Session::GetState() const
{
	return state;
}

Bool Network::Session::IsClosed() const
{
	return state == EState::Closed;
}


void Network::Session::SetAddress( const Char* address, UInt16 port )
{
	addressText = address;
	this->port = port;
}

void Network::Session::SendByte( const Byte* data, UInt64 size )
{
	if( IsClosed() ) return;
	bool willOver = sendBytes + size > sendBuffer.size();
	if( willOver ) sendBuffer.resize( sendBuffer.size() * 2 );
	memcpy_s( sendBuffer.data() + sendBytes, size, data, size );
	sendBytes += size;
}

void Network::Session::OnReceivedPacketInWaitting( const Byte* data )
{

}

void Network::Session::LogInput( const Char* input ) const
{
	printf_s( "%s:%d / %s : %s", addressText.c_str(), port, id.c_str(), input );
}
