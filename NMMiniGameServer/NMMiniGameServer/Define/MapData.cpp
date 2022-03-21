// =================================================================================================
//  @file MapData.cpp
// 
//  @brief 상세 설명을 입력하시오
//  
//  @date 2022/03/22
// 
//  Copyright 2022 2022 Netmarble Neo, Inc. All Rights Reserved.
// =================================================================================================


#include "Define/MapData.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <iterator>


using namespace Constant;
using namespace std;


namespace Constant
{
    // Server
    Int32 TickTerm = 1000 / 60;
    Int32 MaxUserCount = 3;
    // Map
    Double MapSize = 1350;
    Double SpawnPointRatio = 0.75;

    // Character
    //Double CharacterRadius = 100.0;
    Double CharacterRadius = 150.0;
    Double CharacterWeight = 10.0;
    Double CharacterSpring = 1.0;
    Double CharacterDefaultSpeed = 600.0;
    Double CharacterRotateSpeed = 360.0;
    Double CharacterRushSpeed = 1000.0;
    Double CharacterFriction = 1000.0;
    Double CharacterMapOutSpeed = 300.0;
    Double DefaultHeight = -84.787506;
    Double RespawnHeight = -84.787506;
    Double CollideForceRatio = 1;
    Double MaxSpeed = 2000.0;

    // Game
    Int32 MaxRushCount = 3;

    //Score 점수 판정 무조건 합산
    Int32 KillerScore = 1;
    Int32 DieScore = -1;
    Int32 SelfDieScore = DieScore;

    // Timer
    Double RushCountRegenTime = 7.0;
    Double RespawnTime = 1.5;
    Double FirstWaitTime = 1.5;
    Double TotalGameTime = 90;
    Double RushMinimumRecastTime = 1;
    Double KillerJudgeTime = 1; // 공격 판정 남는 시간
}


enum class ETypeToken
{
    Digit,
    Float,
};


#define AddToken(type, x) { #x, {type, static_cast< void* >( &Constant::x )} }


map< std::string, std::pair< ETypeToken, void* > > variableMaps = {
    // Server
    AddToken( ETypeToken::Digit, MaxUserCount ),
    // Map
    AddToken( ETypeToken::Float, MapSize ),
    AddToken( ETypeToken::Float, SpawnPointRatio ),
    // Character
    AddToken( ETypeToken::Float, CharacterRadius ),
    AddToken( ETypeToken::Float, CharacterWeight ),
    AddToken( ETypeToken::Float, CharacterSpring ),
    AddToken( ETypeToken::Float, CharacterDefaultSpeed ),
    AddToken( ETypeToken::Float, CharacterRotateSpeed ),
    AddToken( ETypeToken::Float, CharacterRushSpeed ),
    AddToken( ETypeToken::Float, CharacterFriction ),
    AddToken( ETypeToken::Float, CharacterMapOutSpeed ),
    AddToken( ETypeToken::Float, DefaultHeight ),
    AddToken( ETypeToken::Float, RespawnHeight ),
    AddToken( ETypeToken::Float, CollideForceRatio ),
    AddToken( ETypeToken::Float, MaxSpeed ),
    // Game
    AddToken( ETypeToken::Digit, MaxRushCount ),
    //  Score 점수 판정 무조건 합산    
    AddToken( ETypeToken::Digit, KillerScore ),
    AddToken( ETypeToken::Digit, DieScore ),
    AddToken( ETypeToken::Digit, SelfDieScore ),
    // Timer
    AddToken( ETypeToken::Float, RushCountRegenTime ),
    AddToken( ETypeToken::Float, RespawnTime ),
    AddToken( ETypeToken::Float, FirstWaitTime ),
    AddToken( ETypeToken::Float, TotalGameTime ),
    AddToken( ETypeToken::Float, RushMinimumRecastTime ),
    AddToken( ETypeToken::Float, KillerJudgeTime ),
};

void TokenReadValue( const std::string& token, float value )
{
    auto it = variableMaps.find( token );
    if( it == variableMaps.end( ) )
    {
        std::cout << "Token[" << token << "] is Unvalidated Token" << std::endl;
        return;
    }
    const pair< ETypeToken, void* >& tokenInfo = it->second;
    ETypeToken tokenType = tokenInfo.first;
    void* tokenPtr = tokenInfo.second;
    switch ( tokenType )
    {
        case ETypeToken::Digit:
            *static_cast<Int32*>(tokenPtr) = value;
            break;
        case ETypeToken::Float:
            *static_cast<Double*>( tokenPtr ) = value;
            break;
    }
}

float TokenOutValue( const std::string& token )
{
    auto it = variableMaps.find( token );
    if( it == variableMaps.end( ) )
    {
        std::cout << "Token[" << token << "] is Unvalidated Token" << std::endl;
    }
    const pair< ETypeToken, void* >& tokenInfo = it->second;
    ETypeToken tokenType = tokenInfo.first;
    void* tokenPtr = tokenInfo.second;
    switch( tokenType )
    {
    case ETypeToken::Digit:
        return *static_cast<Int32*>( tokenPtr );
    case ETypeToken::Float:
        return *static_cast<Double*>( tokenPtr );
    }
}

bool Constant::LoadMapData( const std::string& mapDir )
{
    ifstream mapFile( mapDir );
    if ( mapFile.fail() )
    {
        std::cout << "맵 파일 경로[" << mapDir << "]를 찾을 수 없습니다. 기본 변수로 서버가 시작됩니다." << std::endl;

        ofstream newMapFile(mapDir);
        for( auto& i : variableMaps )
        {
            auto token = i.first;
            float value = TokenOutValue(token);
            newMapFile << token << " = " << value << std::endl;
        }
        mapFile.close();
        newMapFile.close();
        return false;
    }
    //string fileString = { istreambuf_iterator<char>( mapFile ), istreambuf_iterator<char>( ) };
    stringstream ss;
    ss.set_rdbuf( mapFile.rdbuf() );

    char buffer[ 4096 ] = {};
    char token[ 4096 ] = {};
    float value = 0;
    while ( ss.getline( buffer, 4096 ) )
    {
        if ( buffer[ 0 ] == '#' ) continue;
        ::sscanf_s( buffer, "%s = %f", token, sizeof( token ), &value, sizeof( token ) );
        std::cout << "Parsing : " << token << " = " << value << std::endl;
        TokenReadValue( token, value );
    }
}
