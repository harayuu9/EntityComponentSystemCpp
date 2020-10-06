//#include "ECS.h"
//
//struct HeapMemoryChecker
//{
//	static constexpr auto cMaxMemoryMap = 1'000'000;
//
//	HeapMemoryChecker()
//	{
//		memoryMap = static_cast<std::pair<void*, std::size_t>*>(malloc(
//			sizeof(std::pair<void*, std::size_t>) * cMaxMemoryMap));
//	}
//
//	~HeapMemoryChecker()
//	{
//		std::free(memoryMap);
//	}
//
//	void* alloc(const std::size_t size)
//	{
//		auto mem = std::malloc(size);
//		memoryMap[mapSize] = { mem, size };
//		++mapSize;
//		return mem;
//	}
//
//	void free(void* ptr)
//	{
//		for (std::size_t i = 0; i < mapSize; i++)
//		{
//			if (memoryMap[i].first != ptr)
//				continue;
//
//			std::free(memoryMap[i].first);
//			--mapSize;
//			const auto afterSize = mapSize - i;
//			if (afterSize > 0)
//				std::memmove(&memoryMap[i], &memoryMap[i + 1], sizeof(std::pair<void*, std::size_t>) * afterSize);
//			return;
//		}
//
//		assert(false);
//	}
//
//	std::pair<void*, std::size_t>* memoryMap;
//	std::size_t mapSize = 0;
//};
//
//HeapMemoryChecker memoryChecker;
//
//[[nodiscard]] void* operator new(const std::size_t size)
//{
//	return memoryChecker.alloc(size);
//}
//
//[[nodiscard]] void* operator new[](const std::size_t size)
//{
//	return memoryChecker.alloc(size);
//}
//
//void operator delete(void* ptr)
//{
//	memoryChecker.free(ptr);
//}
//
//void operator delete[](void* ptr)
//{
//	memoryChecker.free(ptr);
//}
//
//void memoryDump()
//{
//	std::size_t numMemory = 0;
//	for (auto i = 0; i < memoryChecker.mapSize; ++i)
//	{
//		numMemory += memoryChecker.memoryMap[i].second;
//	}
//
//	printf("[MemoryDump] use %llu byte, cnt %llu\n", numMemory, memoryChecker.mapSize);
//}
