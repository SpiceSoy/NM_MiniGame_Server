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
#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <iostream>


Game::Room::Room( Int32 userCount )
    : maxUserCount( userCount ), state( ERoomState::Opened )
{
    players.resize( userCount );
    characters.resize( userCount );
    sessions.resize( userCount );
    scores.resize( userCount );
    currentMapSize = Constant::MapSize;
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


void Game::Room::SpawnItem()
{
    if ( itemSpawnedTime.IsOverNow() )
    {
        LogLine( "Item Spawn Check" );
        if ( items.size() < Constant::ItemSameTimeMaxSpawnCount )
        {
            LogLine( "Item Spawned" );
            Vector location = GetRandomItemLocation();
            std::vector< EItemType > itemPool = { EItemType::Fortify, EItemType::Ghost, EItemType::StrongWill, EItemType::SwiftMove, EItemType::Clover };
            Int32 itemMax = this->startTime.IsOverSeconds( Constant::ItemCloverSpawnStartTime ) ? itemPool.size() : itemPool.size() - 1;
            Int32 itemType = rand() % itemMax;
            items.emplace_back( itemIndex, location, itemPool[itemType] );
            BroadcastSpawnItem( items.back() );
            itemIndex++;
        }
        Int32 maxDelta = static_cast< int >( round( Constant::ItemRegenMaxSeconds - Constant::ItemRegenMinSeconds ) );
        Double randomSecond = Constant::ItemRegenMinSeconds + rand() % maxDelta;
        itemSpawnedTime.AddSeconds( randomSecond );
    }
}


Game::Vector Game::Room::GetRandomItemLocation() const
{
    Double angle = rand() % 360;
    Double mapSize = rand() % static_cast< Int32 >( floor( currentMapSize ) * 0.9 );
    return Vector( 0.0, mapSize, 0.0 ).Rotated2D( angle );
}


void Game::Room::CheckCollisionItem()
{
    for ( auto i = items.begin(); i != items.end(); )
    {
        auto& item = *i;
        bool itemErased = false;
        if ( item.IsExpired() )
        {
            //remove by expire
            BroadcastRemoveItem( item, false );
            i = items.erase( i );
            itemErased = true;
        }
        else
        {
            for ( Int32 characterIndex = 0; characterIndex < maxUserCount; characterIndex++ )
            {
                PlayerCharacter& character = characters[ characterIndex ];
                PlayerController& controller = players[ characterIndex ];

                bool isCollide = IsCollide( character, item );

                if ( isCollide )
                {
                    LogLine( "Item Collided" );
                    //remove by get
                    BroadcastRemoveItem( item, true );
                    controller.ApplyBuff( item.GetType() );
                    i = items.erase( i );
                    itemErased = true;
                    break;
                }
            }
        }
        if( !itemErased ) ++i;
    }
}


void Game::Room::UpdateItem( Double deltaTime )
{
    SpawnItem();
    CheckCollisionItem();
}


void Game::Room::UpdateMap()
{
    if( mapPhase == 0 && startTime.IsOverSeconds( Constant::MapFirstDisableSeconds ) )
    {
        LogLine( "MapPhase Changed : %d" , mapPhase );
        BroadcastMapSizeChanged(mapPhase);
        currentMapSize = Constant::MapFirstDisableSize;
        mapPhase = 1;
    }
    else if( mapPhase == 1 && startTime.IsOverSeconds( Constant::MapSecondDisableSeconds ))
    {
        LogLine( "MapPhase Changed : %d", mapPhase );
        BroadcastMapSizeChanged( mapPhase );
        currentMapSize = Constant::MapSecondDisableSize;
        mapPhase = 2;
    }
}


void Game::Room::AddSession( Int32 index, Network::Session* session )
{
    sessions[ index ] = session;
    session->SetRoom( this );
    session->SetController( GetNewPlayerController( index, session ) );
}


void Game::Room::UpdatePlayerController( Double deltaTime )
{
    for ( PlayerController& player : players )
    {
        player.Update( deltaTime );
    }
}


void Game::Room::UpdateCharacter( Double deltaTime )
{
    for ( Int32 i = 0; i < maxUserCount; i++ )
    {
        PlayerCharacter& character = characters[ i ];
        PlayerController& controller = players[ i ];
        character.Update( deltaTime );
        controller.BroadcastObjectLocation( false );
    }
}


void Game::Room::CheckStateChange()
{
    // �����ߴ°�?
    if ( state == ERoomState::Waited && startTime.IsOverNow() )
    {
        SetState( ERoomState::Doing );
        LogLine( "Start of Game" );
        BroadcastStartGame();
    }
    // �����°�?
    if ( state == ERoomState::Doing && startTime.IsOverSeconds( Constant::GameTotalTimeSeconds ) )
    {
        Int32 maxScore = 0;
        for( Int32 i = 0; i < maxUserCount; ++i ) maxScore = std::max( maxScore, scores[i] );
        for ( Int32 i = 0; i < maxUserCount; ++i )
        {
            players[i].ChangeState( maxScore == scores[i] ? EPlayerState::Win : EPlayerState::Lose );
        }
        SetState( ERoomState::End );
        LogLine( "End of Game" );
        BroadcastEndGame();
        for ( Network::Session* i : sessions )
        {
            i->ClearRoomData();
        }
    }
}


void Game::Room::Update( Double deltaTime )
{
    if ( state == ERoomState::End ) return;
    UpdateMap();
    UpdatePlayerController( deltaTime );
    UpdateCharacter( deltaTime );
    CheckCollision( deltaTime );
    UpdateItem( deltaTime );
    CheckStateChange();
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
    startTime.SetNow().AddSeconds( Constant::GameFirstWaitSeconds );
    itemSpawnedTime.SetNow().AddSeconds( Constant::GameFirstWaitSeconds );
}


void Game::Room::BroadcastByte( const Byte* data, UInt32 size )
{
    BroadcastByteInternal( data, size, nullptr );
}


void Game::Room::BroadcastByte( const Byte* data, UInt32 size, Int32 expectedUserIndex )
{
    BroadcastByteInternal( data, size, &players[ expectedUserIndex ] );
}


bool Game::Room::CheckCollisionTwoPlayer( PlayerCharacter& firstChr, Game::PlayerController& firstCon, Game::PlayerCharacter& secondChr, Game::PlayerController& secondCon, Double deltaTime )
{
    Double penetration = 0.0;
    bool isCollide = IsCollide( firstChr, secondChr, penetration );
    bool isLastCollided = firstChr.GetColliderFillter( firstChr ) || secondChr.GetColliderFillter( firstChr );
    //if( isCollide )
    if ( isCollide && !isLastCollided )
    {
        firstChr.TurnOnColliderFillter( secondChr );
        secondChr.TurnOnColliderFillter( firstChr );

        if( firstCon.GetState( ) == EPlayerState::Spawn )
        {
            ResolveSpawnCollision( firstChr, secondChr, deltaTime, penetration );
        }
        else if( secondCon.GetState( ) == EPlayerState::Spawn )
        {
            ResolveSpawnCollision( secondChr, firstChr, deltaTime, penetration );
        }
        else
        {
            ResolveCollision( firstChr, secondChr, deltaTime, penetration );
        }
        return true;
    }

    firstChr.TurnOffColliderFillter( secondChr );
    secondChr.TurnOffColliderFillter( firstChr );
    return false;
}


void Game::Room::ResolveCollision( PlayerCharacter& firstChr, PlayerCharacter& secondChr, Double deltaTime, Double penetration )
{
    auto& a = firstChr;
    auto& b = secondChr;
    auto rv = a.GetFinalSpeed() - b.GetFinalSpeed();
    auto normal = ( a.GetLocation() - b.GetLocation() ).Normalized();


    firstChr.SetLocation( firstChr.GetLocation() - normal * penetration * 0.4f );
    secondChr.SetLocation( secondChr.GetLocation() + normal * penetration * 0.4f );

    Double velAlongNormal = Vector::Dot( rv, normal );
    if ( velAlongNormal > 0 ) return;

    Double e = Constant::CharacterElasticity; // ź�� ���
    Double j = -( 1 + e ) * velAlongNormal;

    Double AMass = firstChr.GetWeight();
    Double BMass = secondChr.GetWeight();
    j /= 1 / AMass + 1 / BMass;
    auto impulse = normal * j;
    Vector aNewSpeed = ( impulse / AMass );
    a.AddSpeed( aNewSpeed );
    Vector bNewSpeed = -( impulse / BMass );
    b.AddSpeed( bNewSpeed );
    printf( "Collision by A[%lf,%lf,%lf] / B[%lf,%lf,%lf] / penetraion : %lf\n", aNewSpeed.x, aNewSpeed.y, aNewSpeed.z, bNewSpeed.x, bNewSpeed.y, bNewSpeed.z, penetration );
}


void Game::Room::ResolveSpawnCollision( PlayerCharacter& spawnCharacter, PlayerCharacter& other, Double deltaTime, Double penetration )
{
    auto& a = spawnCharacter;
    auto& b = other;
    auto normal = ( a.GetLocation( ) - b.GetLocation( ) ).Normalized( );

    other.SetLocation( other.GetLocation( ) + normal * penetration );
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
            bool isCollide = CheckCollisionTwoPlayer( 
                firstChr, firstCon,
                secondChr, secondCon,
                deltaTime );
            if ( isCollide )
            {
                firstCon.OnCollided( secondCon );
                secondCon.OnCollided( firstCon );
            }
        }
        bool isOutOfMap = firstChr.GetLocation().GetLength() > currentMapSize;
        if ( isOutOfMap && firstCon.GetState() != EPlayerState::Die )
        {
            firstCon.ChangeState( EPlayerState::Die );
            OnDiePlayer( &firstCon );
        }
    }
}


bool Game::Room::IsCollide( const PlayerCharacter& firstChr, const PlayerCharacter& secondChr, Double& resultPenetration )
{
    Double dist = Vector::Distance( firstChr.GetLocation(), secondChr.GetLocation() );
    Double sumRadius = firstChr.GetRadius() + secondChr.GetRadius();
    resultPenetration = sumRadius - dist;
    return sumRadius > dist;
}


bool Game::Room::IsCollide( const PlayerCharacter& character, const Item& item )
{
    Double dist = Vector::Distance( character.GetLocation(), item.GetLocation() );
    Double sumRadius = character.GetRadius() + item.GetRadius();
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
    packet.GameTime = static_cast< Int32 >( Constant::GameTotalTimeSeconds );
    BroadcastPacket( &packet );
}


void Game::Room::BroadcastEndGame()
{
    Packet::Server::EndGame packet;
    packet.maxPlayer = maxUserCount;
    std::memset( packet.scores, 0, sizeof( packet.scores ) );
    std::memcpy( packet.scores, scores.data(), scores.size() * sizeof( scores[ 0 ] ) );
    BroadcastPacket( &packet );
}


void Game::Room::BroadcastMapSizeChanged( Int32 mapIndex )
{
    Packet::Server::MapSizeChanged packet;
    packet.mapIndex = mapIndex;
    BroadcastPacket( &packet );
}


void Game::Room::BroadcastSpawnItem( const Item& item )
{
    Packet::Server::ItemSpawn packet;
    packet.itemIndex = item.GetIndex();
    packet.itemType = static_cast< Byte >( item.GetType() );
    Vector location = item.GetLocation();;
    packet.locationX = location.x;
    packet.locationY = location.y;
    packet.locationZ = location.z;
    BroadcastPacket( &packet );
}


void Game::Room::BroadcastRemoveItem( const Item& item, bool isEaten )
{
    Packet::Server::ItemRemove packet;
    packet.itemIndex = item.GetIndex();
    packet.isEaten = isEaten;
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
    Double currentSize = currentMapSize;
    if( startTime.IsOverSeconds( Constant::MapFirstDisableSeconds - 5 ) ) currentSize = Constant::MapFirstDisableSize;

    if( startTime.IsOverSeconds( Constant::MapSecondDisableSeconds - 5 ) ) currentSize = Constant::MapSecondDisableSize;

    Double angle = 360.0 * ( static_cast< Double >( index + 1 ) / static_cast< Double >( maxUserCount ) );
    Double spawnLength = Constant::MapSpawnPointRatio * currentSize;
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


void Game::Room::CheckNewKing()
{
    Int32 maxScore = -1;
    std::set< Int32 > maxUsers;

    for ( Int32 i = 0; i < maxUserCount; ++i )
    {
        if ( maxScore < scores[ i ] )
        {
            maxUsers.clear();
            maxScore = scores[ i ];
            maxUsers.insert( i );
        }
        else if ( maxScore == scores[ i ] )
        {
            maxUsers.insert( i );
        }
    }

    for ( Int32 i = 0; i < maxUserCount; ++i )
    {
        if ( prevMaxUsers.count( i ) )
        {
            if ( !maxUsers.count( i ) )
            {
                players[ i ].RemoveKing();
            }
        }
        else
        {
            if ( maxUsers.count( i ) )
            {
                players[ i ].ApplyKing();
            }
        }
    }
    prevMaxUsers = std::move( maxUsers );
}


void Game::Room::OnDiePlayer( const PlayerController* player )
{
    Int32 playerIndex = player->GetPlayerIndex();
    Int32 killerIndex = player->GetLastCollidedPlayerIndex();
    bool hasKiller = killerIndex != Constant::NullPlayerIndex;
    Int32 PlayerScore = hasKiller ? Constant::ScoreDiePlayer : Constant::ScoreSelfDiePlayer;
    scores[ playerIndex ] += PlayerScore;

    //Update King

    CheckNewKing();


    if ( hasKiller )
    {
        scores[ killerIndex ] += Constant::ScoreKillPlayer;
        LogLine( "P[%d] Die By P[%d]", playerIndex, killerIndex );
    }
    else
    {
        LogLine( "P[%d] Die By Self", playerIndex );
    }

    BroadcastKillLogPacket( playerIndex, killerIndex );
}
