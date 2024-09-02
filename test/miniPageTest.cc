//
// Created by mchxyz_ucchi on 8/31/24.
//

#include "miniPage.hh"
#include <cstring>
#include <gtest/gtest.h>

TEST(miniPageTest, testCreate) {
  BFTree::MiniPage page;
  EXPECT_EQ(page.bufferSize, 64);
  EXPECT_EQ(page.recordOffset, 0);
  BFTree::PageMeta *meta = reinterpret_cast<BFTree::PageMeta *>(page.buffer);
  EXPECT_EQ(meta->pageType, BFTree::MINIPAGE);
  EXPECT_EQ(meta->recordCount, 0);
  EXPECT_EQ(meta->size, 0);
  EXPECT_EQ(meta->splitFlag, false);
}

TEST(miniPageTest, testInsert) {
  BFTree::MiniPage page;
  BFTree::Key key{(unsigned char *)("a"), 1};
  BFTree::Value val{(unsigned char *)("b"), 1};
  page.insert(key, val);

  const BFTree::PageMeta *meta =
      reinterpret_cast<BFTree::PageMeta *>(page.buffer);
  EXPECT_EQ(meta->recordCount, 1);
  EXPECT_EQ(page.recordOffset, 2);
  unsigned char *ptr = &page.buffer[page.bufferSize - page.recordOffset];
  EXPECT_TRUE(memcmp(ptr, "a", 1) == 0);
  EXPECT_TRUE(memcmp(ptr + 1, "b", 1) == 0);

  const BFTree::KVMeta *kvMeta =
      reinterpret_cast<BFTree::KVMeta *>(page.buffer + BFTree::kvMetaBegin);
  EXPECT_EQ(kvMeta->keySize, 1);
  EXPECT_EQ(kvMeta->valueSize, 1);
  EXPECT_EQ(kvMeta->offset, 2);

  BFTree::Key key2{(unsigned char *)("xyz"), 3};
  BFTree::Value val2{(unsigned char *)("abcd"), 3};
  page.insert(key2, val2);

  EXPECT_EQ(meta->recordCount, 2);
  EXPECT_EQ(page.recordOffset, 8);
  ptr = &page.buffer[page.bufferSize - page.recordOffset];
  EXPECT_TRUE(memcmp(ptr, "xyz", 3) == 0);
  EXPECT_TRUE(memcmp(ptr + 3, "abc", 3) == 0);
  EXPECT_TRUE(memcmp(ptr + 3, "abca", 4) == 0);

  kvMeta = reinterpret_cast<BFTree::KVMeta *>(
      page.buffer + BFTree::kvMetaBegin + sizeof(BFTree::KVMeta) * 1);
  EXPECT_EQ(kvMeta->keySize, 3);
  EXPECT_EQ(kvMeta->valueSize, 3);
  EXPECT_EQ(kvMeta->offset, 8);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}