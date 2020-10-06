#include "World.h"
#include "EntityManager.h"
#include "SystemBase.h"

namespace ecs {

World::World()
{
	mpEntityManager.reset( new EntityManager( this ) );
}

World::~World()
{
	for ( auto&& system : mSystemList )
	{
		system->onDestroy();
	}
}

EntityManager* World::getEntityManager() const
{
	return mpEntityManager.get();
}

void World::update()
{
	for ( auto&& system : mSystemList )
	{
		system->onUpdate();
	}
}

void World::marge( World&& other )
{
	for ( auto& otherChunk : other.mChunkList )
	{
		auto findItr = std::find( mChunkList.begin(), mChunkList.end(), otherChunk );
		if ( findItr == mChunkList.end() )
		{
			mChunkList.push_back( std::move( otherChunk ) );
		}
		else
		{
			findItr->marge( std::move( otherChunk ) );
		}
	}
	other.mChunkList.clear();
	other.mSystemList.clear();
}

void World::writeBinaryStream( BinaryStream& output )
{
	const auto chunkSize = mChunkList.size();
	output.write( chunkSize );
	for ( auto&& chunkList : mChunkList )
	{
		chunkList.writeBinaryStream( output );
	}
}

void World::readBinaryStream( BinaryStream& input )
{
	std::size_t chunkSize;
	input.read( &chunkSize );
	mChunkList.resize( chunkSize );
	for ( auto i = 0; i < chunkSize; ++i )
	{
		mChunkList[i] = Chunk::readBinaryStream( input );
	}
}

void World::sortSystem()
{
	std::sort( mSystemList.begin(), mSystemList.end(),
	           []( const auto& lhs, const auto& rhs )
	           {
		           return lhs->getExecutionOrder() < rhs->getExecutionOrder();
	           } );
}

void World::callDoCreate( SystemBase* pSystem )
{
	pSystem->onCreate();
}
}
