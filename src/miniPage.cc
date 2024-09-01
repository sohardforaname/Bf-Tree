//
// Created by mchxyz_ucchi on 8/31/24.
//

#include <algorithm>
#include <cstring>

#include "miniPage.hh"

namespace BFTree {

struct KeyComparator {
    int operator()(const Key &key, const KVMeta &meta) {
        return memcmp(key.first, &meta.lookAhead, sizeof(u_int16_t));
    }
};

const Record NullRecord = {NULL, NULL};

// get the first record index of kvMeta greater equal than given key
// returning kvMeta's size means the key is greater than the keys in the miniPage.
size_t MiniPage::getGreaterEqualIndex(const BFTree::Key &key) const {
    size_t l = 0, r = this->kvMetas.size() - 1;
    while (l < r) {
        size_t m = (l + r) >> 1;
        if (KeyComparator()(key, this->kvMetas[m]) < 0) {
            r = m - 1;
        } else {
            l = m;
        }
    }
    if (l == this->kvMetas.size() - 1 && KeyComparator()(key, this->kvMetas[l]) > 0) {
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
    for (size_t pos = idx; pos < this->kvMetas.size(); ++pos) {
        const KVMeta &curKV = this->kvMetas[pos];
        int res = memcmp(&this->data[curKV.keyOffset], query.first,
                         std::min(static_cast<size_t>(curKV.keyLen), query.second));
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
    [[unlikely]] if (idx == -1) {
        return NullRecord;
    } else {
        const KVMeta &kvMeta = this->kvMetas[idx];
        return {&kvMeta, &this->data[kvMeta.valueOffset]};
    }
}

// kvs in miniPage is sorted, so we do sort when inserting.
void MiniPage::insert(const Key &key, const Value &val) {
    KVMeta kvMeta{
            static_cast<uint32_t>(key.second), static_cast<uint32_t>(val.second),
            0, 0,
            INSERT, false, false,
            *reinterpret_cast<uint16_t *>(key.first)
    };


    this->kvMetas.emplace_back();
}


void MiniPage::erase(const Key &key) {

}

std::vector<Record> MiniPage::rangeScan(const Key &begin, const Key &end) const {
    size_t beginIndex = getGreaterEqualIndex(begin);
    size_t endIndex = getGreaterEqualIndex(end);

    // if beginIndex is -1, it returns an empty vector.
    if (beginIndex == -1) {
        return {};
    }

    // if endIndex is -1, all keys in the miniPage are less than the end, we set it to kvMeta.size() - 1.
    if (endIndex == -1) {
        endIndex = this->kvMetas.size() - 1;
    }

    // endIndex may be greater than the given end key, if true, we minus it by 1.
    if (KeyComparator()(end, this->kvMetas[endIndex]) < 0) {
        --endIndex;
    }

    std::vector<Record> res(endIndex - beginIndex + 1);

    for (size_t pos = beginIndex; pos <= endIndex; ++pos) {
        const KVMeta &kvMeta = this->kvMetas[pos];
        res.emplace_back(&kvMeta, &this->data[kvMeta.valueOffset]);
    }

    return res;
}

} // namespace BFTree