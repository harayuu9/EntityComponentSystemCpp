#pragma once

class BinaryStream
{
public:
	explicit BinaryStream( bool isBigSize = true );

	template<class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	void write( const T& value )
	{
		write( &value, sizeof( T ) );
	}

	void write( const void* pBuff, std::size_t size );

	template<class T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
	void read( T* pValue )
	{
		read( pValue, sizeof( T ) );
	}

	void read( void* pBuff, const std::size_t size )
	{
		std::memcpy( pBuff, mBuffer.data() + mCount, size );
		mCount += size;
	}

	[[nodiscard]] const char* getBuffer() const;

	[[nodiscard]] std::size_t getSize() const;

private:
	std::vector<std::byte> mBuffer;
	std::size_t mCount = 0;
};
