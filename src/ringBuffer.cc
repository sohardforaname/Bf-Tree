#include "ringBuffer.hh"

namespace BFTree {

MiniPage* RingBuffer::allocate(size_t pageSize) {
  new (bufferPtr + tailPtr) MiniPageHeader(pageSize);
  tailPtr += sizeof(MiniPageHeader);
  unsigned char* pagePtr = bufferPtr + tailPtr;
  MiniPage* miniPage = new MiniPage(pagePtr);
  tailPtr += pageSize;
  return miniPage;
}

void RingBuffer::deallocate(MiniPage* miniPage) {
  MiniPageHeader* pageHeader =
      ((MiniPageHeader*)((unsigned char*)(miniPage)-8));
  pageHeader->status = MINIPAGE_FREELIST;
  pageHeader->pad = freeList[pageHeader->size >> 6];
  freeList[pageHeader->size >> 6] = (unsigned char*)(pageHeader)-bufferPtr;
  delete miniPage;
}

void evict() {}

}  // namespace BFTree