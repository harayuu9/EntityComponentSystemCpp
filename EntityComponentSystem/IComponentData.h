#pragma once

#define ECS_DECLARE_COMPONENT_DATA(T)					\
DECLARE_TYPE_INFO( T )
#include "TypeInfo.h"

namespace ecs {

struct IComponentData
{
};

constexpr auto cMaxComponentSize = 16;

template<class T>
constexpr bool cIsComponentData = std::is_base_of_v<IComponentData, T> && std::is_trivial_v<T> &&
	std::is_trivially_destructible_v<T> && type::cCallGetTypeName<T>;

struct Entity
{
	explicit Entity( const std::uint32_t idx )
	{
		chunkIndex = std::numeric_limits<std::uint32_t>::max();
		index = idx;
	}

	Entity( const std::uint32_t chunkIdx, const std::uint32_t idx )
	{
		chunkIndex = chunkIdx;
		index = idx;
	}

	std::uint32_t index;
	std::uint32_t chunkIndex;
};

}
