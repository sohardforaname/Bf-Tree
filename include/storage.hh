#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

#ifndef BF_TREE_STORAGE_HH
#define BF_TREE_STORAGE_HH

namespace BFTree {

const size_t PAGE_SIZE = 1 << 12;

using Page = unsigned char[PAGE_SIZE];

struct Table {
  size_t fileNo;
  size_t fileSize;
  size_t blockNum;
  const char* path;
};

struct Block {
  size_t fileNo;
  size_t pageNo;
  size_t blockNo;
  Page* page;
};

extern std::unordered_map<size_t, Table*> fileMap;
extern std::unordered_map<size_t, Block*> blockMap;

extern Table* readFile(const char* path);
extern Block* readBlock(const Table* table);
extern void flushToFile(const Table* table, const Block* block);

}  // namespace BFTree

#endif