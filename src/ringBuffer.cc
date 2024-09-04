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

void RingBuffer::setFreeList(MiniPageHeader* pageHeader) {
  pageHeader->status = MINIPAGE_FREELIST;
  pageHeader->pad = freeList[pageHeader->size >> 6];
  freeList[pageHeader->size >> 6] = (unsigned char*)(pageHeader)-bufferPtr;
}

void RingBuffer::deallocate(MiniPage* miniPage) {
  setFreeList(((MiniPageHeader*)((unsigned char*)(miniPage)-8)));
  delete miniPage;
}

void RingBuffer::evictOneMiniPage() {
  MiniPageHeader* pageHeader = (MiniPageHeader*)(bufferPtr + headerPtr);
  size_t curMiniPageSize = sizeof(MiniPageHeader) + pageHeader->size;

  setFreeList(pageHeader);

  headerPtr += curMiniPageSize;
}

}  // namespace BFTree