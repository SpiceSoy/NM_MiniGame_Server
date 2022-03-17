//=================================================================================================
// @file LambdaFSM.h
//
// @brief 플레이어 상태 지정을 위한 간단한 FSM 틀입니다.
// 
// @date 2022/03/17
//
// Copyright 2022 Netmarble Neo, Inc. All Rights Reserved.
//=================================================================================================

#pragma once
#include "Define/DataTypes.h"
#include "Define/PacketDefine.h"
#include <functional>
#include <type_traits>
#include <iostream>
#include <map>


namespace Game
{
	template<typename EStateEnum>
	struct StateFuncResult
	{
		bool isChange;
		EStateEnum nextState;

		StateFuncResult( )
			: isChange( false ), nextState( static_cast<EStateEnum>( 0 ) )
		{
		}

		StateFuncResult( bool isChange, EStateEnum nextState )
			: isChange( isChange ), nextState( nextState )
		{
		}

		StateFuncResult( EStateEnum nextState )
			: StateFuncResult( true, nextState )
		{
		}

		static StateFuncResult<EStateEnum> NoChange( )
		{
			return StateFuncResult( );
		}
	};

	template<typename EStateEnum, bool isValid = std::is_enum_v<EStateEnum> == true>
	class LambdaFSM
	{
		using StateFunctionOnEnter = std::function< StateFuncResult<EStateEnum>( EStateEnum prevState ) >;
		using StateFunctionOnReceiveInput = std::function< StateFuncResult<EStateEnum>( const Packet::Client::Input& input ) >;
		using StateFunctionOnUpdate = std::function< StateFuncResult<EStateEnum>( Double deltaTime ) >;
		using StateFunctionOnExit = std::function< void( EStateEnum nextState ) >;
	private:
		std::map<EStateEnum, StateFunctionOnEnter> onEnterFunctions;
		std::map<EStateEnum, StateFunctionOnReceiveInput> onReceiveInputFunctions;
		std::map<EStateEnum, StateFunctionOnUpdate> onUpdateFunctions;
		std::map<EStateEnum, StateFunctionOnExit> onExitFunctions;
		EStateEnum currentState;

	public:
		LambdaFSM( )
			: currentState( static_cast<EStateEnum>( 0 ) )
		{
			static_assert( isValid, "EStateEnum is must enum");
		}

		void Start( EStateEnum firstState )
		{
			ChangeState( firstState, false );
		}

		void Update( Double deltaTime )
		{
			auto it = onUpdateFunctions.find( currentState );
			if( it == onUpdateFunctions.end( ) )
			{
				std::cout << "FSM has unspecific state Update" << static_cast<int>( currentState ) << std::endl;
			}
			else
			{
				StateFuncResult<EStateEnum> result = it->second( deltaTime );
				if( result.isChange )
				{
					ChangeState( result.nextState, true );
				}
			}
		}

		void OnReceiveInput( const Packet::Client::Input& input )
		{
			auto it = onReceiveInputFunctions.find( currentState );
			if( it == onReceiveInputFunctions.end( ) )
			{
				std::cout << "FSM has unspecific state Input" << static_cast<int>(currentState) << std::endl;
			}
			else
			{
				StateFuncResult<EStateEnum> result = it->second( input );
				if( result.isChange )
				{
					ChangeState( result.nextState, true);
				}
			}
		}
		EStateEnum GetState() const
		{
			return currentState;
		}

		void AddStateFunctionOnEnter( EStateEnum  state, StateFunctionOnEnter&& function)
		{
			onEnterFunctions[state] = std::move(function);
		}
		void AddStateFunctionOnEnter( EStateEnum  state, const StateFunctionOnEnter& function )
		{
			onEnterFunctions[state] = function;
		}

		void AddStateFunctionOnUpdate( EStateEnum  state, StateFunctionOnUpdate&& function )
		{
			onUpdateFunctions[state] = std::move( function );
		}
		void AddStateFunctionOnUpdate( EStateEnum  state, const StateFunctionOnUpdate& function )
		{
			onUpdateFunctions[state] = function;
		}

		void AddStateFunctionOnExit( EStateEnum  state, StateFunctionOnExit&& function )
		{
			onExitFunctions[state] = std::move( function );
		}
		void AddStateFunctionOnExit( EStateEnum  state, const StateFunctionOnExit& function )
		{
			onExitFunctions[state] = function;
		}

		void AddStateFunctionOnReceiveInput( EStateEnum  state, StateFunctionOnReceiveInput&& function )
		{
			onReceiveInputFunctions[state] = std::move( function );
		}
		void AddStateFunctionOnReceiveInput( EStateEnum  state, const StateFunctionOnReceiveInput& function )
		{
			onReceiveInputFunctions[state] = function;
		}

	private:
		void ChangeState( EStateEnum state, bool callExit )
		{
			EStateEnum oldState = currentState;
			currentState = state;
			if( callExit )
			{
				auto exitFuncIt = onExitFunctions.find( oldState );
				if( exitFuncIt == onExitFunctions.end( ) )
				{
					std::cout << "FSM has unspecific state Exit" << static_cast<int>( currentState ) << std::endl;
				}
				else
				{
					exitFuncIt->second( oldState );
				}
			}

			auto enterFuncIt = onEnterFunctions.find( state );
			if( enterFuncIt == onEnterFunctions.end( ) )
			{
				std::cout << "FSM has unspecific state Enter" << static_cast<int>( currentState ) << std::endl;
			}
			else
			{
				StateFuncResult<EStateEnum> result = enterFuncIt->second( oldState );
				if( result.isChange )
				{
					ChangeState( result.nextState, true );
				}
			}
		}
	};
};