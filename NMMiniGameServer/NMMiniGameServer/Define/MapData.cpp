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
    Int32 TickTerm = 1000 / 60; // 게임 틱 사이 시간, 쓰지 마십시오
    Int32 MaxUserCount = 3; // 전체 유저

    Double GameFirstWaitSeconds = 1.5; // 게임 최초 대기 시간 (매칭 <-> 조작 가능)
    Double GameTotalTimeSeconds = 90; // 게임 전체 시간

    // Map
    Double MapSize = 1350; // 맵 사이즈 / 현 기준 ( 언리얼 맵 1300 + 연출을 위한 50 )
    Double MapSpawnPointRatio = 0.75; // 0~1 맵 사이즈 기준 최초 스폰 위치 / 1일때 1300에 소환 
    Double MapCharacterDefaultHeight = -84.787506; // 일반적으로 캐릭터 돌아다니는 Z(높이)값
    Double MapSpawnRespawnHeight = -84.787506; // 캐릭터 스폰 시 설정할 Z(높이) 값

    Double MapFirstDisableSeconds = 30;
    Double MapFirstDisableSize = 950;
    Double MapSecondDisableSeconds = 60;
    Double MapSecondDisableSize = 600;

    Double ItemRadius = 50;
    Double ItemRegenMinSeconds = 5;
    Double ItemRegenMaxSeconds = 10;
    Double ItemSpawnLocationMapSizeRatio = 1.0;
    Double ItemCloverSpawnStartTime = 60;
    Double ItemLifeMaxSeconds = 10;
    Double ItemSameTimeMaxSpawnCount = 2;

    //실제 아이템 효과 및 지속시간
    Double ItemFortifyWeight = 3.0; // 포티 파이 무게 / (비례, 배수) 아님
    Double ItemFortifyDurationSeconds = 5.0; // 포티 파이 지속시간

    Double ItemSwiftMoveSpeed = 800; // 스위프트무브 이동속도 / (비례, 배수) 아님
    Double ItemSwiftMoveDurationSeconds = 5; // 스위프트 무브 지속시간

    Double ItemStrongWillRecastSeconds = 0.5;
    Double ItemStrongWillDurationSeconds = 5; // 스트롱 윌 지속시간

    Double ItemGhostDurationSeconds = 3; // 고스트 지속시간

    Double ItemCloverWeight = 3.0; // 클로버 무게 / (비례, 배수) 아님, 포티 파이랑 수치 연동 X
    Double ItemCloverSpeed = 800; // 클로버 이동속도 / (비례, 배수) 아님, 스위프트 무브랑 수치 연동 X
    Double ItemCloverDurationSeconds = 3; // 클로버 지속 시간
    Double ItemCloverGenerationSeconds = 60; // 클로버 등장 시간

    Double CharacterKingRadius = 150.0 * 1.5; // 캐릭터 충돌 판정 크기

    // Character
    Double CharacterRadius = 150.0; // 캐릭터 충돌 판정 크기
    Double CharacterWeight = 1.0; // 캐릭터 무게
    Double CharacterInfiniteWeight = 10000.0; // 스폰, 무적 시 적용할 무게
    Double CharacterDefaultSpeed = 600.0; // 캐릭터 기본 이동 속도
    Double CharacterRotateSpeed = 360.0; // 캐릭터 기본 회전 속도 (각 / 라디안 아님)
    Double CharacterRushSpeed = 1000.0; // 러쉬 누를 때 이동할 속도
    Double CharacterFriction = 1000.0; // 러쉬, 충돌로 인한 스피드가 감소될 마찰력 (추후 조정 가능성 있음)
    Double CharacterMapOutSpeed = 300.0; // 맵 밖으로 나갈 시 더해줄 속도
    Double CharacterElasticity = 1; // 캐릭터 충돌 시 탄성 계수
    Double CharacterMaxSpeed = 2000.0; // 캐릭터 최대 속도 (아직 미사용)
    Int32 CharacterMaxRushCount = 3; // 캐릭터 러시 스택 최대치
    Double CharacterRushCountRegenSeconds = 7.0; // 스택 리젠 시간 / 현재 사용시 쿨타임 리셋 안됨 -> 추후 개선 예정
    Double CharacterRespawnSeconds = 1.5; // 사망 판정 이후 리스폰까지 걸릴 시간 (사망 판정(맵 밖으로 나감) <->

    // ****Score 점수 판정 무조건 합산****
    Int32 ScoreKillPlayer = 1; // 최종 킬러 판단자
    Int32 ScoreDiePlayer = -1;
    Int32 ScoreSelfDiePlayer = ScoreDiePlayer;
    Double ScoreKillerJudgeTime = 1; // 공격 판정 남는 시간

    Double CharacterRushMinimumRecastSeconds = 1; // 러쉬 사용간 최소 재사용 대기시간
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
    AddToken( ETypeToken::Float, MapSpawnPointRatio ),
    // Character
    AddToken( ETypeToken::Float, CharacterRadius ),
    AddToken( ETypeToken::Float, CharacterWeight ),
    AddToken( ETypeToken::Float, CharacterInfiniteWeight ),
    AddToken( ETypeToken::Float, CharacterDefaultSpeed ),
    AddToken( ETypeToken::Float, CharacterRotateSpeed ),
    AddToken( ETypeToken::Float, CharacterRushSpeed ),
    AddToken( ETypeToken::Float, CharacterFriction ),
    AddToken( ETypeToken::Float, CharacterMapOutSpeed ),
    AddToken( ETypeToken::Float, MapCharacterDefaultHeight ),
    AddToken( ETypeToken::Float, MapSpawnRespawnHeight ),
    AddToken( ETypeToken::Float, CharacterElasticity ),
    AddToken( ETypeToken::Float, CharacterMaxSpeed ),
    // Game
    AddToken( ETypeToken::Digit, CharacterMaxRushCount ),

    //  Score 점수 판정 무조건 합산    
    AddToken( ETypeToken::Digit, ScoreKillPlayer ),
    AddToken( ETypeToken::Digit, ScoreDiePlayer ),
    AddToken( ETypeToken::Digit, ScoreSelfDiePlayer ),
    // Timer
    AddToken( ETypeToken::Float, CharacterRushCountRegenSeconds ),
    AddToken( ETypeToken::Float, CharacterRespawnSeconds ),
    AddToken( ETypeToken::Float, GameFirstWaitSeconds ),
    AddToken( ETypeToken::Float, GameTotalTimeSeconds ),
    AddToken( ETypeToken::Float, CharacterRushMinimumRecastSeconds ),
    AddToken( ETypeToken::Float, ScoreKillerJudgeTime ),
    // 새로 추가
    AddToken( ETypeToken::Float, ItemRadius ),
    AddToken( ETypeToken::Float, ItemRegenMinSeconds ),
    AddToken( ETypeToken::Float, ItemRegenMaxSeconds ),
    AddToken( ETypeToken::Float, ItemSpawnLocationMapSizeRatio ),
    AddToken( ETypeToken::Float, ItemCloverSpawnStartTime ),
    AddToken( ETypeToken::Float, ItemLifeMaxSeconds ),
    AddToken( ETypeToken::Float, ItemSameTimeMaxSpawnCount ),
    // 아이템 관련
    AddToken( ETypeToken::Float, ItemFortifyWeight ),
    AddToken( ETypeToken::Float, ItemFortifyDurationSeconds ),
    AddToken( ETypeToken::Float, ItemSwiftMoveSpeed ),
    AddToken( ETypeToken::Float, ItemSwiftMoveDurationSeconds ),
    AddToken( ETypeToken::Float, ItemStrongWillRecastSeconds ),
    AddToken( ETypeToken::Float, ItemStrongWillDurationSeconds ),
    AddToken( ETypeToken::Float, ItemGhostDurationSeconds ),
    AddToken( ETypeToken::Float, ItemCloverWeight ),
    AddToken( ETypeToken::Float, ItemCloverSpeed ),
    AddToken( ETypeToken::Float, ItemCloverDurationSeconds ),
    AddToken( ETypeToken::Float, CharacterKingRadius), // 캐릭터 충돌 판정 크기
    AddToken( ETypeToken::Float, MapFirstDisableSeconds ),
    AddToken( ETypeToken::Float, MapFirstDisableSize ),
    AddToken( ETypeToken::Float, MapSecondDisableSeconds ),
    AddToken( ETypeToken::Float, MapSecondDisableSize ),

};


void TokenReadValue( const std::string& token, float value )
{
    auto it = variableMaps.find( token );
    if ( it == variableMaps.end() )
    {
        std::cout << "Token[" << token << "] is Unvalidated Token" << std::endl;
        return;
    }
    const pair< ETypeToken, void* >& tokenInfo = it->second;
    ETypeToken tokenType = tokenInfo.first;
    void* tokenPtr = tokenInfo.second;
    switch ( tokenType )
    {
        case ETypeToken::Digit :
            *static_cast< Int32* >( tokenPtr ) = value;
            break;
        case ETypeToken::Float :
            *static_cast< Double* >( tokenPtr ) = value;
            break;
    }
}


float TokenOutValue( const std::string& token )
{
    auto it = variableMaps.find( token );
    if ( it == variableMaps.end() )
    {
        std::cout << "Token[" << token << "] is Unvalidated Token" << std::endl;
    }
    const pair< ETypeToken, void* >& tokenInfo = it->second;
    ETypeToken tokenType = tokenInfo.first;
    void* tokenPtr = tokenInfo.second;
    switch ( tokenType )
    {
        case ETypeToken::Digit :
            return *static_cast< Int32* >( tokenPtr );
        case ETypeToken::Float :
            return *static_cast< Double* >( tokenPtr );
    }
}


bool Constant::LoadMapData( const std::string& mapDir )
{
    ifstream mapFile( mapDir );
    if ( mapFile.fail() )
    {
        std::cout << "맵 파일 경로[" << mapDir << "]를 찾을 수 없습니다. 기본 변수로 서버가 시작됩니다." << std::endl;
        mapFile.close();
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


void Constant::SaveMapData( const std::string& mapDir )
{
    ofstream newMapFile( mapDir );
    for ( auto& i : variableMaps )
    {
        auto token = i.first;
        float value = TokenOutValue( token );
        newMapFile << token << " = " << value << std::endl;
    }
    newMapFile.close();
}
