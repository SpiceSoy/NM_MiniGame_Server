//=================================================================================================
// @file PlayerController.h
//
// @brief �̴ϰ��� ������ �÷��̾��� ���� �Է� ó���� ����� Ŭ�����Դϴ�.
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

using namespace std::literals::chrono_literals;


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
	fsm.Start( EState::Spawn );
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

	// ���� ���� üũ
	if( rushCount < Constant::MaxRushCount )
	{
		auto it = std::next(rushQueue.begin(), rushCount );
		if(it->IsOverNow())
		{
			rushCount++;
			SendRushCountChangedPacket();
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

Game::PlayerController::EState Game::PlayerController::GetState( ) const
{
	return fsm.GetState( );
}

void Game::PlayerController::ChangeState( EState state )
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
		std::cout << "rush" << std::endl;
		return true;
	}
	std::cout << "can't rush cool" << std::endl;
	return false;
}

void Game::PlayerController::SendStateChangedPacket( ) const
{
	SendStateChangedPacket( this->GetState( ) );
}

void Game::PlayerController::SendRushCountChangedPacket( ) const
{
	std::cout << "Rush Changed" << std::endl;
	Packet::Server::PlayerRushCountChanged packet;
	//UInt32 count = std::count_if( rushQueue.begin( ), rushQueue.end( ), [] ( Timer timer ) {return timer.IsOverNow( ); } );
	packet.count = rushCount;
	SendPacket( &packet );
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

void Game::PlayerController::SendStateChangedPacket( EState state ) const
{
	Packet::Server::ObjectStateChanged  packet;
	packet.targetIndex = this->playerIndex;
	packet.chracterState = static_cast<Byte>( state );
	this->room->BroadcastPacket( &packet );
}

void Game::PlayerController::AddStateFunctions( )
{
	auto defaultEnter = [this] ( EState prevState ) -> StateFuncResult<EState>
	{
		this->SendStateChangedPacket( );
		return StateFuncResult<EState>::NoChange( );
	};

	auto defaultUpdate = [] ( Double deltaTime ) -> StateFuncResult<EState> { return StateFuncResult<EState>::NoChange( ); };
	auto defaultOnInput = [] ( const Packet::Client::Input& input ) ->StateFuncResult<EState> { return StateFuncResult<EState>::NoChange( ); };
	auto defaultExit = [] ( EState nextState )->void {};

#pragma region Spawn
	fsm.AddStateFunctionOnEnter( EState::Spawn,
		[this] ( EState prevState ) -> StateFuncResult<EState>
		{
			std::cout << "Enter Spawn" << std::endl;
			this->SendStateChangedPacket( );
			this->character->SetMoveSpeed( 0 );
			TimeSecond waitTime = prevState == EState::Die ? Constant::RespawnSeconds : Constant::FirstSpawnWaitSeconds;
			this->timerSpawnStart.SetNow( ).Add( waitTime );
			return StateFuncResult<EState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnUpdate( EState::Spawn,
		[this] ( Double deltaTime ) -> StateFuncResult<EState>
		{
			if( this->timerSpawnStart.IsOverNow( ) )
			{
				std::cout << "Change idle" << std::endl;
				return StateFuncResult<EState>( EState::Idle );
			}
			return StateFuncResult<EState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnReceiveInput( EState::Spawn, defaultOnInput );
	fsm.AddStateFunctionOnExit( EState::Spawn, defaultExit );
#pragma endregion

#pragma region Idle
	fsm.AddStateFunctionOnEnter( EState::Idle, defaultEnter );
	fsm.AddStateFunctionOnUpdate( EState::Idle, defaultUpdate );
	fsm.AddStateFunctionOnReceiveInput( EState::Idle,
		[] ( const Packet::Client::Input& input ) ->StateFuncResult<EState>
		{
			if( input.left == Packet::EInputState::Click ) return StateFuncResult<EState>( EState::RotateLeft );
			else if( input.right == Packet::EInputState::Click ) return StateFuncResult<EState>( EState::RotateRight );
			else if( input.rush == Packet::EInputState::Click ) return StateFuncResult<EState>( EState::Rush );
			return StateFuncResult<EState>::NoChange( );
		} );
	fsm.AddStateFunctionOnExit( EState::Idle, defaultExit );
#pragma endregion

#pragma region Run
	fsm.AddStateFunctionOnEnter( EState::Run,
		[this] ( EState prevState ) -> StateFuncResult<EState>
		{
			std::cout << "Change Run" << std::endl;
			this->SendStateChangedPacket( );
			this->character->SetMoveSpeed( Constant::CharacterDefaultSpeed );
			return StateFuncResult<EState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnUpdate( EState::Run, defaultUpdate );
	fsm.AddStateFunctionOnReceiveInput( EState::Run,
		[] ( const Packet::Client::Input& input ) ->StateFuncResult<EState>
		{
			if( input.left == Packet::EInputState::Click ) return StateFuncResult<EState>( EState::RotateLeft );
			else if( input.right == Packet::EInputState::Click ) return StateFuncResult<EState>( EState::RotateRight );
			else if( input.rush == Packet::EInputState::Click ) return StateFuncResult<EState>( EState::Rush );
			return StateFuncResult<EState>::NoChange( );
		} );
	fsm.AddStateFunctionOnExit( EState::Run, defaultExit );
#pragma endregion

#pragma region RotateLeft
	fsm.AddStateFunctionOnEnter( EState::RotateLeft,
		[this] ( EState prevState ) -> StateFuncResult<EState>
		{
			std::cout << "Change Rotate Left" << std::endl;
			this->SendStateChangedPacket( EState::Rotate );
			this->character->SetMoveSpeed( 0 );
			return StateFuncResult<EState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnUpdate( EState::RotateLeft,
		[this] ( Double deltaTime ) -> StateFuncResult<EState>
		{
			character->RotateLeft( Constant::CharacterRotateSpeed * deltaTime );
			return StateFuncResult<EState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnReceiveInput( EState::RotateLeft,
		[] ( const Packet::Client::Input& input ) ->StateFuncResult<EState>
		{
			if( input.left == Packet::EInputState::Release ) return  StateFuncResult<EState>( EState::Run );
			else if( input.right == Packet::EInputState::Click ) return StateFuncResult<EState>( EState::RotateRight );
			else if( input.rush == Packet::EInputState::Click ) return StateFuncResult<EState>( EState::Rush );
			return StateFuncResult<EState>::NoChange( );
		} );
	fsm.AddStateFunctionOnExit( EState::RotateLeft, defaultExit );
#pragma endregion

#pragma region RotateRight
	fsm.AddStateFunctionOnEnter( EState::RotateRight,
		[this] ( EState prevState ) -> StateFuncResult<EState>
		{
			std::cout << "Change Rotate Right" << std::endl;
			this->SendStateChangedPacket( EState::Rotate );
			this->character->SetMoveSpeed( 0 );
			return StateFuncResult<EState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnUpdate( EState::RotateRight,
		[this] ( Double deltaTime ) -> StateFuncResult<EState>
		{
			character->RotateRight( Constant::CharacterRotateSpeed * deltaTime );
			return StateFuncResult<EState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnReceiveInput( EState::RotateRight,
		[] ( const Packet::Client::Input& input ) ->StateFuncResult<EState>
		{
			if( input.left == Packet::EInputState::Click ) return  StateFuncResult<EState>( EState::RotateLeft );
			else if( input.right == Packet::EInputState::Release ) return StateFuncResult<EState>( EState::Run );
			else if( input.rush == Packet::EInputState::Click ) return StateFuncResult<EState>( EState::Rush );
			return StateFuncResult<EState>::NoChange( );
		} );
	fsm.AddStateFunctionOnExit( EState::RotateRight, defaultExit );
#pragma endregion

#pragma region Rush
	fsm.AddStateFunctionOnEnter( EState::Rush,
		[this] ( EState prevState ) -> StateFuncResult<EState>
		{
			std::cout << "Try Rush" << std::endl;
			if( this->CanRush( ) )
			{
				this->SendStateChangedPacket( EState::Rush );
				this->UseRush( );
				return StateFuncResult<EState>::NoChange( );
			}
			else
			{
				return StateFuncResult<EState>( prevState );
			}
			return StateFuncResult<EState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnUpdate( EState::Rush,
		[this] ( Double deltaTime ) -> StateFuncResult<EState>
		{
			if( this->character->GetSpeed( ).GetLength( ) < 20.0f )
			{
				return StateFuncResult<EState>( EState::Run );
			}
			return StateFuncResult<EState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnReceiveInput( EState::Rush, defaultOnInput );
	fsm.AddStateFunctionOnExit( EState::Rush, defaultExit );
#pragma endregion

#pragma region Hit
#pragma endregion

#pragma region Win
#pragma endregion

#pragma region Lose
#pragma endregion

#pragma region Die
	fsm.AddStateFunctionOnEnter( EState::Die,
		[this] ( EState prevState ) -> StateFuncResult<EState>
		{
			std::cout << "Enter Die" << std::endl;
			timerRespawnStart.SetNow( );
			Vector outVector = -character->GetLocation( ).Normalized( );
			character->SetSpeed( outVector * Constant::CharacterMapOutSpeed );
			return StateFuncResult<EState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnUpdate( EState::Die,
		[this] ( Double deltaTime ) -> StateFuncResult<EState>
		{
			if( timerRespawnStart.IsOver( Constant::RespawnSeconds ) )
			{
				character->SetLocation( room->GetSpawnLocation( playerIndex ) );
				character->SetSpeed( Vector::Zero( ) );
				BroadcastObjectLocation( true );
				return StateFuncResult<EState>( EState::Spawn );
			}
			return StateFuncResult<EState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnReceiveInput( EState::Die, defaultOnInput );
	fsm.AddStateFunctionOnExit( EState::Die, defaultExit );
#pragma endregion


}


