#include <vector>
#include <algorithm>
#include <queue>
#include <iostream>

template <typename T>
struct Node {
  int occurrences, weight;
  struct Node *leftChild, *rightChild;
  struct Node *parent;
  std::vector<T> data;
};

template <typename T>
Node<T> initialize_tree(T){
  Node<T> start = {0, 10000000, nullptr, nullptr, nullptr, {}};
  return start;
}

template <typename T>
Node<T>* find_leaf_by_term(Node<T> *root, T term){
  std::queue<Node<T>*> q;
  q.push(root);

  while (!q.empty()) {
    Node<T>* current = q.front();
    q.pop();

    // Visit the root node
    if(std::find(current->data.begin(), current->data.end(), term) == current->data.end())
      return current;

    // Enqueue left child
    if (current->leftChild != nullptr) q.push(current->leftChild);

    // Enqueue right child
    if (current->rightChild != nullptr) q.push(current->rightChild);
  }
  return root;
}

template <typename T>
Node<T>* find_leaf_by_occurrences(Node<T> *root, int occurrences){
  std::queue<Node<T>*> q;
  q.push(root);

  while (!q.empty()) {
    Node<T>* current = q.front();
    q.pop();

    // Visit the root node
    if(current->occurrences == occurrences) return current;

    // Enqueue left child
    if (current->leftChild != nullptr) q.push(current->leftChild);

    // Enqueue right child
    if (current->rightChild != nullptr) q.push(current->rightChild);
  }
  return nullptr;
}

template <typename T>
void update_weight(Node<T>* node){
  node->weight = node->leftChild->weight + node->rightChild->weight;
}

template <typename T>
void swap_children(Node<T>* node){
  Node<T>* holder = node->leftChild;
  node->leftChild = node->rightChild;
  node->rightChild = holder;
}

template <typename T>
void exchange_nodes(Node<T>* leftNode, Node<T>* rightNode){
  if(leftNode->parent->leftChild == leftNode && rightNode->parent->leftChild == rightNode){
    leftNode->parent->leftChild = rightNode;
    rightNode->parent->leftChild = leftNode;
    Node<T>* holder = leftNode->parent;
    leftNode->parent = rightNode->parent;
    rightNode->parent = holder;
  }else if(leftNode->parent->leftChild == leftNode && rightNode->parent->rightChild == rightNode){
    leftNode->parent->leftChild = rightNode;
    rightNode->parent->rightChild = leftNode;
    Node<T>* holder = leftNode->parent;
    leftNode->parent = rightNode->parent;
    rightNode->parent = holder;
  } else if(leftNode->parent->rightChild == leftNode && rightNode->parent->leftChild == rightNode){
    leftNode->parent->rightChild = rightNode;
    rightNode->parent->leftChild = leftNode;
    Node<T>* holder = leftNode->parent;
    leftNode->parent = rightNode->parent;
    rightNode->parent = holder;
  } else {
    leftNode->parent->rightChild = rightNode;
    rightNode->parent->rightChild = leftNode;
    Node<T>* holder = leftNode->parent;
    leftNode->parent = rightNode->parent;
    rightNode->parent = holder;
  }
}

template <typename T>
void shift_up(Node<T>* root, Node<T>* node){
  Node<T>* currNode = node;
  while(currNode != root){
    if(currNode->leftChild != nullptr && currNode->rightChild != nullptr)
      currNode->weight = currNode->leftChild->weight + currNode->rightChild->weight;
    else
      currNode->weight=currNode->occurrences*currNode->weight;
    
    if(currNode->parent != nullptr && currNode->weight > get_sibling(currNode)->weight+1){
      if(currNode->parent->parent != nullptr && currNode->weight > get_sibling(currNode->parent)->weight){
        Node<T>* q = currNode->parent->parent;
        Node<T>* oldParent = currNode->parent;
        exchange_nodes(currNode, get_sibling(currNode->parent));
        swap_children(q);
        update_weight(oldParent);
      }
    }
    
    if(currNode->parent == currNode) std::cout << "wtf" << std::endl << std::flush;
    if(currNode->parent->parent == nullptr) std::cout << "root" << std::endl << std::flush;
    currNode = currNode->parent;
  }
}

template <typename T>
void remove_child_from_parent(Node<T> *child){
  if(child->parent->leftChild == child){
    child->parent->leftChild = nullptr;
    free(child);
  } else {
    child->parent->rightChild = nullptr;
    free(child);
  }
}

template <typename T>
Node<T>* get_sibling(Node<T> *node){
  if(node->parent->leftChild == node){
    return node->parent->rightChild;
  }else {
    return node->parent->leftChild;
  }
}

template <typename T>
void update_tree(Node<T> *root, T nextTerm){
  Node<T>* p = find_leaf_by_term(root, nextTerm);
  Node<T>* q = find_leaf_by_occurrences(root, p->occurrences+1);

  std::cout << "Found leaves" << std::endl << std::flush;

  if(q!=nullptr){
    std::cout << "q defined" << std::endl << std::flush;
    p->data.erase(std::find(p->data.begin(),p->data.end(), nextTerm));
    p->weight = p->weight - p->occurrences;
    q->data.push_back(nextTerm);
    q->weight = q->weight + p->occurrences;
    std::cout << "weights adjusted" << std::endl << std::flush;

    shift_up(root, q);
    if(p->data.empty())
      remove_child_from_parent(p);
    else
      shift_up(root, get_sibling(p));
  } else {
    std::cout << "q undefined" << std::endl << std::flush;
    Node<T> newClass;
    Node<T> newNode = {0,0, p, &newClass, p->parent, {}};
    newClass = {p->occurrences+1,p->occurrences+1, nullptr, nullptr, &newNode, {nextTerm}};
    std::cout << "new nodes defined" << std::endl << std::flush;

    if(p->parent != nullptr){
      if(p->parent->leftChild == p)
        p->parent->leftChild = &newNode;
      else
        p->parent->rightChild = &newNode;
    } else {
      root = &newNode;
    }
    p->parent = &newNode;
    p->data.erase(std::find(p->data.begin(),p->data.end(), nextTerm));
    p->weight = p->weight - p->occurrences;

    std::cout << "new nodes inserted" << std::endl << std::flush;

    if(p->data.empty()){
      std::cout << "p is empty" << std::endl << std::flush;
      remove_child_from_parent(p);
    } else {
      std::cout << "p is nonempty, shifting sibling" << std::endl << std::flush;
      shift_up(root, get_sibling(p));
    }

    std::cout << "shifting newnode" << std::endl << std::flush;
    shift_up(root, &newNode);
  }
  
}

template <typename T>
void print_tree(Node<T> *root){
  std::queue<Node<T>*> q;
  q.push(root);

  while (!q.empty()) {
    Node<T>* current = q.front();
    q.pop();

    // Visit the root node
    std::cout << current->weight << std::endl << std::flush;
    //for(int i = 0; i < current->data.size(); i++)
    // std::cout << current->data[i] << std::endl << std::flush;

    // Enqueue left child
    if (current->leftChild != nullptr) q.push(current->leftChild);

    // Enqueue right child
    if (current->rightChild != nullptr) q.push(current->rightChild);
  }
}

int main(){
  std::cout << "starting program" << std::endl <<  std::flush;
  int type;
  std::cout << "initializing tree" << std::endl <<  std::flush;
  Node<int> tree = initialize_tree(type);
  std::cout << "initialized tree" << std::endl <<  std::flush;
  //std::vector<int> data = {1,2,5,2,4,1,2,5,3,5,0,1};
  std::vector<int> data = {1,2,5,2};
  std::cout << "initialized dummy data, starting loop" << std::endl <<  std::flush;
  for(int i = 0; i < data.size(); i++){
    std::cout << "adding the value at " << i << std::endl <<  std::flush;
    update_tree(&tree, data[i]);
    std::cout << "printing the tree at " << i << std::endl <<  std::flush;
    print_tree(&tree);
  }
}
