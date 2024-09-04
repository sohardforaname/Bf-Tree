//
// Created by mchxyz_ucchi on 8/31/24.
//

#include <sys/types.h>

#include <cstdint>

#include "miniPage.hh"

#ifndef BF_TREE_RINGBUFFER_HH
#define BF_TREE_RINGBUFFER_HH

namespace BFTree {

const size_t BUFFERSIZE = 1 << 28;

const uint8_t MINIPAGE_READY = 0x0;
const uint8_t MINIPAGE_FREELIST = 0x1;

struct MiniPageHeader {
  uint8_t status;
  uint16_t size;
  uint32_t pad;

  MiniPageHeader(uint16_t size) : status(MINIPAGE_READY), size(size) {}
};

struct BufferedMiniPage {
  MiniPageHeader header;
  unsigned char* content;
};

struct RingBuffer {
  unsigned char* bufferPtr;
  size_t bufferSize = BUFFERSIZE;

  size_t headerPtr = 0;
  size_t secondOffset = 0;
  size_t tailPtr = 0;

  ssize_t freeList[6] = {-1};

  RingBuffer() : bufferPtr((unsigned char*)(malloc(BUFFERSIZE))) {}

  MiniPage* allocate(size_t pageSize);
  void deallocate(MiniPage* miniPage);
  void mergeFragment();
  void evict();
};

}  // namespace BFTree

#endif  // BF_TREE_RINGBUFFER_HH