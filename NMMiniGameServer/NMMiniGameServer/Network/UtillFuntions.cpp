//=================================================================================================
// @file Utill.cpp
//
// @brief 네트워크 프로그래밍 도중 자주 사용하는 유틸 함수들입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#include "Network/UtillFuntions.h"
#include "Define/DataTypes.h"
#include <iomanip>
#include <iostream>
#include <WinSock2.h>


Char* Network::GetErrorMessageString( Int32 err )
{
    Char* msgBuffer = nullptr;
    FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err,
                    MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), ( LPSTR )&msgBuffer, 0, nullptr );
    return msgBuffer;
}


Void Network::PrintErrorMessage( const Char* title, const Char* message, const Char* file, Int32 line )
{
    std::cout << "[Error] " << std::setw( 10 ) << title << " : " << message << "\n" << "File : " << file << ", " <<
            line;
}


Void Network::PrintLastErrorMessage( const Char* title, const Char* file, Int32 line )
{
    int err = WSAGetLastError();
    Char* errMessage = GetErrorMessageString( err );
    PrintErrorMessage( title, errMessage, file, line );
    LocalFree( errMessage );
}
