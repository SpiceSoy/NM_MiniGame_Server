//=================================================================================================
// @file Room.cpp
//
// @brief 미니게임 룸 하나를 시뮬레이션 하기 위한 클래스입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#include "Game/Room.h"
#include "Define/MapData.h"
#include "Define/PacketDefine.h"
#include <cstdarg>
#include <iostream>


static const char* to_string( Game::ERoomState e )
{
    using namespace Game;
    switch ( e )
    {
        case ERoomState::Opened :
            return "Opened";
        case ERoomState::Waited :
            return "Waited";
        case ERoomState::Doing :
            return "Doing";
        case ERoomState::End :
            return "End";
        case ERoomState::Closed :
            return "Closed";
        default :
            return "unknown";
    }
}


Game::Room::Room( Int32 userCount )
    : maxUserCount( userCount ), state( ERoomState::Opened )
{
    players.resize( userCount );
    characters.resize( userCount );
}


Game::Room::~Room()
{
}


Game::PlayerController* Game::Room::GetNewPlayerController( Int32 index, Network::Session* session )
{
    auto& instance = players[ index ];
    auto& character = characters[ index ];
    instance.SetSession( session );
    instance.SetCharacter( &character );
    instance.SetRoom( this );
    instance.SetPlayerIndex( index );
    character.SetLocation( GetSpawnLocation( index ) );
    character.SetForward( GetSpawnForward( index ) );

    return &instance;
}


void Game::Room::Update( Double deltaTime )
{
    for ( PlayerController& player : players )
    {
        player.Update( deltaTime );
    }
    CheckCollision( deltaTime );
    for ( Int32 i = 0; i < maxUserCount; i++ )
    {
        PlayerCharacter& character = characters[ i ];
        PlayerController& controller = players[ i ];
        character.Update( deltaTime );
        controller.BroadcastObjectLocation( false );
    }
    // 시작했는가?
    if ( state == ERoomState::Waited && startTime.IsOverNow() )
    {
        SetState( ERoomState::Doing );
        LogLine( "Start of Game" );
        BroadcastStartGame();
    }
    // 끝났는가?
    if ( state == ERoomState::Doing && startTime.IsOver( Constant::GameLengthSeconds ) )
    {
        SetState( ERoomState::End );
        LogLine( "End of Game" );
        BroadcastEndGame();
    }
}


void Game::Room::ReadyToGame()
{
    Packet::Server::StartMatch packet;
    packet.userCount = maxUserCount;
    for ( Int32 i = 0; i < maxUserCount; i++ )
    {
        packet.playerIndex = i; // 임시
        players[ i ].SendPacket( &packet );
    }
    for ( Int32 i = 0; i < maxUserCount; i++ )
    {
        players[ i ].BroadcastObjectLocation( true );
        players[ i ].Initialize();
    }
    SetState( ERoomState::Waited );
    LogLine( "Ready of Game" );
    startTime.SetNow().Add( Constant::FirstSpawnWaitSeconds );
}


void Game::Room::BroadcastByte( const Byte* data, UInt32 size )
{
    BroadcastByteInternal( data, size, nullptr );
}


void Game::Room::BroadcastByte( const Byte* data, UInt32 size, Int32 expectedUserIndex )
{
    BroadcastByteInternal( data, size, &players[ expectedUserIndex ] );
}


void Game::Room::CheckCollisionTwoPlayer( PlayerCharacter& firstChr, PlayerCharacter& secondChr )
{
    bool isCollide = IsCollide( firstChr, secondChr );
    bool isLastCollided = firstChr.GetColliderFillter( secondChr ) || secondChr.GetColliderFillter( firstChr );
    if ( isCollide && !isLastCollided )
    {
        firstChr.TurnOnColliderFillter( secondChr );
        secondChr.TurnOnColliderFillter( firstChr );

        //std::cout << "Collide!" << std::endl;

        Vector normal = firstChr.GetLocation() - secondChr.GetLocation();
        normal.Normalize();

        Vector firstForward = firstChr.GetForward();
        Vector firstReflected = Vector::Reflect( normal, firstForward ).Normalized();
        Vector firstFinalSpeed = firstChr.GetFinalSpeed();

        Vector secondForward = secondChr.GetForward();
        Vector secondReflected = Vector::Reflect( -normal, secondForward ).Normalized();
        Vector secondFinalSpeed = firstChr.GetFinalSpeed();

        firstChr.SetSpeed( firstReflected * secondFinalSpeed.GetLength() * Constant::CollideForceRatio );
        secondChr.SetSpeed( secondReflected * firstFinalSpeed.GetLength() * Constant::CollideForceRatio );

        while ( IsCollide( firstChr, secondChr ) )
        {
            firstChr.SetLocation( firstChr.GetLocation() + firstReflected * 0.1f );
            secondChr.SetLocation( secondChr.GetLocation() + secondReflected * 0.1f );
        }
    }
    else
    {
        firstChr.TurnOffColliderFillter( secondChr );
        secondChr.TurnOffColliderFillter( firstChr );
    }
}


void Game::Room::CheckCollision( Double deltaTime )
{
    //std::cout << "CollideFrame" << std::endl;
    for ( Int32 first = 0; first < maxUserCount; first++ )
    {
        PlayerCharacter& firstChr = characters[ first ];
        PlayerController& firstCon = players[ first ];
        for ( Int32 second = first + 1; second < maxUserCount; second++ )
        {
            PlayerCharacter& secondChr = characters[ second ];
            CheckCollisionTwoPlayer( firstChr, secondChr );
        }
        bool isOutOfMap = firstChr.GetLocation().GetLength() > Constant::MapSize;
        if ( isOutOfMap && firstCon.GetState() != EPlayerState::Die )
            firstCon.ChangeState( EPlayerState::Die );
    }
}


bool Game::Room::IsCollide( PlayerCharacter& firstChr, PlayerCharacter& secondChr )
{
    Double dist = Vector::Distance( firstChr.GetLocation(), secondChr.GetLocation() );
    Double sumRadius = firstChr.GetRadius() + secondChr.GetRadius();
    return sumRadius > dist;
}


void Game::Room::BroadcastByteInternal( const Byte* data, UInt32 size, PlayerController* expectedUser )
{
    for ( PlayerController& player : players )
    {
        if ( &player == expectedUser )
            continue;
        player.SendByte( data, size );
    }
}


void Game::Room::BroadcastStartGame()
{
    Packet::Server::StartGame packet;
    packet.GameTime = std::chrono::duration_cast< std::chrono::seconds >( Constant::GameLengthSeconds ).count();
    BroadcastPacket( &packet );
}


void Game::Room::BroadcastEndGame()
{
    Packet::Server::EndGame packet;
    BroadcastPacket( &packet );
}


void Game::Room::LogLine( const char* format, ... ) const
{
    time_t c;
    time( &c );
    tm t;
    localtime_s( &t, &c );
    printf( "[%02d:%02d:%02d] ROOM[%s] : ", t.tm_hour, t.tm_min, t.tm_sec, to_string( GetState() ) );
    va_list va;
    va_start( va, format );
    vprintf_s( format, va );
    va_end( va );
    printf( "\n" );
}


Game::Vector Game::Room::GetSpawnLocation( UInt32 index ) const
{
    Double angle = 360.0 * ( static_cast< Double >( index + 1 ) / static_cast< Double >( maxUserCount ) );
    Double spawnLength = Constant::SpawnPointRatio * Constant::MapSize;
    Vector spawnPoint = Vector( 0.0, -spawnLength, 0.0 ).Rotated2D( angle );
    return spawnPoint;
}


Game::Vector Game::Room::GetSpawnForward( UInt32 index ) const
{
    Vector start = GetSpawnLocation( index );
    Vector end = Vector( 0 );
    return ( end - start ).Normalized();
}


Game::ERoomState Game::Room::GetState() const
{
    return state;
}


void Game::Room::SetState( ERoomState state )
{
    this->state = state;
}
