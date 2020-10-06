#pragma once
#include "IComponentData.h"
#include "Archetype.h"
#include "World.h"

namespace ecs {

class EntityManager
{
public:
	explicit EntityManager( World* pWorld ) : mpWorld( pWorld )
	{
	}

	[[nodiscard]] Entity createEntity( const Archetype& archetype ) const;

	template<typename ...Args>
	[[nodiscard]] Entity createEntity() const
	{
		constexpr auto archetype = Archetype::create<Args...>();
		return createEntity( archetype );
	}

	void destroyEntity( const Entity& entity ) const;

	template<typename T>
	void addComponentData( Entity& entity, const T& data )
	{
		auto newArchetype = mpWorld->mChunkList[entity.chunkIndex].getArchetype();
		newArchetype.addType<T>();
		const auto newChunkIndex = getAndCreateChunkIndex( newArchetype );

		auto& chunk = mpWorld->mChunkList[newChunkIndex];
		mpWorld->mChunkList[entity.chunkIndex].moveEntity( entity, chunk );
		entity.chunkIndex = newChunkIndex;
		chunk.setComponentData( entity, data );
	}

	template<typename T, typename = std::enable_if_t<cIsComponentData<T>>>
	void setComponentData( const Entity& entity, const T& data )
	{
		mpWorld->mChunkList[entity.chunkIndex].setComponentData( entity, data );
	}

	[[nodiscard]] std::vector<Chunk*> getChunkList( const Archetype& archetype ) const;

	template<typename ...Args>
	[[nodiscard]] std::vector<Chunk*> getChunkList() const
	{
		constexpr auto archetype = Archetype::create<Args...>();
		return getChunkList( archetype );
	}

	[[nodiscard]] std::uint32_t getAndCreateChunkIndex( const Archetype& archetype ) const;

	template<typename ...Args>
	[[nodiscard]] std::uint32_t getAndCreateChunkIndex() const
	{
		constexpr auto archetype = Archetype::create<Args...>();
		return getAndCreateChunkIndex( archetype );
	}

private:
	World* mpWorld = nullptr;
};

}
