#include "Strategy.h"

#include <vector>

/*
 * Implement your search strategy.
 */
Node root = Node(Ctx.int_val(0), true, NULL, NULL, NULL);

bool equal(z3::expr expr1, z3::expr expr2) {
  Solver.reset();
  Solver.add(z3::implies(expr1, expr2));
  Solver.add(z3::implies(expr2, expr1));
  return Solver.check() == z3::sat;
}

// 使用 DFS
void searchStrategy(z3::expr_vector &OldVec) {
  Node *ptr = &root;

  for (z3::expr_vector::iterator it = OldVec.begin(); it != OldVec.end();
       ++it) {
    z3::expr curNode = *it;

    if (!ptr->leftChild) {
      z3::expr &prevCond = curNode;
      Node *left = new Node(prevCond, false, ptr, NULL, NULL);
      ptr->leftChild = left;
      ptr = left;
    } else if (equal(ptr->leftChild->data, curNode)) {
      ptr = ptr->leftChild;
    } else if (ptr->rightChild == NULL) {
      z3::expr &prevCond = curNode;
      Node *right = new Node(prevCond, false, ptr, NULL, NULL);
      ptr->rightChild = right;
      ptr = right;
    } else if (equal(ptr->rightChild->data, curNode)) {
      ptr = ptr->rightChild;
    }
  }

  if (!ptr->parent) {
    std::cout << "Search has done\n";

    while (!OldVec.empty()) {
      OldVec.pop_back();
    }

    return;
  }

  while (ptr->parent->rightChild) {
    ptr = ptr->parent;
    if (!ptr->parent) {
      std::cout << "Search has done\n";

      while (!OldVec.empty()) {
        OldVec.pop_back();
      }

      return;
    }
  }

  z3::expr rightCond = !(ptr->data);
  Node *newRightChild = new Node(rightCond, false, ptr->parent, NULL, NULL);
  ptr->parent->rightChild = newRightChild;
  ptr = newRightChild;

  std::vector<z3::expr> tmpVec;

  while (!ptr->isRoot) {
    tmpVec.push_back(ptr->data);
    ptr = ptr->parent;
  }
  while (!OldVec.empty()) {
    OldVec.pop_back();
  }
  while (!tmpVec.empty()) {
    std::vector<z3::expr>::iterator lastElem = tmpVec.end() - 1;
    OldVec.push_back(*lastElem);
    tmpVec.pop_back();
  }
}