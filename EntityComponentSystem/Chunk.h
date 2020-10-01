#pragma once
#include <cassert>

#include "ComponentArray.h"
#include "Archetype.h"

namespace ecs {

class Chunk
{
public:
	template<typename ...Args>
	static Chunk create( const std::uint32_t capacity = 1 )
	{
		return create(Archetype::create<Args...>(), capacity);
	}

	static Chunk create( const Archetype& archetype, std::uint32_t capacity = 1 );

	bool operator==( const Chunk& other ) const;

	bool operator!=( const Chunk& other ) const;

	/**
	 * \brief 現在のSizeでCapacityを切り詰める
	 */
	void fit();

	/**
	 * \brief Chunkにデータを追加する
	 * \param value Archetypeに設定されているデータ
	 * \return 作成したデータのEntity
	 */
	template<typename ...Args>
	Entity addComponentData( const Args&... value )
	{
		// 追加不能の場合メモリの移動を行う
		if ( mCapacity == mSize )
		{
			resetMemory( mCapacity * 2 );
		}

		const auto entity = Entity( mSize );

		addComponentDataImpl( value... );
		++mSize;
		return entity;
	}

	/**
	 * \brief Chunkにデータを追加する
	 * \warning 入っているデータは未定義
	 * \return 作成したEntity
	 */
	Entity createEntity();

	/**
	 * \brief Entityを他のチャンクに移動させる
	 * \param entity 移動させるEntity
	 * \param other 移動先Chunk
	 */
	void moveEntity( Entity& entity, Chunk& other );

	/**
	 * \brief Entityを削除する
	 * \param entity 削除するEntity
	 */
	void destroyEntity( const Entity& entity );

	/**
	 * \brief 既に追加してあるEntityのデータを設定する
	 * \tparam T Archetypeに設定されているいずれかの型
	 * \param entity 設定するEntity
	 * \param data Tの値
	 */
	template<typename T>
	void setComponentData( const Entity& entity, const T& data )
	{
		if ( entity.index >= mSize )
			std::abort();

		using TType = std::remove_const_t<std::remove_reference_t<T>>;
		const auto offset = mArchetype.getOffset<TType>() * mCapacity;
		const auto currentIndex = sizeof TType * entity.index;
		std::memcpy( mpBegin.get() + offset + currentIndex, &data, sizeof TType );
	}

	/**
	 * \brief データを取得する
	 * \tparam T Archetypeに設定されているいずれかの型
	 * \return Tの配列
	 */
	template<class T>
	[[nodiscard]] ComponentArray<T> getComponentArray()
	{
		using TType = std::remove_const_t<std::remove_reference_t<T>>;
		auto offset = mArchetype.getOffset<TType>() * mCapacity;
		return ComponentArray<T>( reinterpret_cast<TType*>( mpBegin.get() + offset ), mSize );
	}

	/**
	 * \brief Archetypeを取得する
	 * \return archetype
	 */
	[[nodiscard]] const Archetype& getArchetype() const;

	/**
	 * \brief Chunkに登録されているデータの数を取得する
	 * \return dataSize
	 */
	[[nodiscard]] std::uint32_t getSize() const;

private:
	/**
	 * \brief チャンクのメモリを再アロケートする
	 * \param capacity 新しいMemoryのサイズ
	 */
	void resetMemory( std::uint32_t capacity );

	template<typename Head, typename ...Types>
	void addComponentDataImpl( Head&& head, Types&&... type )
	{
		using HeadType = std::remove_const_t<std::remove_reference_t<Head>>;
		const auto offset = mArchetype.getOffset<HeadType>() * mCapacity;
		const auto currentIndex = sizeof HeadType * mSize;
		std::memcpy( mpBegin.get() + offset + currentIndex, &head, sizeof HeadType );
		if constexpr ( sizeof...( Types ) > 0 )
			addComponentDataImpl( type... );
	}

	Archetype mArchetype;
	std::unique_ptr<std::byte[]> mpBegin = nullptr;
	std::uint32_t mSize = 0;
	std::uint32_t mCapacity = 1;
};

}
