//=================================================================================================
// @file Session.cpp
//
// @brief �÷��̾�� ������ �����ϱ� ���� ���� Ŭ�����Դϴ�.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#include "Network/Session.h"
#include "Network/UtillFuntions.h"
#include "Network/Server.h"
#include "Define/PacketDefine.h"
#include "Game/PlayerController.h"
#include <WinSock2.h>
#include <iostream>
#include <array>

Network::Session::Session( SocketHandle socket, class Server* server )
	: socket( socket ), port( 0 ), server(server)
{
	readBuffer.resize( 1024 );
	sendBuffer.resize( 1024 );
}

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
		//�޸� �����
		memcpy_s( sendBuffer.data(), sendBytes - sentBytes, sendBuffer.data() + sentBytes, sendBytes - sentBytes );
		sendBytes -= sentBytes;
	}
}

void Network::Session::ProcessReceive()
{
	// �̸� ���ļ� ���ú��� ���� �ۼ�
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
			if(headerPtr->Size < recvBytes) // ���� �����Ͱ� �������� ������
			{
				break;
			}
			else // �����ϸ�
			{
				//��Ŷ ó��
				if(contoller) contoller->OnReceivedPacket( headerPtr );
				else OnReceivedPacketInWaitting( headerPtr );

				//Ŀ�� �̵�
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

void Network::Session::OnReceivedPacketInWaitting( const Packet::Header* data )
{
	if(data->Type == Packet::EType::ClientRequestFindMatch)
	{
		LogInput("Request Match Find Recv\n");
		RequestMatch req;
		req.requester = this;
		if(server) server->AddRequest(req);
	}
}

void Network::Session::SetRoom( Game::Room* room )
{
	this->room = room;
	LogInput("Enter Room");
}

void Network::Session::SetController( Game::PlayerController* contoller )
{
	this->contoller = contoller;
}

void Network::Session::LogInput( const Char* input ) const
{
	printf_s( "%s:%d / %s : %s", addressText.c_str(), port, id.c_str(), input );
}
