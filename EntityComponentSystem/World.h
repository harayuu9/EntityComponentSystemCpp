#pragma once
#include "Chunk.h"

namespace ecs {
class EntityManager;
class SystemBase;

class World
{
	friend class EntityManager;
public:
	World();
	~World();
	[[nodiscard]] EntityManager* getEntityManager() const;

	void update();

	template<class T>
	void addSystem()
	{
		callDoCreate( mSystemList.emplace_back( new T( this ) ).get() );
		sortSystem();
	}

	/**
	 * \brief 他のワールドを結合する
	 * \details SystemListは結合しない
	 * \param other マージするワールド
	 */
	void marge(World&& other);
	
	void writeBinaryStream(BinaryStream& output);
	void readBinaryStream(BinaryStream& input);
	
private:
	void sortSystem();
	static void callDoCreate( SystemBase* pSystem );

	std::vector<Chunk> mChunkList;
	std::vector<std::unique_ptr<SystemBase>> mSystemList;
	std::unique_ptr<EntityManager> mpEntityManager;
};
}
