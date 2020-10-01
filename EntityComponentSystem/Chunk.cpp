#include "Chunk.h"

namespace ecs {

Chunk Chunk::create( const Archetype& archetype, const std::uint32_t capacity )
{
	Chunk result;
	result.mCapacity = capacity;
	result.mArchetype = archetype;
	result.mpBegin = std::make_unique<std::byte[]>( result.mCapacity * result.mArchetype.getArchetypeMemorySize() );
	return result;
}

bool Chunk::operator==( const Chunk& other ) const
{
	return mArchetype == other.mArchetype;
}

bool Chunk::operator!=( const Chunk& other ) const
{
	return mArchetype != other.mArchetype;
}

void Chunk::fit()
{
	resetMemory( mSize );
}

Entity Chunk::createEntity()
{
	// ’Ç‰Á•s”\‚Ìê‡ƒƒ‚ƒŠ‚ÌˆÚ“®‚ğs‚¤
	if ( mCapacity == mSize )
	{
		resetMemory( mCapacity * 2 );
	}

	const auto entity = Entity( mSize );
	++mSize;
	return entity;
}

void Chunk::moveEntity( Entity& entity, Chunk& other )
{
	const auto newEntity = other.createEntity();

	for ( auto i = 0; i < mArchetype.getArchetypeSize(); ++i )
	{
		const auto currentOffset = mArchetype.getOffsetByIndex( i ) * mCapacity;
		const auto currentIndex = currentOffset + mArchetype.getSize( i ) * entity.index;

		const auto otherI = other.mArchetype.getIndexByTypeInfo( mArchetype.getTypeInfo( i ) );
		const auto otherOffset = other.mArchetype.getOffsetByIndex( otherI ) * other.mCapacity;
		const auto otherIndex = otherOffset + other.mArchetype.getSize( otherI ) * newEntity.index;

		std::memcpy( other.mpBegin.get() + otherIndex, mpBegin.get() + currentIndex, mArchetype.getSize( i ) );
	}

	destroyEntity( entity );
	entity = newEntity;
}

void Chunk::destroyEntity( const Entity& entity )
{
	for ( auto i = 0; i < mArchetype.getArchetypeSize(); ++i )
	{
		const auto offset = mArchetype.getOffsetByIndex( i ) * mCapacity;
		const auto currentIndex = offset + mArchetype.getSize( i ) * entity.index;
		std::memmove( mpBegin.get() + currentIndex, mpBegin.get() + currentIndex + mArchetype.getSize( i ),
		              mArchetype.getSize( i ) * ( mSize - entity.index - 1 ) );
	}
	--mSize;
}

const Archetype& Chunk::getArchetype() const
{
	return mArchetype;
}

std::uint32_t Chunk::getSize() const
{
	return mSize;
}

void Chunk::resetMemory( const std::uint32_t capacity )
{
	auto pWork = std::make_unique<std::byte[]>( mArchetype.getArchetypeMemorySize() * capacity );

	std::size_t offsetBase = 0;

	for ( std::size_t i = 0; i < mArchetype.getArchetypeSize(); ++i )
	{
		const auto offset = offsetBase * mCapacity;
		const auto newOffset = offsetBase * capacity;
		std::memcpy( pWork.get() + newOffset, mpBegin.get() + offset, mArchetype.getSize( i ) * mCapacity );

		offsetBase += mArchetype.getSize( i );
	}

	mCapacity = capacity;
	mpBegin = std::move( pWork );
}
}
