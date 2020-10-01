#include "ECS.h"

extern void memoryDump();

constexpr auto cNumObject = 100'000;
constexpr auto cNumUpdate = 5;

struct Position : ecs::IComponentData
{
	ECS_DECLARE_COMPONENT_DATA(Position);
	int x, y;
};

struct Scale : ecs::IComponentData
{
	ECS_DECLARE_COMPONENT_DATA(Scale);
	int value;
};

class TestSystem : public ecs::SystemBase
{
public:
	using SystemBase::SystemBase;

	void onCreate() override
	{
		for (auto i = 0; i < cNumObject; i++)
		{
			auto entity = getEntityManager()->createEntity<Position>();
			Position pos;
			pos.x = pos.y = i;
			getEntityManager()->setComponentData(entity, pos);
		}
		for (auto i = 0; i < cNumObject; i++)
		{
			Scale scale;
			scale.value = 500;

			ecs::Entity entity(0, 0);
			getEntityManager()->addComponentData(entity, scale);
		}
	}
	
	void onUpdate() override
	{
		foreach<Position, Scale>([](Position& position, Scale& scale)
			{
				position.x += 2;
				scale.value -= 1;
			});
	}
};


void EcsTest()
{
	ecs::World world;

	world.addSystem<TestSystem>();

	for (auto i = 0; i < cNumUpdate; i++)
	{
		world.update();
	}
	memoryDump();
}

class Component
{
public:
	virtual ~Component() = default;
	virtual void update() {}
};

class CPosition : public Component
{
public:
	void update() override
	{
		x += 2;
	}
private:
	int x = 0, y = 3;
};

class CScale : public Component
{
public:
	void update() override
	{
		value -= 1;
	}
private:
	int value = 1;
};

class GameObject
{
public:
	void update()
	{
		for (auto&& pComponent : mpComponentList)
		{
			pComponent->update();
		}
	}
	std::vector<std::unique_ptr<Component>> mpComponentList;
};

void GoTest()
{
	std::vector<GameObject> gameObjects;

	for (auto i = 0; i < cNumObject; i++)
	{
		GameObject go;
		go.mpComponentList.emplace_back(new CPosition());
		go.mpComponentList.emplace_back(new CScale());
		gameObjects.push_back(std::move(go));
	}

	for (auto i = 0; i < cNumUpdate; i++)
	{
		for (auto&& gameObject : gameObjects)
		{
			gameObject.update();
		}
	}
	memoryDump();
}

int main()
{
	printf("ƒƒ‚ƒŠÁ”ï—Ê‚Ì—˜_’l:%lld\n", (sizeof(Position) + sizeof(Scale)) * cNumObject);
	EcsTest();
	memoryDump();
	GoTest();
	memoryDump();
	getchar();
}
