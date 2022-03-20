//=================================================================================================
// @file Room.cpp
//
// @brief �̴ϰ��� �� �ϳ��� �ùķ��̼� �ϱ� ���� Ŭ�����Դϴ�.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#include "Game/Room.h"
#include "Define/MapData.h"
#include "Define/PacketDefine.h"
#include "Network/Session.h"
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
    sessions.resize( userCount );
    scores.resize( userCount );
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


void Game::Room::AddSession( Int32 index, Network::Session* session)
{
    sessions[index] = session;
    session->SetRoom( this );
    session->SetController( GetNewPlayerController( index, session ) );
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
    // �����ߴ°�?
    if ( state == ERoomState::Waited && startTime.IsOverNow() )
    {
        SetState( ERoomState::Doing );
        LogLine( "Start of Game" );
        BroadcastStartGame();
    }
    // �����°�?
    if ( state == ERoomState::Doing && startTime.IsOver( Constant::TotalGameTime ) )
    {
        SetState( ERoomState::End );
        LogLine( "End of Game" );
        BroadcastEndGame();
        for( Network::Session* i : sessions )
        {
            i->ClearRoomData();
        }
    }
}


void Game::Room::ReadyToGame()
{
    Packet::Server::StartMatch packet;
    packet.userCount = maxUserCount;
    for ( Int32 i = 0; i < maxUserCount; i++ )
    {
        packet.playerIndex = i; // �ӽ�
        players[ i ].SendPacket( &packet );
    }
    for ( Int32 i = 0; i < maxUserCount; i++ )
    {
        players[ i ].BroadcastObjectLocation( true );
        players[ i ].Initialize();
    }
    SetState( ERoomState::Waited );
    LogLine( "Ready of Game" );
    startTime.SetNow().Add( Constant::FirstWaitTime );
}


void Game::Room::BroadcastByte( const Byte* data, UInt32 size )
{
    BroadcastByteInternal( data, size, nullptr );
}


void Game::Room::BroadcastByte( const Byte* data, UInt32 size, Int32 expectedUserIndex )
{
    BroadcastByteInternal( data, size, &players[ expectedUserIndex ] );
}


bool Game::Room::CheckCollisionTwoPlayer( PlayerCharacter& firstChr, PlayerCharacter& secondChr )
{
    bool isCollide = IsCollide( firstChr, secondChr );
    bool isLastCollided = firstChr.GetColliderFillter( secondChr ) || secondChr.GetColliderFillter( firstChr );
    if ( isCollide && !isLastCollided )
    {
        firstChr.TurnOnColliderFillter( secondChr );
        secondChr.TurnOnColliderFillter( firstChr );

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
        return true;
    }

    firstChr.TurnOffColliderFillter( secondChr );
    secondChr.TurnOffColliderFillter( firstChr );
    return false;
}


void Game::Room::CheckCollision( Double deltaTime )
{
    for ( Int32 first = 0; first < maxUserCount; first++ )
    {
        PlayerCharacter& firstChr = characters[ first ];
        PlayerController& firstCon = players[ first ];
        for ( Int32 second = first + 1; second < maxUserCount; second++ )
        {
            PlayerCharacter& secondChr = characters[ second ];
            PlayerController& secondCon = players[ second ];
            bool isCollide = CheckCollisionTwoPlayer( firstChr, secondChr );
            if ( isCollide )
            {
                firstCon.OnCollided( secondCon );
                secondCon.OnCollided( firstCon );
            }
        }
        bool isOutOfMap = firstChr.GetLocation().GetLength() > Constant::MapSize;
        if ( isOutOfMap && firstCon.GetState() != EPlayerState::Die )
        {
            firstCon.ChangeState( EPlayerState::Die );
            OnDiePlayer( &firstCon );
        }
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
        if ( &player == expectedUser ) continue;
        player.SendByte( data, size );
    }
}


void Game::Room::BroadcastStartGame()
{
    Packet::Server::StartGame packet;
    packet.GameTime = std::chrono::duration_cast< std::chrono::seconds >( Constant::TotalGameTime ).count();
    BroadcastPacket( &packet );
}


void Game::Room::BroadcastEndGame()
{
    Packet::Server::EndGame packet;
    std::memset( packet.scores, 0, sizeof( packet.scores ) );
    std::memcpy( packet.scores, scores.data(), scores.size() * sizeof( scores[ 0 ] ) );
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


void Game::Room::BroadcastKillLogPacket( Int32 playerIndex, Int32 killerIndex )
{
    Packet::Server::KillLog packet;
    packet.killerIndex = killerIndex;
    packet.victimIndex = playerIndex;
    BroadcastPacket( &packet );
}


void Game::Room::OnDiePlayer( const PlayerController* player )
{
    Int32 playerIndex = player->GetPlayerIndex();
    Int32 killerIndex = player->GetLastCollidedPlayerIndex();
    bool hasKiller = killerIndex != Constant::NullPlayerIndex;
    Int32 PlayerScore = hasKiller ? Constant::DieScore : Constant::SelfDieScore;
    scores[ playerIndex ] += PlayerScore;

    if ( hasKiller )
    {
        scores[ killerIndex ] += Constant::KillerScore;
        LogLine( "P[%d] Die By P[%d]", playerIndex, killerIndex );
    }
    else
    {
        LogLine( "P[%d] Die By Self", playerIndex );
    }

    BroadcastKillLogPacket( playerIndex, killerIndex );
}
