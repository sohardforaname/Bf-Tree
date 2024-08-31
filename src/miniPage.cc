//
// Created by mchxyz_ucchi on 8/31/24.
//

#include "miniPage.hh"
#include <cstring>

namespace BFTree {

struct KeyComparator {
    bool operator()(const Key &key, const KVMeta &meta) {
        return static_cast<uint16_t>(key.first) < meta.lookAhead;
    }
};

const Record NullRecord = {NULL, NULL};

int MiniPage::getInternal(const Key &query) const {
    size_t l = 0, r = this->kvMetas.size() - 1;
    while (l < r) {
        size_t m = (l + r) >> 1;
        if (KeyComparator()(query, this->kvMetas[m])) {
            r = m - 1;
        } else {
            l = m;
        }
    }
    // we query the kv by the first two bytes, it may get more than one answer.
    for (size_t pos = l; pos < this->kvMetas.size(); ++pos) {
        const KVMeta &curKV = this->kvMetas[pos].keyOffset;
        int res = memcmp(this->data[curKV.keyOffset], query.first, std::min(curKV.keyLen, query.second));
        if (res == 0) {
            return pos;
        } else if (res > 0) {
            break;
        }
    }
    return -1;
}

Record MiniPage::get(const Key &query) const {
    int idx = getInternal(query);
    [[unlikely]] if (idx == -1) {
        return NullRecord;
    } else {
        const KVMeta &kvMeta = this->kvMetas[idx];
        return {kvMeta, this->data[kvMeta.valueOffset]};
    }
}

std::vector <Record> MiniPage::rangeScan(BFTree::Key begin, BFTree::Key end) {

}

Record MiniPage

}