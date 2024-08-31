//
// Created by mchxyz_ucchi on 8/31/24.
//

#include "miniPage.hh"

#ifndef BF_TREE_RINGBUFFER_HH
#define BF_TREE_RINGBUFFER_HH

namespace BFTree {

struct RingBuffer {
    unsigned char *bufferPtr;
    size_t secondOffset;
    size_t bufferSize;

    MiniPage* freeList;

    void allocate();
    void deallocate();
    void mergeFragment();
};

}

#endif //BF_TREE_RINGBUFFER_HH