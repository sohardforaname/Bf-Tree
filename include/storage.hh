#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

#include "miniPage.hh"

#ifndef BF_TREE_STORAGE_HH
#define BF_TREE_STORAGE_HH

namespace BFTree {

const size_t PAGE_SIZE = 1 << 12;

using Page = unsigned char*;

struct Block {
  size_t fileNo;
  size_t pageNo;
  size_t blockNo;
  Page page;
};

struct Table {
  size_t fileNo;
  size_t fileSize;
  size_t blockNum;
  std::vector<Block*> blocks;
};

extern std::unordered_map<size_t, Table*> fileMap;

extern Table* readFile(const char* path);
extern Block* readBlock(Table* table, size_t blockId);
extern int flushToFile(Table* table, const Block* block);

extern int mergeMiniPage(MiniPage* miniPage, Page dataPage);

}  // namespace BFTree

#endif