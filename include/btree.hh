//
// Created by mchxyz_ucchi on 8/31/24.
//

#include "miniPage.hh"
#include "storage.hh"

#ifndef BF_TREE_BTREE_HH
#define BF_TREE_BTREE_HH

namespace BFTree {

enum NodeType { LEAF_NODE, INNER_NODE };

struct Node {
  NodeType type;
  Node(NodeType type) : type(type) {}
};

struct LeafNode : public Node {
  size_t pageId;
  Page pageData;
};

struct InnerNode : public Node {
  std::vector<Node*> children;
};

struct BTree {
  std::vector<Node*> children;

  Record get(Key key);
  std::vector<Record> rangeScan(Key begin, Key end);
  void insert(Key key, Value val);
  void erase(Key key);
};

}  // namespace BFTree

#endif  // BF_TREE_BTREE_HH
