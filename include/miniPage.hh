//
// Created by mchxyz_ucchi on 8/31/24.
//

#include <vector>
#include <string>

#ifndef BF_TREE_MINIPAGE_HH
#define BF_TREE_MINIPAGE_HH

namespace BFTree {

enum RecordType {
    INSERT, // 新插入的记录
    CACHE,  // 缓存的热点记录
    TOMBSTONE, // 被删除的记录
    PHANTOM // 负搜索结果
};

struct KVMeta {
    uint32_t keyLen;   // 键长度
    uint32_t valueLen; // 值长度
    uint32_t keyOffset; // 键偏移量
    uint32_t valueOffset; // 值偏移量
    RecordType type; // 记录类型
    bool fenceKey; // 是否为 fence key
    bool reference; // 引用标志
    uint16_t lookAhead; // 键的前两个字节
};

using Record = std::pair<const KVMeta*, unsigned char*>;
using DataType = unsigned char*;
using Key = std::pair<DataType, size_t>;
using Value = Key;

struct MiniPage {
    uint32_t size;        // MiniPage 大小
    bool splitFlag;       // 是否已满
    uint32_t valueCount;  // 记录数量
    std::vector <KVMeta> kvMetas; // KV 元数据数组
    std::vector<unsigned char> data; // 数据存储区

    int getInternal(const Key& query) const;

    Record get(const Key& query) const;
    std::vector<Record> rangeScan(const Key& begin, const Key& end) const;
    void insert(const Key& key, const Value& val);
    void erase(const Key& key);
};

}

#endif //BF_TREE_MINIPAGE_HH
