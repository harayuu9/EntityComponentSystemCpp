#include "SystemBase.h"

namespace ecs {

SystemBase::SystemBase( World* pWorld ): mpWorld( pWorld )
{
}

void SystemBase::onCreate()
{
}

void SystemBase::onDestroy()
{
}

int SystemBase::getExecutionOrder() const
{
	return mExecutionOrder;
}

EntityManager* SystemBase::getEntityManager() const
{
	return mpWorld->getEntityManager();
}

void SystemBase::setExecutionOrder( const int executionOrder )
{
	mExecutionOrder = executionOrder;
}
}
