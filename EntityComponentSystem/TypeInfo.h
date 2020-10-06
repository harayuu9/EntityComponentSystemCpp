#pragma once

#define DECLARE_TYPE_INFO(T) 						   \
public:												   \
	static constexpr std::string_view getTypeName()	   \
	{												   \
		return #T;									   \
	}												   \
	static constexpr std::size_t getTypeHash()		   \
	{												   \
		auto fnvOffsetBasis = 14695981039346656037ULL; \
		constexpr auto cFnvPrime = 1099511628211ULL;   \
													   \
		for (auto idx : #T)							   \
		{											   \
			fnvOffsetBasis ^= static_cast<size_t>(idx);\
			fnvOffsetBasis *= cFnvPrime;			   \
		}											   \
		return fnvOffsetBasis;						   \
	}												   \
	void _dumyFunction() = delete


namespace type {

template<typename T>
struct CallGetTypeHash
{
private:
	template<typename U> static auto Test( int )
	-> decltype(U::getTypeHash(), std::true_type());
	template<typename U> static auto Test( ... )
	-> decltype(std::false_type());
public:
	using Type = decltype(Test<T>( 0 ));
};

template<typename T>
constexpr bool cCallGetTypeName = CallGetTypeHash<T>::Type::value;
}

class TypeInfo
{
	constexpr explicit TypeInfo( const std::size_t typeHash, const std::size_t size ) : mTypeHash( typeHash ),
		mSize( size )
	{
	}

public:
	constexpr TypeInfo() : mTypeHash( -1 ), mSize( 0 )
	{
	}

	constexpr bool operator==( const TypeInfo& other ) const
	{
		return mTypeHash == other.mTypeHash;
	}

	constexpr bool operator!=( const TypeInfo& other ) const
	{
		return !( *this == other );
	}

	[[nodiscard]] constexpr std::size_t getHash() const
	{
		return mTypeHash;
	}

	[[nodiscard]] constexpr std::size_t getSize() const
	{
		return mSize;
	}

	template<class T, typename = std::enable_if_t<type::cCallGetTypeName<T>>>
	static constexpr TypeInfo create()
	{
		return TypeInfo( T::getTypeHash(), sizeof( T ) );
	}

	static constexpr TypeInfo create( const std::size_t hash, const std::size_t size )
	{
		return TypeInfo( hash, size );
	}

private:
	std::size_t mTypeHash;
	std::size_t mSize;
};
