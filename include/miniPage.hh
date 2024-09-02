//
// Created by mchxyz_ucchi on 8/31/24.
//

#include <cstdint>
#include <string>
#include <sys/types.h>
#include <vector>

#ifndef BF_TREE_MINIPAGE_HH
#define BF_TREE_MINIPAGE_HH

namespace BFTree {

struct KVMeta;

using DataType = unsigned char *;
using Record = std::pair<const KVMeta *, const unsigned char *>;
using Key = std::pair<const DataType, size_t>;
using Value = Key;

const size_t KV_INSERT = 0x00;
const size_t KV_CACHED = 0x01;
const size_t KV_DELETED = 0x10;
const size_t KV_RESERVED = 0x11;

struct PageMeta {
  uint16_t size;
  uint8_t pageType;
  uint8_t splitFlag;
  uint16_t recordCount;
  unsigned char leafPad[6] = {0};

  PageMeta(int pageType)
      : size(0), pageType(pageType), splitFlag(false), recordCount(0) {}
};

struct KVMeta {
  uint16_t keySize : 14;
  uint16_t valueSize : 14;
  uint16_t offset : 16;
  int type : 2;
  bool fenceKey : 1;
  bool reference : 1;
  uint16_t lookAhead : 16;

  KVMeta(const Key &key, const Value &val)
      : keySize(key.second), valueSize(val.second), type(KV_INSERT),
        fenceKey(false), reference(false),
        lookAhead(*reinterpret_cast<uint16_t *>(key.first)) {}
};

const int MINIPAGE = 0x0;
const int LEAFPAGE = 0x1;

const size_t kvMetaBegin = sizeof(PageMeta);

struct MiniPage {
  size_t bufferSize;
  size_t recordOffset;
  unsigned char *buffer;

private:
  const PageMeta *getPageMeta() const;
  PageMeta *getPageMetaMutable();
  const KVMeta *getKVMeta(size_t idx) const;
  size_t getRecordCount() const;

  bool isFull(size_t kvSize) const;
  bool extendBuffer();
  unsigned char *getKeyPtr(const KVMeta *kvMeta) const;
  unsigned char *getValuePtr(const KVMeta *kvMeta) const;

  size_t getGreaterEqualIndex(const Key &key) const;
  size_t getEqualIndex(const Key &key) const;

public:
  MiniPage()
      : bufferSize(64), recordOffset(0),
        buffer(static_cast<unsigned char *>(
            malloc(sizeof(unsigned char) * bufferSize))) {
    new (buffer) PageMeta(MINIPAGE);
  }

  Record get(const Key &key) const;
  std::vector<Record> rangeScan(const Key &begin, const Key &end) const;
  void insert(const Key &key, const Value &val);
  void erase(const Key &key);
};

} // namespace BFTree

#endif // BF_TREE_MINIPAGE_HH
