#pragma once
#include "IComponentData.h"

namespace ecs {

/**
 * \brief
 * Chunk内のデータ構造を表す
 * 型の順番を関係なく扱う
 * Archetype::create<Position,Rotation,Scale>() == Archetype::create<Rotation,Scale,Position>()
 */
struct Archetype
{
	constexpr bool operator==( const Archetype& other ) const
	{
		if ( mArchetypeSize != other.mArchetypeSize )
			return false;
		for ( auto i = 0; i < mArchetypeSize; ++i )
		{
			if ( mTypeDataList[i] != other.mTypeDataList[i] )
				return false;
		}
		return true;
	}

	constexpr bool operator!=( const Archetype& other ) const
	{
		return !( *this == other );
	}

	/**
	 * \brief 自身が持っているTypeをotherが全て含んでいるかどうかチェック
	 * \param other 相手
	 * \return this in other ならTrue
	 */
	[[nodiscard]] constexpr bool isIn( const Archetype& other ) const
	{
		for ( auto i = 0; i < mArchetypeSize; ++i )
		{
			auto isIn = false;
			for ( auto j = 0; j < other.mArchetypeSize; ++j )
			{
				if ( mTypeDataList[i] == other.mTypeDataList[j] )
				{
					isIn = true;
				}
			}
			if ( !isIn )
				return false;
		}
		return true;
	}

	/**
	 * \brief ComponentDataからArchetypeを作成する
	 * \tparam Args ComponentData...
	 * \return Archetype
	 */
	template<typename ...Args>
	static constexpr Archetype create()
	{
		Archetype result;
		result.createImpl<Args...>();

		for ( auto i = 0; i < result.mArchetypeSize - 1; ++i )
		{
			for ( auto j = i + 1; j < result.mArchetypeSize; ++j )
			{
				if ( result.mTypeDataList[i].getName() > result.mTypeDataList[j].getName() )
				{
					const auto work = result.mTypeDataList[i];
					result.mTypeDataList[i] = result.mTypeDataList[j];
					result.mTypeDataList[j] = work;
				}
			}
		}

		for ( auto i = 0; i < result.mArchetypeSize; i++ )
		{
			result.mArchetypeMemorySize += result.mTypeDataList[i].getSize();
		}
		return result;
	}

	/**
	 * \brief Archetypeを追加で登録する
	 * \tparam T 追加するType
	 * \return 自身の参照
	 */
	template<typename T>
	constexpr Archetype& addType()
	{
		constexpr auto newType = TypeInfo::create<T>();
		mArchetypeMemorySize += sizeof( T );
		for ( std::size_t i = 0; i < mArchetypeSize; ++i )
		{
			if ( mTypeDataList[i].getName() > newType.getName() )
			{
				for ( auto j = mArchetypeSize; j > i; --j )
				{
					mTypeDataList[j] = mTypeDataList[j - 1];
				}
				mTypeDataList[i] = newType;
				++mArchetypeSize;
				return *this;
			}
		}

		mTypeDataList[mArchetypeSize] = newType;
		mArchetypeSize++;

		return *this;
	}

	/**
	 * \brief IComponentDataを継承した型が何番目に登録されているか
	 * \tparam Type IComponentData
	 * \return Typeのある番号
	 */
	template<typename Type, typename = std::enable_if_t<cIsComponentData<Type>>>
	[[nodiscard]] constexpr std::size_t getIndex() const
	{
		for ( auto i = 0; i < mArchetypeSize; ++i )
		{
			if ( mTypeDataList[i] == TypeInfo::create<Type>() )
				return i;
		}
		return mArchetypeSize;
	}

	/**
	 * \brief IComponentDataを継承した型までのメモリサイズを取得
	 * \details Archetypeに含まれない型の場合getArchetypeSize()と同じ値を返す 
	 * \tparam Type IComponentData
	 * \return Indexまでのバイト数
	 */
	template<typename Type, typename = std::enable_if_t<cIsComponentData<Type>>>
	[[nodiscard]] constexpr std::size_t getOffset() const
	{
		std::size_t result = 0;
		for ( auto i = 0; i < mArchetypeSize; ++i )
		{
			if ( mTypeDataList[i] == TypeInfo::create<Type>() )
				return result;
			result += mTypeDataList[i].getSize();
		}
		return result;
	}

	/**
	 * \brief indexまでのメモリサイズを取得
	 * \param index N番目
	 * \return indexまでのバイト数
	 */
	[[nodiscard]] constexpr std::size_t getOffsetByIndex( const std::size_t index ) const
	{
		std::size_t result = 0;
		for ( auto i = 0; i < index; ++i )
		{
			result += mTypeDataList[i].getSize();
		}
		return result;
	}

	/**
	 * \brief TypeInfoからIndexを取得する
	 * \param info 型情報
	 * \return Index
	 */
	[[nodiscard]] constexpr std::size_t getIndexByTypeInfo( const TypeInfo info ) const
	{
		for ( auto i = 0; i < mArchetypeSize; ++i )
		{
			if ( mTypeDataList[i] == info )
				return i;
		}
		return mArchetypeSize;
	}

	/**
	 * \brief N番目のTypeのサイズを取得する
	 * \param index N
	 * \return サイズ
	 */
	[[nodiscard]] constexpr std::size_t getSize( const std::size_t index ) const
	{
		return mTypeDataList[index].getSize();
	}

	/**
	 * \brief N番目のTypeInfoを取得する
	 * \param index N
	 * \return TypeInfo
	 */
	[[nodiscard]] constexpr TypeInfo getTypeInfo( const std::size_t index ) const
	{
		return mTypeDataList[index];
	}

	/**
	 * \brief Archetypeの数を取得する
	 * \return Archetypeの数
	 */
	[[nodiscard]] constexpr std::size_t getArchetypeSize() const
	{
		return mArchetypeSize;
	}

	/**
	 * \brief ArchetypeのByte数を取得する
	 * \return Archetypeの数
	*/
	[[nodiscard]] constexpr std::size_t getArchetypeMemorySize() const
	{
		return mArchetypeMemorySize;
	}

private:
	template<typename Head, typename ...Tails, typename = std::enable_if_t<cIsComponentData<Head>>>
	constexpr void createImpl()
	{
		mTypeDataList[mArchetypeSize] = TypeInfo::create<Head>();
		mArchetypeSize++;
		if constexpr ( sizeof...( Tails ) != 0 )
			createImpl<Tails...>();
	}

	TypeInfo mTypeDataList[cMaxComponentSize];
	std::size_t mArchetypeMemorySize = 0;
	std::size_t mArchetypeSize = 0;
};

}
