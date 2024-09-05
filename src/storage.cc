#include "storage.hh"

#include <sys/stat.h>

#include <cstring>

namespace BFTree {

Table* readFile(const char* path) {
  FILE* file = fopen(path, "rb");
  if (!file) {
    return nullptr;
  }

  struct stat stat_buf;
  int rc = stat(path, &stat_buf);
  if (rc == -1) {
    return nullptr;
  }

  Table* table = new Table();
  table->path = strdup(path);
  table->fileNo = fileMap.size();
  table->fileSize = stat_buf.st_size;
  table->blockNum = table->fileSize >> 12;
  fileMap.emplace(table->fileNo, table);
}

Block* readBlock(const Table* table) { return nullptr; }

void flushToFile(const Table* table, const Block* block) {}

}  // namespace BFTree