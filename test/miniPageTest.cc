//
// Created by mchxyz_ucchi on 8/31/24.
//

#include <gtest/gtest.h>

#include <cstring>

#include "miniPage.hh"

TEST(miniPageTest, testCreate) {
  unsigned char buffer[64];
  BFTree::MiniPage page(buffer);
  BFTree::PageMeta *meta = reinterpret_cast<BFTree::PageMeta *>(page.buffer);
  EXPECT_EQ(meta->pageType, BFTree::MINIPAGE);
  EXPECT_EQ(meta->recordCount, 0);
  EXPECT_EQ(meta->size, 64);
  EXPECT_EQ(meta->splitFlag, false);
}

TEST(miniPageTest, testInsert) {
  unsigned char buffer[64];
  BFTree::MiniPage page(buffer);
  BFTree::Key key{(unsigned char *)("a"), 1};
  BFTree::Value val{(unsigned char *)("b"), 1};
  page.insert(key, val);

  const BFTree::PageMeta *meta =
      reinterpret_cast<BFTree::PageMeta *>(page.buffer);
  EXPECT_EQ(meta->recordCount, 1);

  const BFTree::KVMeta *kvMeta =
      reinterpret_cast<BFTree::KVMeta *>(page.buffer + BFTree::kvMetaBegin);
  EXPECT_EQ(kvMeta->keySize, 1);
  EXPECT_EQ(kvMeta->valueSize, 1);
  EXPECT_EQ(kvMeta->offset, 2);

  BFTree::Key key2{(unsigned char *)("xyz"), 3};
  BFTree::Value val2{(unsigned char *)("abcd"), 3};
  page.insert(key2, val2);

  EXPECT_EQ(meta->recordCount, 2);

  kvMeta = reinterpret_cast<BFTree::KVMeta *>(
      page.buffer + BFTree::kvMetaBegin + sizeof(BFTree::KVMeta) * 1);
  EXPECT_EQ(kvMeta->keySize, 3);
  EXPECT_EQ(kvMeta->valueSize, 3);
  EXPECT_EQ(kvMeta->offset, 8);
}

TEST(miniPageTest, testErase) {
  BFTree::Key keys[3] = {{(unsigned char *)("abc"), 3},
                         {(unsigned char *)("xyz"), 2},
                         {(unsigned char *)("cfxd"), 3}};
  BFTree::Value vals[3] = {{(unsigned char *)("def"), 3},
                           {(unsigned char *)("zyx"), 2},
                           {(unsigned char *)("dxc"), 3}};
  unsigned char buffer[64];
  BFTree::MiniPage page(buffer);
  for (int i = 0; i < 3; ++i) {
    page.insert(keys[i], vals[i]);
  }

  page.erase({(unsigned char *)("zyx"), 2});
  page.erase({(unsigned char *)("xyz"), 2});

  const BFTree::PageMeta *meta =
      reinterpret_cast<BFTree::PageMeta *>(page.buffer);

  EXPECT_EQ(meta->recordCount, 2);

  const BFTree::KVMeta *kvMeta = reinterpret_cast<BFTree::KVMeta *>(
      page.buffer + BFTree::kvMetaBegin + sizeof(BFTree::KVMeta) * 1);
  EXPECT_EQ(kvMeta->keySize, 3);
  EXPECT_EQ(kvMeta->valueSize, 3);
  EXPECT_EQ(kvMeta->offset, 16);
}

TEST(miniPageTest, testRangeScan) {
  BFTree::Key keys[3] = {{(unsigned char *)("abc"), 3},
                         {(unsigned char *)("xyz"), 2},
                         {(unsigned char *)("cfxd"), 3}};
  BFTree::Value vals[3] = {{(unsigned char *)("def"), 3},
                           {(unsigned char *)("zyx"), 2},
                           {(unsigned char *)("dxc"), 3}};
  unsigned char buffer[64];
  BFTree::MiniPage page(buffer);
  for (int i = 0; i < 3; ++i) {
    page.insert(keys[i], vals[i]);
  }

  auto res = page.rangeScan({(unsigned char *)("aa"), 2},
                            {(unsigned char *)("ff"), 2});

  EXPECT_EQ(res.size(), 2);
  EXPECT_TRUE(memcmp(res[0].first, page.buffer + BFTree::kvMetaBegin,
                     sizeof(BFTree::KVMeta)) == 0);
  EXPECT_TRUE(memcmp(res[1].first,
                     page.buffer + BFTree::kvMetaBegin + sizeof(BFTree::KVMeta),
                     sizeof(BFTree::KVMeta)) == 0);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}