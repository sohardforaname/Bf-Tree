//
// Created by mchxyz_ucchi on 8/31/24.
//

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "miniPage.hh"

namespace BFTree {

struct KeyComparator {
  int operator()(const Key &key, const KVMeta &meta) {
    uint16_t tmpLookAhead = meta.lookAhead;
    return memcmp(key.first, &tmpLookAhead, sizeof(u_int16_t));
  }
};

const Record NullRecord = {nullptr, nullptr};

PageMeta *MiniPage::getPageMetaMutable() {
  return reinterpret_cast<PageMeta *>(buffer);
}

const PageMeta *MiniPage::getPageMeta() const {
  return reinterpret_cast<const PageMeta *>(buffer);
};

const KVMeta *MiniPage::getKVMeta(size_t idx) const {
  return reinterpret_cast<const KVMeta *>(buffer + kvMetaBegin +
                                          sizeof(KVMeta) * idx);
}

size_t MiniPage::getRecordCount() const { return getPageMeta()->recordCount; }

unsigned char *MiniPage::getKeyPtr(const KVMeta *kvMeta) const {
  return buffer + bufferSize - kvMeta->offset;
};

unsigned char *MiniPage::getValuePtr(const KVMeta *kvMeta) const {
  return buffer + bufferSize - kvMeta->offset + kvMeta->keySize;
}

// get the first record index of kvMeta greater equal than given key
// returning kvMeta's size means the key is greater than the keys in the
// miniPage.
size_t MiniPage::getGreaterEqualIndex(const BFTree::Key &key) const {
  size_t l = 0, r = this->getRecordCount();
  while (l < r) {
    size_t m = (l + r) >> 1;
    if (KeyComparator()(key, *this->getKVMeta(m)) <= 0) {
      r = m;
    } else {
      l = m + 1;
    }
  }
  if (l == this->getPageMeta()->recordCount) {
    return -1;
  }
  return l;
}

size_t MiniPage::getEqualIndex(const Key &key) const {
  size_t idx = getGreaterEqualIndex(key);
  if (idx == -1) {
    return -1;
  }
  // we query the kv by the first two bytes, it may get more than one answer.
  for (size_t pos = idx; pos < this->getRecordCount(); ++pos) {
    const KVMeta *curKV = this->getKVMeta(pos);
    int res = memcmp(getKeyPtr(curKV), key.first,
                     std::min(static_cast<size_t>(curKV->keySize), key.second));
    if (res == 0) {
      return pos;
    } else if (res > 0) {
      break;
    }
  }
  return -1;
}

Record MiniPage::get(const Key &key) const {
  size_t idx = getEqualIndex(key);
  if (idx == -1) {
    return NullRecord;
  } else {
    const KVMeta *kvMeta = this->getKVMeta(idx);
    return {kvMeta, getValuePtr(kvMeta)};
  }
}

bool MiniPage::extendBuffer() {
  this->buffer = static_cast<unsigned char *>(
      std::realloc(this->buffer, this->bufferSize << 1));
  return this->buffer != nullptr;
}

bool MiniPage::isFull(size_t kvSize) const {
  return kvMetaBegin + sizeof(KVMeta) * (getRecordCount() + 1) + kvSize >
         bufferSize - recordOffset;
}

// kvs in miniPage is sorted, so we do sort when inserting.
void MiniPage::insert(const Key &key, const Value &val) {
  if (isFull(key.second + val.second)) {
    this->extendBuffer();
  }
  KVMeta *newKVMeta =
      new (buffer + kvMetaBegin + sizeof(KVMeta) * getRecordCount())
          KVMeta(key, val);

  newKVMeta->offset = this->recordOffset + key.second + val.second;
  getPageMetaMutable()->recordCount++;

  memcpy(getKeyPtr(newKVMeta), key.first, key.second);
  memcpy(getValuePtr(newKVMeta), val.first, val.second);

  this->recordOffset += key.second + val.second;
}

void MiniPage::erase(const Key &key) {
  size_t idx = getEqualIndex(key);
  if (idx == -1) {
    return;
  }
  uint16_t *recordCount = &getPageMetaMutable()->recordCount;
  memmove((void *)(getKVMeta(idx)), (void *)(getKVMeta(idx + 1)),
          sizeof(KVMeta) * (0ul + *recordCount - idx - 1));

  --(*recordCount);
}

std::vector<Record> MiniPage::rangeScan(const Key &begin,
                                        const Key &end) const {
  size_t beginIndex = getGreaterEqualIndex(begin);
  size_t endIndex = getGreaterEqualIndex(end);

  // if beginIndex is -1, it returns an empty vector.
  if (beginIndex == -1) {
    return {};
  }

  // if endIndex is -1, all keys in the miniPage are less than the end, we set
  // it to kvMeta.size() - 1.
  if (endIndex == -1) {
    endIndex = this->getRecordCount() - 1;
  }

  // endIndex may be greater than the given end key, if true, we minus it by 1.
  if (KeyComparator()(end, *this->getKVMeta(endIndex)) < 0) {
    --endIndex;
  }

  std::vector<Record> res(endIndex - beginIndex + 1);

  for (size_t pos = beginIndex; pos <= endIndex; ++pos) {
    const KVMeta *kvMeta = this->getKVMeta(pos);
    res.emplace_back(kvMeta, getValuePtr(kvMeta));
  }

  return res;
}

} // namespace BFTree