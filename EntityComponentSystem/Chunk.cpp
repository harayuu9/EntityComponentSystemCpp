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
	// 追加不能の場合メモリの移動を行う
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

void Chunk::marge( Chunk&& other )
{
	// Archetypeが同じでなければマージ出来ない
	if ( mArchetype != other.mArchetype )
		return;

	const auto needSize = mSize + other.mSize;
	if ( mCapacity < needSize )
	{
		resetMemory( needSize );
		mCapacity = needSize;
	}

	for ( auto i = 0; i < mArchetype.getArchetypeSize(); ++i )
	{
		const auto archetypeOffset = mArchetype.getOffsetByIndex( i );
		const auto archetypeSize = mArchetype.getSize( i );
		const auto offset = archetypeOffset * mCapacity + archetypeSize * mSize;
		const auto otherOffset = archetypeOffset * other.mCapacity;
		std::memcpy( mpBegin.get() + offset, other.mpBegin.get() + otherOffset, archetypeSize * other.mSize );
	}
	mSize += other.mSize;
	
	other.mpBegin.reset();
	other.mSize = 0;
	other.mCapacity = 0;
}

void Chunk::writeBinaryStream( BinaryStream& output )
{
	// 書き込み前にチャンク内を整理する
	fit();

	// Archetype出力
	const auto archetypeSize = mArchetype.getArchetypeSize();
	output.write( archetypeSize );
	for ( auto i = 0; i < archetypeSize; ++i )
	{
		const auto typeHash = mArchetype.getTypeInfo( i ).getHash();
		const auto typeSize = mArchetype.getTypeInfo( i ).getSize();
		output.write( typeHash );
		output.write( typeSize );
	}

	// ComponentData出力
	output.write( mSize );
	output.write( mpBegin.get(), mArchetype.getArchetypeMemorySize() * mSize );
}

Chunk Chunk::readBinaryStream( BinaryStream& input )
{
	Chunk result;

	// Archetype読み込み
	std::size_t archetypeSize;
	input.read( &archetypeSize );
	for ( auto i = 0; i < archetypeSize; ++i )
	{
		std::size_t typeHash, typeSize;
		input.read( &typeHash );
		input.read( &typeSize );
		result.mArchetype.addType( typeHash, typeSize );
	}

	// ComponentData読み込み
	input.read( &result.mSize );
	result.mCapacity = result.mSize;
	result.mpBegin = std::make_unique<std::byte[]>( result.mCapacity * result.mArchetype.getArchetypeMemorySize() );
	input.read( result.mpBegin.get(), result.mCapacity * result.mArchetype.getArchetypeMemorySize() );
	return result;
}

void Chunk::resetMemory( const std::uint32_t capacity )
{
	auto pWork = std::make_unique<std::byte[]>( mArchetype.getArchetypeMemorySize() * capacity );

	std::size_t offsetBase = 0;

	for ( std::size_t i = 0; i < mArchetype.getArchetypeSize(); ++i )
	{
		const auto offset = offsetBase * mCapacity;
		const auto newOffset = offsetBase * capacity;
		std::memcpy( pWork.get() + newOffset, mpBegin.get() + offset, mArchetype.getSize( i ) * mSize );

		offsetBase += mArchetype.getSize( i );
	}

	mCapacity = capacity;
	mpBegin = std::move( pWork );
}
}
