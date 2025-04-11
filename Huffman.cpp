#include <vector>
#include <algorithm>
#include <queue>
#include <iostream>
#include <string>
#include <fstream>
#include <memory> // Use smart pointers for better memory management
//This is for the C++ image library im using
#include "bitmap-cpp/bitmap.h"

// Node structure for the tree
template <typename T>
struct Node {
  int occurrences = 0, weight = 0;
  std::shared_ptr<Node> leftChild = nullptr, rightChild = nullptr, parent = nullptr;
  bool isLeft = false;
  std::vector<T> data;
};

// Function to initialize the tree
template <typename T>
std::shared_ptr<Node<T>> initialize_tree(T type) {
  auto start = std::make_shared<Node<T>>();
  start->data = {};
  start->weight = 100000000;
  start->isLeft = false;
  return start;
}

// Function to find a leaf node by term
template <typename T>
std::shared_ptr<Node<T>> find_leaf_by_term(std::shared_ptr<Node<T>> root,
                                           std::shared_ptr<Node<T>>& dictNode,
                                           T term) {
  std::queue<std::shared_ptr<Node<T>>> q;
  q.push(root);

  while (!q.empty()) {
    auto current = q.front();
    q.pop();

    if (!current->data.empty() && std::find(current->data.begin(), current->data.end(), term) != current->data.end())
      return current;

    if (current->leftChild) q.push(current->leftChild);
    if (current->rightChild) q.push(current->rightChild);
  }
  return dictNode;
}

// Function to find a leaf node by occurrences
template <typename T>
std::shared_ptr<Node<T>> find_leaf_by_occurrences(std::shared_ptr<Node<T>> root, int occurrences) {
  std::queue<std::shared_ptr<Node<T>>> q;
  q.push(root);

  while (!q.empty()) {
    auto current = q.front();
    q.pop();

    if (current->occurrences == occurrences) return current;

    if (current->leftChild) q.push(current->leftChild);
    if (current->rightChild) q.push(current->rightChild);
  }
  return nullptr;
}

// Function to update the weight of a node
template <typename T>
void update_weight(std::shared_ptr<Node<T>> node) {
  if (node->leftChild && node->rightChild)
    node->weight = node->leftChild->weight + node->rightChild->weight;
  else
    node->weight = node->occurrences * node->data.size();
}

// Function to swap children of a node
template <typename T>
void swap_children(std::shared_ptr<Node<T>> node) {
  std::swap(node->leftChild, node->rightChild);
}

// Function to remove a child from its parent
template <typename T>
void remove_child_from_parent(std::shared_ptr<Node<T>> child) {
  auto sibling = get_sibling(child);
  if (child->parent) {
    if (child->parent->leftChild == child) {
      child->parent->leftChild = nullptr;
    } else {
      child->parent->rightChild = nullptr;
    }
  }
  sibling->parent = child->parent->parent;

  if (child->parent->parent->leftChild == child->parent) {
    child->parent->parent->leftChild = sibling;
  } else {
    child->parent->parent->rightChild = sibling;
  }
}

// Function to get the sibling of a node
template <typename T>
std::shared_ptr<Node<T>> get_sibling(std::shared_ptr<Node<T>> node) {
  if (!node->parent) return nullptr;
  return (node->parent->leftChild == node) ? node->parent->rightChild : node->parent->leftChild;
}

template <typename T>
void exchange_nodes(std::shared_ptr<Node<T>> leftNode, std::shared_ptr<Node<T>> rightNode) {
  if (!leftNode || !rightNode || leftNode == rightNode) return; // Safety check

  std::shared_ptr<Node<T>> leftParent = leftNode->parent;
  std::shared_ptr<Node<T>> rightParent = rightNode->parent;

  if (!leftParent || !rightParent) return; // Ensure nodes are not root

  // Check if one node is a direct parent of the other
  bool leftIsParent = (leftNode == rightParent);
  bool rightIsParent = (rightNode == leftParent);

  // Standard swap (if they are NOT direct parent-child)
  bool isLeftLeftChild = (leftParent->leftChild == leftNode);
  bool isRightLeftChild = (rightParent->leftChild == rightNode);

  leftNode->parent = rightParent;
  rightNode->parent = leftParent;

  if (isLeftLeftChild)
    leftParent->leftChild = rightNode;
  else
    leftParent->rightChild = rightNode;

  if (isRightLeftChild)
    rightParent->leftChild = leftNode;
  else
    rightParent->rightChild = leftNode;

  // Update children’s parent pointers
  if (leftNode->leftChild) leftNode->leftChild->parent = leftNode;
  if (leftNode->rightChild) leftNode->rightChild->parent = leftNode;
  
  if (rightNode->leftChild) rightNode->leftChild->parent = rightNode;
  if (rightNode->rightChild) rightNode->rightChild->parent = rightNode;
}

template <typename T>
void shift_up(std::shared_ptr<Node<T>> root, std::shared_ptr<Node<T>> node){
  std::shared_ptr<Node<T>> currNode = node;
  while(currNode != nullptr){
    if(currNode->leftChild != nullptr && currNode->rightChild != nullptr)
      currNode->weight = currNode->leftChild->weight + currNode->rightChild->weight;
    else
      currNode->weight = currNode->occurrences*currNode->data.size();
    
    if(currNode->parent != nullptr && currNode->weight > get_sibling(currNode)->weight+1){
      if(currNode->parent->parent != nullptr && currNode->weight > get_sibling(currNode->parent)->weight){
        std::shared_ptr<Node<T>> q = currNode->parent->parent;
        std::shared_ptr<Node<T>> oldParent = currNode->parent;
        exchange_nodes(currNode, get_sibling(currNode->parent));
        swap_children(q);
        update_weight(oldParent);
      }
    }
    
    currNode = currNode->parent;
  }
}

template <typename T>
std::vector<bool> toBinary(T n)
{
    std::vector<bool> r;
    while(n!=0) {
      r.insert(r.begin(), n%2==0 ?0:1);
      n/=2;
    };
    return r;
}

template <typename T>
std::vector<bool> nodePrefix(std::shared_ptr<Node<T>> node, std::shared_ptr<Node<T>> root){
  std::shared_ptr<Node<T>> currNode = node;
  std::vector<bool> prefix = {};
  while(currNode != root){
    if(currNode->isLeft)
      prefix.insert(prefix.begin(), 0);
    else
      prefix.insert(prefix.begin(), 1);
    currNode = currNode->parent;
  }
  return prefix;
}

// Function to update the tree
template <typename T>
std::vector<bool> update_tree(std::shared_ptr<Node<T>>& root, std::shared_ptr<Node<T>>& dictNode, T nextTerm) {
  std::vector<bool> resultCode;
  std::shared_ptr<Node<T>> p = find_leaf_by_term(root, dictNode, nextTerm);
  std::shared_ptr<Node<T>> q = find_leaf_by_occurrences(root, p->occurrences + 1);

  if(p != dictNode){
    std::vector<bool> prefix = nodePrefix(p, root);
    std::vector<bool> dataBin = {};
    if(p->data.size() != 1)
      std::vector<bool> dataBin = toBinary(p->data.size() - 1);
    resultCode.insert(resultCode.end(), prefix.begin(), prefix.end());
    resultCode.insert(resultCode.end(), dataBin.begin(), dataBin.end());
  } else {
    std::vector<bool> prefix = nodePrefix(p, root);
    std::vector<bool> dataBin = toBinary(nextTerm);
    resultCode.insert(resultCode.end(), prefix.begin(), prefix.end());
    resultCode.insert(resultCode.end(), dataBin.begin(), dataBin.end());
  }

  if (q) {
    if(p != dictNode){
      p->data.erase(std::remove(p->data.begin(), p->data.end(), nextTerm), p->data.end());
      p->weight -= p->occurrences;
    }
    q->data.push_back(nextTerm);
    q->weight += q->occurrences;

    shift_up(root, q);

    if(p!= dictNode){
      if (p->data.empty() && p != dictNode)
        remove_child_from_parent(p);
      else if (auto sibling = get_sibling(p))
        shift_up(root, sibling);
    }
  } else {
    auto newNode = std::make_shared<Node<T>>();
    auto newClass = std::make_shared<Node<T>>();

    newNode->leftChild = p;
    newNode->rightChild = newClass;
    newNode->parent = p->parent;
    newClass->parent = newNode;

    newClass->occurrences = p->occurrences + 1;
    newClass->weight = newClass->occurrences;
    newClass->data.push_back(nextTerm);

    if (p->parent) {
      if (p->parent->leftChild == p)
        p->parent->leftChild = newNode;
      else
        p->parent->rightChild = newNode;
    } else {
      root = newNode;
    }

    p->parent = newNode;
    if(p != dictNode)
      p->data.erase(std::remove(p->data.begin(), p->data.end(), nextTerm), p->data.end());
    p->weight -= p->occurrences;

    newNode->weight = newClass->weight + p->weight;

    if (p->data.empty() && p != dictNode)
      remove_child_from_parent(p);
    else
      shift_up(root, get_sibling(p));

    shift_up(root, newNode);
  }
  return resultCode;

}

// Function to print the tree
template <typename T>
void print_tree(std::shared_ptr<Node<T>> root) {
  std::queue<std::shared_ptr<Node<T>>> q;
  q.push(root);

  while (!q.empty()) {
    auto current = q.front();
    q.pop();

    std::cout << "Occurrences: " << current->occurrences;
    std::cout << ", Weight: " << current->weight << std::endl << std::flush;
    for(auto itr : current->data){
      std::cout << itr << " ";
    }
    std::cout << std::endl << std::flush;

    if (current->leftChild) q.push(current->leftChild);
    if (current->rightChild) q.push(current->rightChild);
  }
}

template <typename T>
void printBT(const std::string& prefix, const std::shared_ptr<Node<T>> node, bool isLeft)
{
    if( node != nullptr )
    {
        std::cout << prefix;

        std::cout << (isLeft ? "├──" : "└──" );

        // print the value of the node
        std::cout << node->weight << std::endl;

        // enter the next tree level - left and right branch
        printBT( prefix + (isLeft ? "│   " : "    "), node->leftChild, true);
        printBT( prefix + (isLeft ? "│   " : "    "), node->rightChild, false);
    }
}

template <typename T>
void printBT(const std::shared_ptr<Node<T>> node)
{
    printBT("", node, false);    
}

int main() {
  // rewrote so dictionary is no longer neccessary
  //std::vector<int> dictionary = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  // 1, 2, 5 are 3 occurrences
  // 0, 3, 4 are 1 occurrence
  // everything else is 0
  //std::vector<int> data = {1,2,5,2,4,1,2,5,3,5,0,1};
  //std::vector<int> data = {7,7,7,7,7,7,7,7,7,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,3,3,3,3,3,3,3,3,3,3,3};

  // everything occurs once
  //std::vector<int> data = {0,1,2,3,4,5,6,7,8,9};
  //std::vector<int> data = {1, 2, 5, 2};

  // this stuff is for the C++ image library im using

  //auto tree = initialize_tree(data[0]);
  //auto dictNode = tree;

  /*
  for (auto value : data) {
    update_tree(tree, dictNode, value);
    print_tree(tree);
    std::cout << std::endl << std::flush;
  }
  */
  std::vector<std::vector<int>> data = bmp::readBitmap("lichtenstein.bmp");

  //std::vector<int> data = {1,2,4,5,9,1,2,5,6,7,5,2,8,0};

  auto tree = initialize_tree(data[0][0]);
  auto dictNode = tree;

  //std::string code = "";
  std::vector<bool> code = {};

  for(auto x : data){
    for(auto y : x){
      std::vector<bool> temp = update_tree(tree, dictNode, y);
      code.insert(code.end(), temp.begin(), temp.end());
    }
  }

  printBT(tree);

  //std::cout << "Image Size: " << data.size() << "x" << data[0].size() << std::endl << std::flush;
  //
  std::ofstream out("out.txt");
  std::streambuf *coutbuf = std::cout.rdbuf();
  std::cout.rdbuf(out.rdbuf());

  for(auto i : code)
    std::cout << i;

  std::cout << std::endl << std::flush;


  std::cout.rdbuf(coutbuf);
  std::cout << code.size() << std::endl << std::flush;

  //stbi_image_free(rgb_image);
}
