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

#include <algorithm>

#include "Game/PlayerCharacter.h"
#include "Game/Room.h"
#include "Define/DataTypes.h"
#include "Define/PacketDefine.h"
#include "Define/MapData.h"
#include "Network/Session.h"


static const char* to_string( Game::EPlayerState e )
{
	using namespace Game;
	switch( e )
	{
	case EPlayerState::Spawn: return "Spawn";
	case EPlayerState::Idle: return "Idle";
	case EPlayerState::Run: return "Run";
	case EPlayerState::Rush: return "Rush";
	case EPlayerState::Rotate: return "Rotate";
	case EPlayerState::Hit: return "Hit";
	case EPlayerState::Win: return "Win";
	case EPlayerState::Lose: return "Lose";
	case EPlayerState::Die: return "Die";
	case EPlayerState::RotateLeft: return "RotateLeft";
	case EPlayerState::RotateRight: return "RotateRight";
	default: return "unknown";
	}
}

Game::PlayerController::PlayerController( )
{
	this->AddStateFunctions( );
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

void Game::PlayerController::Initialize( )
{
	fsm.Start( EPlayerState::Spawn );
	timerRushUse.SetNow( );
	timerRushGen.SetNow( );
	for( int i = 0; i < Constant::MaxRushCount; ++i ) rushQueue.emplace_back( Timer::Now( ) );
}

void Game::PlayerController::SendByte( const Byte* data, UInt64 size ) const
{
	if( !session ) return;
	session->SendByte( data, size );
}

void Game::PlayerController::Update( Double deltaTime )
{
	if( !character ) return;
	fsm.Update( deltaTime );

	// 러시 개수 체크
	if( rushCount < Constant::MaxRushCount )
	{
		auto it = std::next( rushQueue.begin( ), rushCount );
		if( it->IsOverNow( ) )
		{
			rushCount++;
			SendRushCountChangedPacket( );
		}
	}

}

void Game::PlayerController::OnReceivedPacket( const Packet::Header* ptr )
{
	if( !ptr ) return;
	switch( ptr->Type )
	{
	case Packet::EType::ClientInput:
		fsm.OnReceiveInput( *reinterpret_cast<const Packet::Client::Input*>( ptr ) );
		return;
	}
}

Game::EPlayerState Game::PlayerController::GetState( ) const
{
	return fsm.GetState( );
}

void Game::PlayerController::ChangeState( EPlayerState state )
{
	fsm.ChangeState( state );
}

void Game::PlayerController::UseRush( )
{
	timerRushUse.SetNow( ).Add( Constant::RushRecastTime );
	rushQueue.pop_front( );
	rushQueue.emplace_back( Timer::Now( ).Add( Constant::RushRegenSeconds ) );
	character->AddSpeed( character->GetForward( ) * Constant::CharacterRushSpeed );
	rushCount -= 1;
	SendRushCountChangedPacket( );
}

bool Game::PlayerController::CanRush( )
{
	bool canRecast = timerRushUse.IsOverNow( );
	bool hasRushCount = rushQueue.front( ).IsOverNow( );
	if( hasRushCount && canRecast )
	{
		return true;
	}
	return false;
}

void Game::PlayerController::SendStateChangedPacket( ) const
{
	SendStateChangedPacket( this->GetState( ) );
}

void Game::PlayerController::SendRushCountChangedPacket( ) const
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
	printf( "[%02d:%02d:%02d] P%d[%02d:%s] : ", t.tm_hour, t.tm_min, t.tm_sec, playerIndex, static_cast<Int32>(fsm.GetState( )), to_string( fsm.GetState( ) ) );
	va_list va;
	va_start( va, format );
	vprintf_s( format, va );
	va_end( va );
	printf( "\n" );
}

void Game::PlayerController::BroadcastObjectLocation( bool isSetHeight ) const
{
	Packet::Server::ObjectLocation packet;
	packet.targetIndex = playerIndex;
	packet.chracterState = static_cast<Int32>( GetState( ) );
	packet.isSetHeight = isSetHeight;
	Vector location = character->GetLocation( );
	packet.locationX = location.x;
	packet.locationY = location.y;
	packet.locationZ = isSetHeight ? Constant::RespawnHeight : Constant::DefaultHeight;
	//packet.rotation = character.GetRotation();
	Vector forward = character->GetForward( );
	packet.forwardX = forward.x;
	packet.forwardY = forward.y;
	packet.forwardZ = forward.z;

	room->BroadcastPacket( &packet );
}

void Game::PlayerController::SendStateChangedPacket( EPlayerState state ) const
{
	Packet::Server::ObjectStateChanged  packet;
	packet.targetIndex = this->playerIndex;
	packet.chracterState = static_cast<Byte>( state );
	this->room->BroadcastPacket( &packet );
}

void Game::PlayerController::AddStateFunctions( )
{
	auto defaultEnter = [this] ( EPlayerState prevState ) -> StateFuncResult<EPlayerState>
	{
		this->SendStateChangedPacket( );
		return StateFuncResult<EPlayerState>::NoChange( );
	};

	auto defaultUpdate = [] ( Double deltaTime ) -> StateFuncResult<EPlayerState> { return StateFuncResult<EPlayerState>::NoChange( ); };
	auto defaultOnInput = [] ( const Packet::Client::Input& input ) ->StateFuncResult<EPlayerState> { return StateFuncResult<EPlayerState>::NoChange( ); };
	auto defaultExit = [] ( EPlayerState nextState )->void {};

#pragma region Spawn
	fsm.AddStateFunctionOnEnter( EPlayerState::Spawn,
		[this] ( EPlayerState prevState ) -> StateFuncResult<EPlayerState>
		{
			LogLine( "Entered" );
			this->SendStateChangedPacket( );
			this->character->SetMoveSpeed( 0 );
			TimeSecond waitTime = prevState == EPlayerState::Die ? Constant::RespawnSeconds : Constant::FirstSpawnWaitSeconds;
			this->timerSpawnStart.SetNow( ).Add( waitTime );
			return StateFuncResult<EPlayerState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnUpdate( EPlayerState::Spawn,
		[this] ( Double deltaTime ) -> StateFuncResult<EPlayerState>
		{
			if( this->timerSpawnStart.IsOverNow( ) )
			{
				return StateFuncResult<EPlayerState>( EPlayerState::Idle );
			}
			return StateFuncResult<EPlayerState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnReceiveInput( EPlayerState::Spawn, defaultOnInput );
	fsm.AddStateFunctionOnExit( EPlayerState::Spawn, defaultExit );
#pragma endregion

#pragma region Idle
	fsm.AddStateFunctionOnEnter( EPlayerState::Idle, defaultEnter );
	fsm.AddStateFunctionOnUpdate( EPlayerState::Idle, defaultUpdate );
	fsm.AddStateFunctionOnReceiveInput( EPlayerState::Idle,
		[] ( const Packet::Client::Input& input ) ->StateFuncResult<EPlayerState>
		{
			if( input.left == Packet::EInputState::Click ) return StateFuncResult<EPlayerState>( EPlayerState::RotateLeft );
			else if( input.right == Packet::EInputState::Click ) return StateFuncResult<EPlayerState>( EPlayerState::RotateRight );
			else if( input.rush == Packet::EInputState::Click ) return StateFuncResult<EPlayerState>( EPlayerState::Rush );
			return StateFuncResult<EPlayerState>::NoChange( );
		} );
	fsm.AddStateFunctionOnExit( EPlayerState::Idle, defaultExit );
#pragma endregion

#pragma region Run
	fsm.AddStateFunctionOnEnter( EPlayerState::Run,
		[this] ( EPlayerState prevState ) -> StateFuncResult<EPlayerState>
		{
			LogLine( "Entered" );
			this->SendStateChangedPacket( );
			this->character->SetMoveSpeed( Constant::CharacterDefaultSpeed );
			return StateFuncResult<EPlayerState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnUpdate( EPlayerState::Run, defaultUpdate );
	fsm.AddStateFunctionOnReceiveInput( EPlayerState::Run,
		[] ( const Packet::Client::Input& input ) ->StateFuncResult<EPlayerState>
		{
			if( input.left == Packet::EInputState::Click ) return StateFuncResult<EPlayerState>( EPlayerState::RotateLeft );
			else if( input.right == Packet::EInputState::Click ) return StateFuncResult<EPlayerState>( EPlayerState::RotateRight );
			else if( input.rush == Packet::EInputState::Click ) return StateFuncResult<EPlayerState>( EPlayerState::Rush );
			return StateFuncResult<EPlayerState>::NoChange( );
		} );
	fsm.AddStateFunctionOnExit( EPlayerState::Run, defaultExit );
#pragma endregion

#pragma region RotateLeft
	fsm.AddStateFunctionOnEnter( EPlayerState::RotateLeft,
		[this] ( EPlayerState prevState ) -> StateFuncResult<EPlayerState>
		{
			LogLine( "Entered" );
			this->SendStateChangedPacket( EPlayerState::Rotate );
			this->character->SetMoveSpeed( 0 );
			return StateFuncResult<EPlayerState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnUpdate( EPlayerState::RotateLeft,
		[this] ( Double deltaTime ) -> StateFuncResult<EPlayerState>
		{
			character->RotateLeft( Constant::CharacterRotateSpeed * deltaTime );
			return StateFuncResult<EPlayerState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnReceiveInput( EPlayerState::RotateLeft,
		[] ( const Packet::Client::Input& input ) ->StateFuncResult<EPlayerState>
		{
			if( input.left == Packet::EInputState::Release ) return  StateFuncResult<EPlayerState>( EPlayerState::Run );
			else if( input.right == Packet::EInputState::Click ) return StateFuncResult<EPlayerState>( EPlayerState::RotateRight );
			else if( input.rush == Packet::EInputState::Click ) return StateFuncResult<EPlayerState>( EPlayerState::Rush );
			return StateFuncResult<EPlayerState>::NoChange( );
		} );
	fsm.AddStateFunctionOnExit( EPlayerState::RotateLeft, defaultExit );
#pragma endregion

#pragma region RotateRight
	fsm.AddStateFunctionOnEnter( EPlayerState::RotateRight,
		[this] ( EPlayerState prevState ) -> StateFuncResult<EPlayerState>
		{
			LogLine( "Entered" );
			this->SendStateChangedPacket( EPlayerState::Rotate );
			this->character->SetMoveSpeed( 0 );
			return StateFuncResult<EPlayerState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnUpdate( EPlayerState::RotateRight,
		[this] ( Double deltaTime ) -> StateFuncResult<EPlayerState>
		{
			character->RotateRight( Constant::CharacterRotateSpeed * deltaTime );
			return StateFuncResult<EPlayerState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnReceiveInput( EPlayerState::RotateRight,
		[] ( const Packet::Client::Input& input ) ->StateFuncResult<EPlayerState>
		{
			if( input.left == Packet::EInputState::Click ) return  StateFuncResult<EPlayerState>( EPlayerState::RotateLeft );
			else if( input.right == Packet::EInputState::Release ) return StateFuncResult<EPlayerState>( EPlayerState::Run );
			else if( input.rush == Packet::EInputState::Click ) return StateFuncResult<EPlayerState>( EPlayerState::Rush );
			return StateFuncResult<EPlayerState>::NoChange( );
		} );
	fsm.AddStateFunctionOnExit( EPlayerState::RotateRight, defaultExit );
#pragma endregion

#pragma region Rush
	fsm.AddStateFunctionOnEnter( EPlayerState::Rush,
		[this] ( EPlayerState prevState ) -> StateFuncResult<EPlayerState>
		{
			LogLine( "Rush Try" );
			if( this->CanRush( ) )
			{
				LogLine( "Entered" );
				this->SendStateChangedPacket( EPlayerState::Rush );
				this->UseRush( );
				return StateFuncResult<EPlayerState>::NoChange( );
			}
			else
			{
				LogLine( "Failed" );
				return StateFuncResult<EPlayerState>( prevState );
			}
			return StateFuncResult<EPlayerState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnUpdate( EPlayerState::Rush,
		[this] ( Double deltaTime ) -> StateFuncResult<EPlayerState>
		{
			if( this->character->GetSpeed( ).GetLength( ) < 20.0f )
			{
				return StateFuncResult<EPlayerState>( EPlayerState::Run );
			}
			return StateFuncResult<EPlayerState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnReceiveInput( EPlayerState::Rush, defaultOnInput );
	fsm.AddStateFunctionOnExit( EPlayerState::Rush, defaultExit );
#pragma endregion

#pragma region Hit
#pragma endregion

#pragma region Win
#pragma endregion

#pragma region Lose
#pragma endregion

#pragma region Die
	fsm.AddStateFunctionOnEnter( EPlayerState::Die,
		[this] ( EPlayerState prevState ) -> StateFuncResult<EPlayerState>
		{
			LogLine( "Entered" );
			timerRespawnStart.SetNow( );
			Vector outVector = -character->GetLocation( ).Normalized( );
			this->SendStateChangedPacket( EPlayerState::Die );
			character->SetSpeed( outVector * Constant::CharacterMapOutSpeed );
			return StateFuncResult<EPlayerState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnUpdate( EPlayerState::Die,
		[this] ( Double deltaTime ) -> StateFuncResult<EPlayerState>
		{
			if( timerRespawnStart.IsOver( Constant::RespawnSeconds ) )
			{
				character->SetLocation( room->GetSpawnLocation( playerIndex ) );
				character->SetSpeed( Vector::Zero( ) );
				BroadcastObjectLocation( true );
				return StateFuncResult<EPlayerState>( EPlayerState::Spawn );
			}
			return StateFuncResult<EPlayerState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnReceiveInput( EPlayerState::Die, defaultOnInput );
	fsm.AddStateFunctionOnExit( EPlayerState::Die, defaultExit );
#pragma endregion


}


