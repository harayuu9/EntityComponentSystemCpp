#pragma once
#include "World.h"
#include "EntityManager.h"

namespace ecs {

class SystemBase
{
public:
	SystemBase( SystemBase& ) = delete;
	SystemBase( SystemBase&& ) = delete;

	explicit SystemBase( World* pWorld );

	virtual ~SystemBase() = default;

	virtual void onCreate();

	virtual void onUpdate() = 0;

	virtual void onDestroy();

	[[nodiscard]] int getExecutionOrder() const;

protected:
	[[nodiscard]] EntityManager* getEntityManager() const;

	void setExecutionOrder( int executionOrder );

	template<class T1, typename Func>
	void foreach( Func&& func );

	template<class T1, class T2, typename Func>
	void foreach( Func&& func );

	template<class T1, class T2, class T3, typename Func>
	void foreach( Func&& func );

	template<class T1, class T2, class T3, class T4, typename Func>
	void foreach( Func&& func );

private:

	template<typename Func, class... Args>
	static void foreachImpl_( Chunk* pChunk, Func&& func, Args ... args );

	World* mpWorld = nullptr;
	int mExecutionOrder = 0;
};


template<class T1, typename Func> void SystemBase::foreach( Func&& func )
{
	auto pChunkList = getEntityManager()->getChunkList<T1>();
	for ( auto&& pChunk : pChunkList )
	{
		auto arg1 = pChunk->template getComponentArray<T1>();
		foreachImpl_(pChunk, func, arg1);
	}
}

template<class T1, class T2, typename Func> void SystemBase::foreach( Func&& func )
{
	auto pChunkList = getEntityManager()->getChunkList<T1, T2>();
	for ( auto&& pChunk : pChunkList )
	{
		auto arg1 = pChunk->template getComponentArray<T1>();
		auto arg2 = pChunk->template getComponentArray<T2>();
		foreachImpl_(pChunk, func, arg1, arg2);
	}
}

template<class T1, class T2, class T3, typename Func> void SystemBase::foreach( Func&& func )
{
	auto pChunkList = getEntityManager()->getChunkList<T1, T2, T3>();
	for ( auto&& pChunk : pChunkList )
	{
		auto arg1 = pChunk->template getComponentArray<T1>();
		auto arg2 = pChunk->template getComponentArray<T2>();
		auto arg3 = pChunk->template getComponentArray<T3>();
		foreachImpl_(pChunk, func, arg1, arg2, arg3);
	}
}

template<class T1, class T2, class T3, class T4, typename Func> void SystemBase::foreach( Func&& func )
{
	auto pChunkList = getEntityManager()->getChunkList<T1, T2, T3, T4>();
	for ( auto&& pChunk : pChunkList )
	{
		auto arg1 = pChunk->template getComponentArray<T1>();
		auto arg2 = pChunk->template getComponentArray<T2>();
		auto arg3 = pChunk->template getComponentArray<T3>();
		auto arg4 = pChunk->template getComponentArray<T4>();
		foreachImpl_(pChunk, func, arg1, arg2, arg3, arg4);
	}
}

template<typename Func, class ... Args> void SystemBase::foreachImpl_( Chunk* pChunk, Func&& func, Args ... args )
{
	for ( std::uint32_t i = 0; i < pChunk->getSize(); ++i )
	{
		func( args[i]... );
	}
}

}
