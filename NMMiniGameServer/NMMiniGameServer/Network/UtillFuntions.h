//=================================================================================================
// @file Utill.h
//
// @brief ��Ʈ��ũ ���α׷��� ���� ���� ����ϴ� ��ƿ �Լ����Դϴ�.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#pragma once
#include "Define/DataTypes.h"

#define PrintLastErrorMessageInFile(title)  Network::PrintLastErrorMessage(title, __FILE__, __LINE__)
namespace Network
{
	Char* GetErrorMessageString( Int32 err );
	Void PrintErrorMessage( const Char* title, const Char* message, const Char* file, Int32 line );
	Void PrintLastErrorMessage( const Char* title, const Char* file, Int32 line );
}