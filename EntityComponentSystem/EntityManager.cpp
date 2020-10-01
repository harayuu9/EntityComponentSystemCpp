#include "EntityManager.h"

namespace ecs {

Entity EntityManager::createEntity( const Archetype& archetype ) const
{
	const auto chunkIndex = getAndCreateChunkIndex(archetype);
	auto entity = mpWorld->mChunkList[chunkIndex].createEntity();
	entity.chunkIndex = chunkIndex;
	
	return entity;
}

void EntityManager::destroyEntity( const Entity& entity ) const
{
	mpWorld->mChunkList[entity.chunkIndex].destroyEntity( entity );
}

std::vector<Chunk*> EntityManager::getChunkList( const Archetype& archetype ) const
{
	std::vector<Chunk*> result;
	result.reserve( 4 );
	for ( auto&& chunk : mpWorld->mChunkList )
	{
		if ( archetype.isIn( chunk.getArchetype() ) )
			result.push_back( &chunk );
	}
	return result;
}

std::uint32_t EntityManager::getAndCreateChunkIndex( const Archetype& archetype ) const
{
	auto chunkIndex = 0;
	for ( auto&& chunk : mpWorld->mChunkList )
	{
		if (chunk.getArchetype() == archetype)
			return chunkIndex;
		++chunkIndex;
	}

	mpWorld->mChunkList.push_back( Chunk::create( archetype ) );
	return chunkIndex;
}
}
