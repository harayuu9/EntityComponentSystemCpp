#include "BinaryStream.h"

BinaryStream::BinaryStream( const bool isBigSize )
{
	// ほとんどの場合は大きいサイズが必要な想定のため1MB確保してから行う
	if ( isBigSize )
		mBuffer.reserve( 1024 * 1024 );
}

void BinaryStream::write( const void* pBuff, const std::size_t size )
{
	const auto pByte = static_cast<const std::byte*>( pBuff );
	mBuffer.insert( mBuffer.end(), pByte, pByte + size );
}

const char* BinaryStream::getBuffer() const
{
	return reinterpret_cast<const char*>( mBuffer.data() );
}

std::size_t BinaryStream::getSize() const
{
	return mBuffer.size();
}
