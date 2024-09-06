#include "storage.hh"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstddef>
#include <cstdio>
#include <cstring>

namespace BFTree {

Table* readFile(const char* path) {
  int fd = open64(path, O_CREAT | O_RDWR);
  if (fd == -1) {
    return nullptr;
  }

  struct stat stat_buf;
  int rc = stat(path, &stat_buf);
  if (rc == -1) {
    return nullptr;
  }

  Table* table = new Table();
  table->fileNo = fd;
  table->fileSize = stat_buf.st_size;
  table->blockNum = table->fileSize >> 12;
  fileMap.emplace(table->fileNo, table);

  return table;
}

Block* readBlock(Table* table, size_t blockId) {
  size_t readPtr = blockId * (1 << 12);
  if (lseek64(table->fileNo, readPtr, SEEK_SET) == (off_t)(-1)) {
    return nullptr;
  }

  Page page = (Page)malloc(PAGE_SIZE);
  if (read(table->fileNo, page, PAGE_SIZE) == -1) {
    return nullptr;
  }
  Block* block = new Block();
  block->blockNo = blockId;
  block->fileNo = table->fileNo;
  block->pageNo = blockId;
  block->page = page;
  table->blocks.push_back(block);
  return block;
}

int flushToFile(const Table* table, const Block* block) {
  size_t writePtr = block->pageNo * (1 << 12);
  if (lseek64(table->fileNo, writePtr, SEEK_SET) == (off_t)(-1)) {
    return -1;
  }
  int rc = write(table->fileNo, block->page, PAGE_SIZE);
  return rc;
}

int mergeMiniPage(MiniPage* miniPage, Page dataPage) {
  // we assume that kvs in the leaf page is sorted.
  // calculate the space for merge, if there's not enough space for merging
  // the leaf page should split.

  
}

}  // namespace BFTree