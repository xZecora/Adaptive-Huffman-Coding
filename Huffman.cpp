#include <vector>
#include <algorithm>
#include <queue>
#include <iostream>
#include <memory> // Use smart pointers for better memory management

// Node structure for the tree
template <typename T>
struct Node {
  int occurrences = 0, weight = 10000000;
  std::shared_ptr<Node> leftChild = nullptr, rightChild = nullptr, parent = nullptr;
  std::vector<T> data;
};

// Function to initialize the tree
template <typename T>
std::shared_ptr<Node<T>> initialize_tree(const std::vector<T>& dictionary) {
  auto start = std::make_shared<Node<T>>();
  start->data = dictionary;
  return start;
}

// Function to find a leaf node by term
template <typename T>
std::shared_ptr<Node<T>> find_leaf_by_term(std::shared_ptr<Node<T>> root, T term) {
  std::queue<std::shared_ptr<Node<T>>> q;
  q.push(root);

  while (!q.empty()) {
    auto current = q.front();
    q.pop();

    if (!current->data.empty() && std::find(current->data.begin(), current->data.end(), term) == current->data.end())
      return current;

    if (current->leftChild) q.push(current->leftChild);
    if (current->rightChild) q.push(current->rightChild);
  }
  return root;
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
}

// Function to swap children of a node
template <typename T>
void swap_children(std::shared_ptr<Node<T>> node) {
  std::swap(node->leftChild, node->rightChild);
}

// Function to remove a child from its parent
template <typename T>
void remove_child_from_parent(std::shared_ptr<Node<T>> child) {
  if (child->parent) {
    if (child->parent->leftChild == child)
      child->parent->leftChild = nullptr;
    else
      child->parent->rightChild = nullptr;
  }
}

// Function to get the sibling of a node
template <typename T>
std::shared_ptr<Node<T>> get_sibling(std::shared_ptr<Node<T>> node) {
  if (!node->parent) return nullptr;
  return (node->parent->leftChild == node) ? node->parent->rightChild : node->parent->leftChild;
}

// Function to update the tree
template <typename T>
void update_tree(std::shared_ptr<Node<T>>& root, T nextTerm) {
  std::shared_ptr<Node<T>> p = find_leaf_by_term(root, nextTerm);
  auto q = find_leaf_by_occurrences(root, p->occurrences + 1);

  if (q) {
    p->data.erase(std::remove(p->data.begin(), p->data.end(), nextTerm), p->data.end());
    p->weight -= p->occurrences;
    q->data.push_back(nextTerm);
    q->weight += q->occurrences;

    if (p->data.empty())
      remove_child_from_parent(p);
    else if (auto sibling = get_sibling(p))
      update_weight(sibling);
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

    newNode->weight = newClass->weight + p->weight;

    if (p->parent) {
      if (p->parent->leftChild == p)
        p->parent->leftChild = newNode;
      else
        p->parent->rightChild = newNode;
    } else {
      root = newNode;
    }

    p->parent = newNode;
    p->data.erase(std::remove(p->data.begin(), p->data.end(), nextTerm), p->data.end());
    p->weight -= p->occurrences;

    if (p->data.empty())
      remove_child_from_parent(p);
  }
}

// Function to print the tree
template <typename T>
void print_tree(std::shared_ptr<Node<T>> root) {
  std::queue<std::shared_ptr<Node<T>>> q;
  q.push(root);

  while (!q.empty()) {
    auto current = q.front();
    q.pop();

    for(auto itr : current->data){
      std::cout << itr << " ";
    }
    std::cout << std::endl << std::flush;

    if (current->leftChild) q.push(current->leftChild);
    if (current->rightChild) q.push(current->rightChild);
  }
}

int main() {
  std::vector<int> dictionary = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  auto tree = initialize_tree(dictionary);
  std::vector<int> data = {1,2,5,2,4,1,2,5,3,5,0,1};
  //std::vector<int> data = {1, 2, 5, 2};

  for (int value : data) {
    update_tree(tree, value);
    print_tree(tree);
  }
}
