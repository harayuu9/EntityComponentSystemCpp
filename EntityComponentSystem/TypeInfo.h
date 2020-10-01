#pragma once

#define DECLARE_TYPE_INFO(T) 						   \
public:												   \
	static constexpr std::string_view getTypeName()	   \
	{												   \
		return #T;									   \
	}												   \
	void _dumyFunction() = delete


namespace type {

template<typename T>
struct CallGetTypeName
{
private:
	template<typename U> static auto Test( int )
	-> decltype(U::getTypeName(), std::true_type());
	template<typename U> static auto Test( ... )
	-> decltype(std::false_type());
public:
	using Type = decltype(Test<T>( 0 ));
};

template<typename T>
constexpr bool cCallGetTypeName = CallGetTypeName<T>::Type::value;
}

class TypeInfo
{
	constexpr explicit
	TypeInfo( const std::string_view typeName, const std::size_t size ) : mTypeName( typeName ), mSize( size )
	{
	}

public:
	constexpr TypeInfo() : mTypeName( "0" ), mSize( 0 )
	{
	}

	constexpr bool operator==( const TypeInfo& other ) const
	{
		return mTypeName.data() == other.mTypeName.data();
	}

	constexpr bool operator!=( const TypeInfo& other ) const
	{
		return !( *this == other );
	}

	[[nodiscard]] constexpr std::string_view getName() const
	{
		return mTypeName;
	}

	[[nodiscard]] constexpr std::size_t getSize() const
	{
		return mSize;
	}

	template<class T, typename = std::enable_if_t<type::cCallGetTypeName<T>>>
	static constexpr TypeInfo create()
	{
		return TypeInfo( T::getTypeName(), sizeof( T ) );
	}

private:
	std::string_view mTypeName;
	std::size_t mSize;
};
