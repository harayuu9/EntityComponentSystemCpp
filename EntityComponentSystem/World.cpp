#include "World.h"
#include "EntityManager.h"
#include "SystemBase.h"

namespace ecs {

World::World()
{
	mpEntityManager.reset(new EntityManager(this));
}

World::~World()
{
	for ( auto && system : mSystemList )
	{
		system->onDestroy();
	}
}

EntityManager* World::getEntityManager() const
{
	return mpEntityManager.get();
}

void World::update()
{
	for ( auto && system : mSystemList )
	{
		system->onUpdate();
	}
}

void World::sortSystem()
{
	std::sort(mSystemList.begin(), mSystemList.end(),
		[](const auto& lhs, const auto& rhs)
		{
			return lhs->getExecutionOrder() < rhs->getExecutionOrder();
		});
}

void World::callDoCreate( SystemBase* pSystem )
{
	pSystem->onCreate();
}
}
