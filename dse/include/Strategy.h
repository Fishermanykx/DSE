#include "z3++.h"

extern z3::context Ctx;
extern z3::solver Solver;

struct Node {
  z3::expr data;
  bool isRoot;
  Node *parent;
  Node *leftChild;
  Node *rightChild;

  Node(const z3::expr &expr, bool isRoot, Node *parent, Node *leftChild,
       Node *rightChild)
      : data(expr),
        isRoot(isRoot),
        parent(parent),
        leftChild(leftChild),
        rightChild(rightChild) {}
};

void searchStrategy(z3::expr_vector &OldVec);