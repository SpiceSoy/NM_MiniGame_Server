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
}

void Game::PlayerController::SendByte( const Byte* data, UInt64 size )
{
	if( !session ) return;
	session->SendByte( data, size );
}

void Game::PlayerController::Update( Double deltaTime )
{
	if( !character ) return;
	fsm.Update( deltaTime );
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

void Game::PlayerController::UseRush( )
{
	if( ( timerRushUse.IsOver( 1s ) ) )
	{
		timerRushUse.SetNow( );
		character->AddSpeed( character->GetForward( ) * Constant::CharacterRushSpeed );
		std::cout << "rush" << std::endl;
	}
	std::cout << "can't rush cool" << std::endl;
}

bool Game::PlayerController::CanRush( )
{
	return rushStack > 0 || timerRushGen.IsOverNow( );
}

void Game::PlayerController::SendStateChangedPacket( ) const
{
	SendStateChangedPacket( this->GetState( ) );
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
			this->SendStateChangedPacket( );
			this->timerSpawnStart.SetNow( );
			return StateFuncResult<EState>::NoChange( );
		}
	);
	fsm.AddStateFunctionOnUpdate( EState::Spawn,
		[this] ( Double deltaTime ) -> StateFuncResult<EState>
		{
			if( this->timerSpawnStart.IsOver( 3s ) )
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
#pragma endregion


}


