#include <ECS.h>
#include "TimeObserver.h"
#include <fstream>

struct Position : ecs::IComponentData
{
ECS_DECLARE_COMPONENT_DATA( Position );
	float x, y, z;
};

struct Rotation : ecs::IComponentData
{
ECS_DECLARE_COMPONENT_DATA( Rotation );
	float x, y, z, w;
};

struct Scale : ecs::IComponentData
{
ECS_DECLARE_COMPONENT_DATA( Scale );
	int x, y, z;
};

class TestSystem : public ecs::SystemBase
{
public:
	using SystemBase::SystemBase;

	void onUpdate() override
	{
		foreach<Position, Rotation, Scale>( []( Position& position, Rotation& rotation, Scale& scale )
		{
			position.x += 2.0f;
			scale.x -= 1;
		} );
	}
};

void EcsTest( int numObject, int numUpdate )
{
	ecs::World world;

	TimeObserver time1;
	for ( auto i = 0; i < numObject; i++ )
	{
		auto entity = world.getEntityManager()->createEntity<Position, Rotation, Scale>();
		Position pos;
		pos.x = pos.y = pos.z = static_cast<float>( i );
		Rotation rot;
		rot.x = rot.y = rot.z = rot.w = 1;
		Scale scale;
		scale.x = scale.y = scale.z = 500;
		world.getEntityManager()->setComponentData( entity, pos );
		world.getEntityManager()->setComponentData( entity, rot );
		world.getEntityManager()->setComponentData( entity, scale );
	}
	world.addSystem<TestSystem>();
	printf( "%f,", time1.GetTimeFromStart() );

	time1 = TimeObserver();
	for ( auto i = 0; i < numUpdate; i++ )
	{
		world.update();
	}
	printf( "%f,", time1.GetTimeFromStart() );
}

void OutputWorldTest( int numObject, int numUpdate )
{
	ecs::World world;

	for ( auto i = 0; i < numObject; i++ )
	{
		auto entity = world.getEntityManager()->createEntity<Position, Rotation, Scale>();
		Position pos;
		pos.x = pos.y = pos.z = static_cast<float>( i );
		Rotation rot;
		rot.x = rot.y = rot.z = rot.w = 1;
		Scale scale;
		scale.x = scale.y = scale.z = 0;
		world.getEntityManager()->setComponentData( entity, pos );
		world.getEntityManager()->setComponentData( entity, rot );
		world.getEntityManager()->setComponentData( entity, scale );
	}

	{
		ecs::World otherWorld;
		auto pEntityMgr = otherWorld.getEntityManager();
		for ( auto i = 0; i < numObject / 2; ++i )
		{
			auto entity = pEntityMgr->createEntity<Position, Rotation>();
			Position pos;
			pos.x = pos.y = pos.z = static_cast<float>( i );
			Rotation rot;
			rot.x = rot.y = rot.z = rot.w = 1;
			pEntityMgr->setComponentData( entity, pos );
			pEntityMgr->setComponentData( entity, rot );
		}
		for ( auto i = 0; i < numObject / 2; ++i )
		{
			auto entity = pEntityMgr->createEntity<Position, Rotation, Scale>();
			Position pos;
			pos.x = pos.y = pos.z = static_cast<float>( i );
			Rotation rot;
			rot.x = rot.y = rot.z = rot.w = 1;
			Scale scale;
			scale.x = scale.y = scale.z = 0;
			pEntityMgr->setComponentData( entity, pos );
			pEntityMgr->setComponentData( entity, rot );
			pEntityMgr->setComponentData( entity, scale );
		}
		world.marge( std::move( otherWorld ) );
	}

	world.addSystem<TestSystem>();
	for ( auto i = 0; i < numUpdate; i++ )
	{
		world.update();
	}

	const auto chunkList = world.getEntityManager()->getChunkList<Scale>();
	const auto pChunk = chunkList[0];
	printf( "OutputWorldTest numObject:%d Scale.X:%d\n", pChunk->getSize(), pChunk->getComponentArray<Scale>()[0].x );

	printf( "write start\n" );
	{
		BinaryStream binaryStream;
		world.writeBinaryStream( binaryStream );

		std::ofstream ofs( "world.bin", std::ios_base::binary );
		ofs.write( binaryStream.getBuffer(), binaryStream.getSize() );
	}
	printf( "write end\n" );
}

void InputWorldTest( const int numUpdate )
{
	ecs::World world;

	printf( "read start\n" );
	{
		std::ifstream ifs( "world.bin", std::ios_base::binary );
		ifs.seekg( 0, std::ios_base::end );
		const int fileSize = static_cast<const int>( ifs.tellg() );
		ifs.seekg( 0, std::ios_base::beg );
		const auto fileBuffer = std::make_unique<char[]>( fileSize );
		ifs.read( fileBuffer.get(), fileSize );
		BinaryStream binaryStream;
		binaryStream.write( fileBuffer.get(), fileSize );
		world.readBinaryStream( binaryStream );
	}
	printf( "read end\n" );

	world.addSystem<TestSystem>();
	for ( auto i = 0; i < numUpdate; i++ )
	{
		world.update();
	}

	printf( "InputWorldTest Scale.X:%d\n",
	        world.getEntityManager()->getChunkList<Scale>()[0]->getComponentArray<Scale>()[0].x );
}

class Component
{
public:
	virtual ~Component() = default;

	virtual void update()
	{
	}
};

class Transform : public Component
{
public:
	void update() override
	{
		posX += 2.0f;
		scaleX -= 1.0f;
	}

	float posX, posY, posZ;
	float rotX, rotY, rotZ, rotW;
	float scaleX, scaleY, scaleZ;
};

class GameObject
{
public:
	void update()
	{
		for ( auto&& pComponent : mpComponentList )
		{
			pComponent->update();
		}
	}

	std::vector<std::unique_ptr<Component>> mpComponentList;
};

void GoTest( int numObject, int numUpdate )
{
	std::vector<GameObject> gameObjects;
	TimeObserver time1;
	for ( auto i = 0; i < numObject; i++ )
	{
		GameObject go;
		auto pTrans = new Transform();
		pTrans->posX = pTrans->posY = pTrans->posZ = static_cast<float>( i );
		pTrans->rotX = pTrans->rotY = pTrans->rotZ = pTrans->rotW = 1;
		pTrans->scaleX = pTrans->scaleY = pTrans->scaleZ = 500.0f;

		go.mpComponentList.emplace_back( pTrans );
		gameObjects.push_back( std::move( go ) );
	}
	printf( "%f,", time1.GetTimeFromStart() );

	time1 = TimeObserver();
	for ( auto i = 0; i < numUpdate; i++ )
	{
		for ( auto&& gameObject : gameObjects )
		{
			gameObject.update();
		}
	}
	printf( "%f,", time1.GetTimeFromStart() );
}

int main()
{
	for ( auto i = 1; i < 10'000; i += 100 )
	{
		TimeObserver time;
		EcsTest( i, 1000 );

		printf( "%f,", time.GetTimeFromStart() );

		time = TimeObserver();
		GoTest( i, 1000 );
		printf( "%f\n", time.GetTimeFromStart() );
	}

	OutputWorldTest( 1'000'000, 10 );
	InputWorldTest( 10 );
}
