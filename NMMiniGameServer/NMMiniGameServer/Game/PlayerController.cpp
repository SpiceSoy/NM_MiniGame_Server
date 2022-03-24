//=================================================================================================
// @file PlayerController.h
//
// @brief 미니게임 내에서 플레이어의 게임 입력 처리를 담당할 클래스입니다.
// 
// @date 2022/03/14
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================


#include "Game/PlayerController.h"
#include "Define/DataTypes.h"
#include "Define/MapData.h"
#include "Define/PacketDefine.h"
#include "Game/PlayerCharacter.h"
#include "Game/Room.h"
#include "Network/Session.h"
#include <algorithm>

Int32 Game::PlayerController::GetPlayerIndex() const
{
    return playerIndex;
}


Game::PlayerController::PlayerController()
{
    this->AddStateFunctions();
}


void Game::PlayerController::SetSession( Network::Session* session )
{
    this->session = session;
}


void Game::PlayerController::SetCharacter( PlayerCharacter* character )
{
    this->character = character;
}


void Game::PlayerController::SetRoom( Room* room )
{
    this->room = room;
}


void Game::PlayerController::SetPlayerIndex( Int32 playerIndex )
{
    this->playerIndex = playerIndex;
}


void Game::PlayerController::Initialize()
{
    fsm.Start( EPlayerState::Spawn );
    timerRushUse.SetNow();
    timerRushGen.SetNow();
    rushCount = Constant::CharacterMaxRushCount;
    SendRushCountChangedPacket( );
}


void Game::PlayerController::SendByte( const Byte* data, UInt64 size ) const
{
    if ( !session ) return;
    session->SendByte( data, size );
}


void Game::PlayerController::Update( Double deltaTime )
{
    if ( !character ) return;
    fsm.Update( deltaTime );

    // 러시 개수 체크
    if ( rushCount < Constant::CharacterMaxRushCount )
    {
        if( timerRushGen.IsOverSeconds( Constant::CharacterRushCountRegenSeconds ) )
        {
            rushCount++;
            SendRushCountChangedPacket();
            timerRushGen.SetNow();
        }
    }

    if ( IsItemDurationExpired() )
    {
        RemoveBuff();
    }
}


void Game::PlayerController::OnReceivedPacket( const Packet::Header* ptr )
{
    if ( !ptr ) return;
    switch ( ptr->Type )
    {
        case Packet::EType::ClientInput :
            fsm.OnReceiveInput( *reinterpret_cast< const Packet::Client::Input* >( ptr ) );
            break;
    }
}


Game::EPlayerState Game::PlayerController::GetState() const
{
    return fsm.GetState();
}


void Game::PlayerController::ChangeState( EPlayerState state )
{
    fsm.ChangeState( state );
}


void Game::PlayerController::UseRush()
{
    timerRushUse.SetNow( );
    character->AddSpeed( character->GetForward( ) * Constant::CharacterRushSpeed );
    if( IsUseRushStack() )
    {
        timerRushGen.SetNow( );
        rushCount -= 1;
        SendRushCountChangedPacket();
    }
    else
    {
        LogLine( "Not Use Rush Stack" );
    }
}


bool Game::PlayerController::CanRush()
{
    bool canRecast = timerRushUse.IsOverSeconds( rushRecastTime );
    bool hasRushCount = ( !IsUseRushStack( ) ) || rushCount > 0;
    if ( hasRushCount && canRecast ) return true;
    return false;
}


void Game::PlayerController::SendStateChangedPacket() const
{
    SendStateChangedPacket( this->GetState() );
}


void Game::PlayerController::SendBuffStartPacket() const
{
    Packet::Server::BuffStart packet;
    packet.playerIndex = this->playerIndex;
    packet.buffType = static_cast< Byte >( this->currentItem );
    this->room->BroadcastPacket( &packet );
}


void Game::PlayerController::SendBuffEndPacket() const
{
    Packet::Server::BuffRemove packet;
    packet.playerIndex = this->playerIndex;
    packet.buffType = static_cast< Byte >( this->currentItem );
    this->room->BroadcastPacket( &packet );
}


void Game::PlayerController::SendKingStartPacket() const
{
    Packet::Server::BuffStart packet;
    packet.playerIndex = this->playerIndex;
    packet.buffType = static_cast<Byte>( EItemType::King );
    this->room->BroadcastPacket( &packet );
}


void Game::PlayerController::SendKingEndPacket() const
{
    Packet::Server::BuffRemove packet;
    packet.playerIndex = this->playerIndex;
    packet.buffType = static_cast<Byte>( EItemType::King );
    this->room->BroadcastPacket( &packet );
}


void Game::PlayerController::SendRushCountChangedPacket() const
{
    LogLine( "Rush Changed" );
    Packet::Server::PlayerRushCountChanged packet;
    packet.count = rushCount;
    SendPacket( &packet );
}


void Game::PlayerController::LogLine( const char* format, ... ) const
{
    time_t c;
    time( &c );
    tm t;
    localtime_s( &t, &c );
    printf( "[%02d:%02d:%02d] P%d[%02d:%s] : ", t.tm_hour, t.tm_min, t.tm_sec, playerIndex, static_cast< Int32 >( fsm.GetState() ), to_string( fsm.GetState() ) );
    va_list va;
    va_start( va, format );
    vprintf_s( format, va );
    va_end( va );
    printf( "\n" );
}


bool Game::PlayerController::IsItemDurationExpired()
{
    Double buffDuration = 0.0;
    switch ( currentItem )
    {
        case EItemType::Clover:
            buffDuration = Constant::ItemCloverDurationSeconds;
            break;
        case EItemType::Fortify:
            buffDuration = Constant::ItemFortifyDurationSeconds;
            break;
        case EItemType::Ghost:
            buffDuration = Constant::ItemGhostDurationSeconds;
            break;
        case EItemType::StrongWill:
            buffDuration = Constant::ItemStrongWillDurationSeconds;
            break;
        case EItemType::SwiftMove:
            buffDuration = Constant::ItemSwiftMoveDurationSeconds;
            break;
    }

    return currentItem != EItemType::None ? timerItemChanged.IsOverSeconds( buffDuration ) : false;
}


void Game::PlayerController::BroadcastObjectLocation( bool isSetHeight ) const
{
    Packet::Server::ObjectLocation packet;
    packet.targetIndex = playerIndex;
    packet.chracterState = static_cast< Int32 >( GetState() );
    packet.isSetHeight = isSetHeight;
    Vector location = character->GetLocation();
    packet.locationX = location.x;
    packet.locationY = location.y;
    packet.locationZ = isSetHeight ? Constant::MapSpawnRespawnHeight : Constant::MapCharacterDefaultHeight;
    //packet.rotation = character.GetRotation();
    Vector forward = character->GetForward();
    packet.forwardX = forward.x;
    packet.forwardY = forward.y;
    packet.forwardZ = forward.z;

    room->BroadcastPacket( &packet );
}


void Game::PlayerController::OnCollided( const PlayerController& other )
{
    lastCollidedPlayerIndex = other.GetPlayerIndex();
    timerLastCollided.SetNow();
}


Int32 Game::PlayerController::GetLastCollidedPlayerIndex() const
{
    if ( timerLastCollided.IsOverSeconds( Constant::ScoreKillerJudgeTime ) ) return Constant::NullPlayerIndex;
    else return lastCollidedPlayerIndex;
}


void Game::PlayerController::ApplyBuff( EItemType item )
{
    LogLine( "Apply Buff %s", to_string( item ) );
    currentItem = item;
    timerItemChanged.SetNow();
    SendBuffStartPacket();
    switch( currentItem )
    {
        case EItemType::Clover:
            // 스택 미 소모
            this->character->SetWeight( Constant::ItemCloverWeight );
            this->character->SetMoveSpeed( Constant::ItemCloverSpeed );
            break;
        case EItemType::Fortify:
            this->character->SetWeight( Constant::ItemFortifyWeight );
            break;
        case EItemType::Ghost:
        // 클라에서 처리

            break;
        case EItemType::StrongWill:
        // 스택 미 소모
            rushRecastTime = Constant::ItemStrongWillRecastSeconds;
            break;
        case EItemType::SwiftMove:
            this->character->SetMoveSpeed( Constant::ItemSwiftMoveSpeed );
            break;
        case EItemType::None:
            break;
        default: ;
    }
}


void Game::PlayerController::ApplyKing()
{
    LogLine( "Apply King" );;
    SendKingStartPacket( );
    this->character->SetRadius( Constant::CharacterKingRadius );
}


void Game::PlayerController::RemoveBuff()
{
    if( currentItem == EItemType::None ) return;
    LogLine( "Remove Buff %s", to_string( currentItem ) );
    SendBuffEndPacket( );
    switch ( currentItem )
    {
        case EItemType::Clover:
            // 스택 미 소모
            this->character->SetWeight( Constant::CharacterWeight );
            this->character->SetMoveSpeed( Constant::CharacterDefaultSpeed );
            break;
        case EItemType::Fortify:
            this->character->SetWeight( Constant::CharacterWeight );
            break;
        case EItemType::Ghost:
            // 클라에서 처리
            break;
        case EItemType::StrongWill:
            // 스택 미 소모
            rushRecastTime = Constant::CharacterRushMinimumRecastSeconds;
            break;
        case EItemType::SwiftMove:
            this->character->SetMoveSpeed( Constant::CharacterDefaultSpeed );
            break;
        case EItemType::None:
            break;
        default: ;
    }
    currentItem = EItemType::None;
}


void Game::PlayerController::RemoveKing()
{
    LogLine( "Remove King" );
    this->character->SetRadius( Constant::CharacterRadius );
    SendKingEndPacket();
}


bool Game::PlayerController::IsUseRushStack() const
{
    LogLine( "IsUseRushStakc : %d" , !( currentItem == EItemType::Clover || currentItem == EItemType::StrongWill ) );
    return !( currentItem == EItemType::Clover || currentItem == EItemType::StrongWill );
}


void Game::PlayerController::SendStateChangedPacket( EPlayerState state ) const
{
    Packet::Server::ObjectStateChanged packet;
    packet.targetIndex = this->playerIndex;
    packet.chracterState = static_cast< Byte >( state );
    this->room->BroadcastPacket( &packet );
}


void Game::PlayerController::AddStateFunctions()
{
    auto defaultEnter = [this]( EPlayerState prevState ) -> StateFuncResult< EPlayerState >
    {
        this->SendStateChangedPacket();
        return StateFuncResult< EPlayerState >::NoChange();
    };

    auto defaultUpdate = []( Double deltaTime ) -> StateFuncResult< EPlayerState >
    {
        return StateFuncResult< EPlayerState >::NoChange();
    };
    auto defaultOnInput = []( const Packet::Client::Input& input ) -> StateFuncResult< EPlayerState >
    {
        return StateFuncResult< EPlayerState >::NoChange();
    };
    auto defaultExit = []( EPlayerState nextState )-> void
    {
    };

    #pragma region Spawn
    fsm.AddStateFunctionOnEnter( EPlayerState::Spawn,
                                [this]( EPlayerState prevState ) -> StateFuncResult< EPlayerState >
                                {
                                    //LogLine( "Entered" );
                                    this->SendStateChangedPacket();
                                    this->character->StopMove( );
                                    this->character->SetInfiniteWeight( true );
                                    Double waitTime = prevState == EPlayerState::Die ? Constant::CharacterRespawnSeconds : Constant::GameFirstWaitSeconds;
                                    this->timerSpawnStart.SetNow().AddSeconds( waitTime );
                                    return StateFuncResult< EPlayerState >::NoChange();
                                } );
    fsm.AddStateFunctionOnUpdate( EPlayerState::Spawn,
                                 [this]( Double deltaTime ) -> StateFuncResult< EPlayerState >
                                 {
                                     if ( this->timerSpawnStart.IsOverNow() )
                                     {
                                         this->character->SetInfiniteWeight( false );
                                         return StateFuncResult< EPlayerState >( EPlayerState::Idle );
                                     }
                                     return StateFuncResult< EPlayerState >::NoChange();
                                 } );
    fsm.AddStateFunctionOnReceiveInput( EPlayerState::Spawn, defaultOnInput );
    fsm.AddStateFunctionOnExit( EPlayerState::Spawn, defaultExit );
    #pragma endregion

    #pragma region Idle
    fsm.AddStateFunctionOnEnter( EPlayerState::Idle, defaultEnter );
    fsm.AddStateFunctionOnUpdate( EPlayerState::Idle,
                                 [this]( Double deltaTime ) -> StateFuncResult< EPlayerState >
                                 {
                                     return StateFuncResult< EPlayerState >( EPlayerState::Run );
                                 } );
    fsm.AddStateFunctionOnReceiveInput( EPlayerState::Idle,
                                       []( const Packet::Client::Input& input ) -> StateFuncResult< EPlayerState >
                                       {
                                           if ( input.left == Packet::EInputState::Click )
                                           {
                                               return StateFuncResult< EPlayerState >( EPlayerState::RotateLeft );
                                           }
                                           if ( input.right == Packet::EInputState::Click )
                                           {
                                               return StateFuncResult< EPlayerState >( EPlayerState::RotateRight );
                                           }
                                           if ( input.rush == Packet::EInputState::Click )
                                           {
                                               return StateFuncResult< EPlayerState >( EPlayerState::Rush );
                                           }
                                           return StateFuncResult< EPlayerState >::NoChange();
                                       } );
    fsm.AddStateFunctionOnExit( EPlayerState::Idle, defaultExit );
    #pragma endregion

    #pragma region Run
    fsm.AddStateFunctionOnEnter( EPlayerState::Run,
                                [this]( EPlayerState prevState ) -> StateFuncResult< EPlayerState >
                                {
                                    //LogLine( "Entered" );
                                    this->SendStateChangedPacket();
                                    this->character->StartMove();
                                    return StateFuncResult< EPlayerState >::NoChange();
                                } );
    fsm.AddStateFunctionOnUpdate( EPlayerState::Run, defaultUpdate );
    fsm.AddStateFunctionOnReceiveInput( EPlayerState::Run,
                                       []( const Packet::Client::Input& input ) -> StateFuncResult< EPlayerState >
                                       {
                                           if ( input.left == Packet::EInputState::Click )
                                           {
                                               return StateFuncResult< EPlayerState >( EPlayerState::RotateLeft );
                                           }
                                           if ( input.right == Packet::EInputState::Click )
                                           {
                                               return StateFuncResult< EPlayerState >( EPlayerState::RotateRight );
                                           }
                                           if ( input.rush == Packet::EInputState::Click )
                                           {
                                               return StateFuncResult< EPlayerState >( EPlayerState::Rush );
                                           }
                                           return StateFuncResult< EPlayerState >::NoChange();
                                       } );
    fsm.AddStateFunctionOnExit( EPlayerState::Run, defaultExit );
    #pragma endregion

    #pragma region RotateLeft
    fsm.AddStateFunctionOnEnter( EPlayerState::RotateLeft,
                                [this]( EPlayerState prevState ) -> StateFuncResult< EPlayerState >
                                {
                                    //LogLine( "Entered" );
                                    this->SendStateChangedPacket( EPlayerState::Rotate );
                                    this->character->StopMove( );
                                    return StateFuncResult< EPlayerState >::NoChange();
                                } );
    fsm.AddStateFunctionOnUpdate( EPlayerState::RotateLeft,
                                 [this]( Double deltaTime ) -> StateFuncResult< EPlayerState >
                                 {
                                     character->RotateLeft( Constant::CharacterRotateSpeed * deltaTime );
                                     return StateFuncResult< EPlayerState >::NoChange();
                                 } );
    fsm.AddStateFunctionOnReceiveInput( EPlayerState::RotateLeft,
                                       []( const Packet::Client::Input& input ) -> StateFuncResult< EPlayerState >
                                       {
                                           if ( input.left == Packet::EInputState::Release )
                                           {
                                               return StateFuncResult< EPlayerState >( EPlayerState::Run );
                                           }
                                           if ( input.right == Packet::EInputState::Click )
                                           {
                                               return StateFuncResult< EPlayerState >( EPlayerState::RotateRight );
                                           }
                                           if ( input.rush == Packet::EInputState::Click )
                                           {
                                               return StateFuncResult< EPlayerState >( EPlayerState::Rush );
                                           }
                                           return StateFuncResult< EPlayerState >::NoChange();
                                       } );
    fsm.AddStateFunctionOnExit( EPlayerState::RotateLeft, defaultExit );
    #pragma endregion

    #pragma region RotateRight
    fsm.AddStateFunctionOnEnter( EPlayerState::RotateRight,
                                [this]( EPlayerState prevState ) -> StateFuncResult< EPlayerState >
                                {
                                    //LogLine( "Entered" );
                                    this->SendStateChangedPacket( EPlayerState::Rotate );
                                    this->character->StopMove( );
                                    return StateFuncResult< EPlayerState >::NoChange();
                                } );
    fsm.AddStateFunctionOnUpdate( EPlayerState::RotateRight,
                                 [this]( Double deltaTime ) -> StateFuncResult< EPlayerState >
                                 {
                                     character->RotateRight( Constant::CharacterRotateSpeed * deltaTime );
                                     return StateFuncResult< EPlayerState >::NoChange();
                                 } );
    fsm.AddStateFunctionOnReceiveInput( EPlayerState::RotateRight,
                                       []( const Packet::Client::Input& input ) -> StateFuncResult< EPlayerState >
                                       {
                                           if ( input.left == Packet::EInputState::Click )
                                           {
                                               return StateFuncResult< EPlayerState >( EPlayerState::RotateLeft );
                                           }
                                           if ( input.right == Packet::EInputState::Release )
                                           {
                                               return StateFuncResult< EPlayerState >( EPlayerState::Run );
                                           }
                                           if ( input.rush == Packet::EInputState::Click )
                                           {
                                               return StateFuncResult< EPlayerState >( EPlayerState::Rush );
                                           }
                                           return StateFuncResult< EPlayerState >::NoChange();
                                       } );
    fsm.AddStateFunctionOnExit( EPlayerState::RotateRight, defaultExit );
    #pragma endregion

    #pragma region Rush
    fsm.AddStateFunctionOnEnter( EPlayerState::Rush,
                                [this]( EPlayerState prevState ) -> StateFuncResult< EPlayerState >
                                {
                                    //LogLine( "Rush Try" );
                                    if ( this->CanRush() )
                                    {
                                        //LogLine( "Entered" );
                                        this->SendStateChangedPacket( EPlayerState::Rush );
                                        this->UseRush();
                                        return StateFuncResult< EPlayerState >::NoChange();
                                    }
                                    //LogLine( "Failed" );
                                    return StateFuncResult< EPlayerState >( prevState );
                                    return StateFuncResult< EPlayerState >::NoChange();
                                } );
    fsm.AddStateFunctionOnUpdate( EPlayerState::Rush,
                                 [this]( Double deltaTime ) -> StateFuncResult< EPlayerState >
                                 {
                                     if ( this->character->GetSpeed().GetLength() < 20.0f )
                                     {
                                         return StateFuncResult< EPlayerState >( EPlayerState::Run );
                                     }
                                     return StateFuncResult< EPlayerState >::NoChange();
                                 } );
    fsm.AddStateFunctionOnReceiveInput( EPlayerState::Rush, defaultOnInput );
    fsm.AddStateFunctionOnExit( EPlayerState::Rush, defaultExit );
    #pragma endregion

    #pragma region Hit
    fsm.AddStateFunctionOnEnter( EPlayerState::Hit,
        [this] ( EPlayerState prevState ) -> StateFuncResult< EPlayerState >
        {
            LogLine( "Entered" );
            this->SendStateChangedPacket( EPlayerState::Hit );
            return StateFuncResult< EPlayerState >( prevState );
        } );
    fsm.AddStateFunctionOnUpdate( EPlayerState::Hit, defaultUpdate );
    fsm.AddStateFunctionOnReceiveInput( EPlayerState::Hit, defaultOnInput );
    fsm.AddStateFunctionOnExit( EPlayerState::Hit, defaultExit );
    #pragma endregion

    #pragma region Win
    fsm.AddStateFunctionOnEnter( EPlayerState::Win,
        [this] ( EPlayerState prevState ) -> StateFuncResult< EPlayerState >
        {
            LogLine( "Entered" );
            this->SendStateChangedPacket( EPlayerState::Win );
            return StateFuncResult< EPlayerState >::NoChange( );
        } );
    fsm.AddStateFunctionOnUpdate( EPlayerState::Win, defaultUpdate );
    fsm.AddStateFunctionOnReceiveInput( EPlayerState::Win, defaultOnInput );
    fsm.AddStateFunctionOnExit( EPlayerState::Win, defaultExit );
    #pragma endregion

    #pragma region Lose
    fsm.AddStateFunctionOnEnter( EPlayerState::Lose,
        [this] ( EPlayerState prevState ) -> StateFuncResult< EPlayerState >
        {
            LogLine( "Entered" );
            this->SendStateChangedPacket( EPlayerState::Lose );
            return StateFuncResult< EPlayerState >::NoChange( );
        } );
    fsm.AddStateFunctionOnUpdate( EPlayerState::Lose, defaultUpdate );
    fsm.AddStateFunctionOnReceiveInput( EPlayerState::Lose, defaultOnInput );
    fsm.AddStateFunctionOnExit( EPlayerState::Lose, defaultExit );
    #pragma endregion

    #pragma region Die
    fsm.AddStateFunctionOnEnter( EPlayerState::Die,
                                [this]( EPlayerState prevState ) -> StateFuncResult< EPlayerState >
                                {
                                    LogLine( "Entered" );
                                    timerRespawnStart.SetNow();
                                    Vector outVector = character->GetLocation().Normalized();
                                    this->SendStateChangedPacket( EPlayerState::Die );
                                    character->StopMove( );
                                    character->AddSpeed( outVector * Constant::CharacterMapOutSpeed );
                                    auto speed = outVector * Constant::CharacterMapOutSpeed;
                                    character->SetForward( outVector );
                                    RemoveBuff();
                                    return StateFuncResult< EPlayerState >::NoChange();
                                } );
    fsm.AddStateFunctionOnUpdate( EPlayerState::Die,
                                 [this]( Double deltaTime ) -> StateFuncResult< EPlayerState >
                                 {
                                     if ( timerRespawnStart.IsOverSeconds( Constant::CharacterRespawnSeconds ) )
                                     {
                                         character->SetLocation( room->GetSpawnLocation( playerIndex ) );
                                         character->SetForward( room->GetSpawnForward( playerIndex ) );
                                         //character->SetSpeed( Vector::Zero() );
                                         BroadcastObjectLocation( true );
                                         return StateFuncResult< EPlayerState >( EPlayerState::Spawn );
                                     }
                                     return StateFuncResult< EPlayerState >::NoChange();
                                 } );
    fsm.AddStateFunctionOnReceiveInput( EPlayerState::Die, defaultOnInput );
    fsm.AddStateFunctionOnExit( EPlayerState::Die, defaultExit );
    #pragma endregion
}

