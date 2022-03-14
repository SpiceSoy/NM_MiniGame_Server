//=================================================================================================
// @file main
//
// @brief program entry point
// 
// @date 2022/02/28
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#include "Network/Server.h"
#include "Game/World.h"
#include <iostream>	
#include <WinSock2.h>


constexpr Int32 DEFAULT_PORT = 4000;


int main( int argc, char* argv[] )
{
	Int32 port = argc == 1 ? DEFAULT_PORT : atoi( argv[1] );
	Network::Server server;
	server.Initialize( static_cast<UInt16>( port ) );
	server.Process();
	return 0;
}