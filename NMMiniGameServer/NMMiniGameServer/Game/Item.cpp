#include "Item.h"
#include "Define/MapData.h"


Game::Item::Item( Int32 index, Vector location, EItemType type )
    : index(index), location( location ), type( type ), radius( Constant::ItemRadius )
{
    spawnTime.SetNow( );
}


Game::EItemType Game::Item::GetType( ) const
{
    return type;
}


void Game::Item::SetType( EItemType type )
{
    this->type = type;
}


Game::Vector Game::Item::GetLocation() const
{
    return location;
}


void Game::Item::SetLocation( const Game::Vector& location )
{
    this->location = location;
}


Double Game::Item::GetRadius() const
{
    return radius;
}


void Game::Item::SetRadius( Double radius )
{
    this->radius = radius;
}


bool Game::Item::IsExpired() const
{
    return spawnTime.IsOverSeconds( Constant::ItemLifeMaxSeconds );
}


Int32 Game::Item::GetIndex() const
{
    return index;
}


