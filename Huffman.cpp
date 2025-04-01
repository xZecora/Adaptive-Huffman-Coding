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
Node<T>* initialize_tree(std::vector<T> dictionary){
  Node<T>* start = (Node<T>*)malloc(sizeof(Node<T>));
  start->occurrences = 0;
  start->weight = 10000000;
  start->leftChild = nullptr;
  start->rightChild = nullptr;
  start->parent = nullptr;
  start->data = dictionary;
  return start;
}

template <typename T>
Node<T>* find_leaf_by_term(Node<T> *root, T term){
  std::cout << "here now" << std::endl << std::flush;
  std::queue<Node<T>*> q;
  q.push(root);

  while (!q.empty()) {
    std::cout << "looping" << std::endl << std::flush;
    Node<T>* current = q.front();
    q.pop();

    std::cout << "searching current node" << std::endl << std::flush;
    // Visit the root node
    if(!(current->data.empty())){
      std::cout << "current node not empty" << std::endl << std::flush;
      if(std::find(current->data.begin(), current->data.end(), term) == current->data.end())
      return current;
    }
    std::cout << "not in current node" << std::endl << std::flush;

    std::cout << "still looping" << std::endl << std::flush;
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

/*
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

  if(leftNode->parent == leftNode) std::cout << "wtf" << std::endl << std::flush;
}
*/

template <typename T>
void exchange_nodes(Node<T>* leftNode, Node<T>* rightNode) {
    if (!leftNode || !rightNode || leftNode == rightNode) return; // Safety check
    
    Node<T>* leftParent = leftNode->parent;
    Node<T>* rightParent = rightNode->parent;

    if (!leftParent || !rightParent) return; // Ensure nodes are not root

    // Determine which child they are in their parents
    bool isLeftLeftChild = (leftParent->leftChild == leftNode);
    bool isRightLeftChild = (rightParent->leftChild == rightNode);

    // Swap parent pointers
    leftNode->parent = rightParent;
    rightNode->parent = leftParent;

    // Update the parent's child pointers
    if (isLeftLeftChild)
        leftParent->leftChild = rightNode;
    else
        leftParent->rightChild = rightNode;

    if (isRightLeftChild)
        rightParent->leftChild = leftNode;
    else
        rightParent->rightChild = leftNode;

}

template <typename T>
void shift_up(Node<T>* root, Node<T>* node){
  Node<T>* currNode = node;
  while(currNode->parent != nullptr){
    if(currNode->leftChild != nullptr && currNode->rightChild != nullptr)
      currNode->weight = currNode->leftChild->weight + currNode->rightChild->weight;
    else
      currNode->weight=currNode->occurrences*currNode->weight;

    std::cout << "here45" << std::endl << std::flush;
    
    if(currNode->parent != nullptr &&
       currNode->parent->parent != nullptr){
        std::cout << "here here" << std::endl << std::flush;
      if(currNode->weight > get_sibling(currNode)->weight+1 &&
         currNode->weight > get_sibling(currNode->parent)->weight){
          Node<T>* q = currNode->parent->parent;
          Node<T>* oldParent = currNode->parent;
          exchange_nodes(currNode, get_sibling(currNode->parent));
          swap_children(q);
          update_weight(oldParent);
      }
    }
    
    //if(currNode->parent == currNode) std::cout << "wtf" << std::endl << std::flush;
    if(currNode->parent->parent == nullptr)
      std::cout << "parent is root" << std::endl << std::flush;

    std::cout << "Next iteration" << std::endl << std::flush;
    currNode = currNode->parent;
  }
  std::cout << "end of loop" << std::endl << std::flush;
}

template <typename T>
void remove_child_from_parent(Node<T> *child){
  if(child->parent->leftChild == child)
    child->parent->leftChild = nullptr;
  else
    child->parent->rightChild = nullptr;
}

template <typename T>
Node<T>* get_sibling(Node<T> *node){
  std::cout << "getting sibling" << std::endl << std::flush;
  if(node->parent->leftChild == node){
    return node->parent->rightChild;
  }else {
    return node->parent->leftChild;
  }
}

template <typename T>
void update_tree(Node<T>* &root, T nextTerm){
  std::cout << "Finding leaves" << std::endl << std::flush;

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
    Node<T> newNode = {0,0, p, &newClass, p->parent, std::vector<T>()};
    newClass = {p->occurrences+1,p->occurrences+1, nullptr, nullptr, &newNode, {nextTerm}};
    newNode.weight = newClass.weight + p->weight;
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
    std::cout << current->data.size() << std::endl << std::flush;

    // Visit the root node
    /*
    if(!current->data.empty())
      for(int i = 0; i < current->data.size(); i++)
        std::cout << current->data[i] << std::endl << std::flush;
    */

    // Enqueue left child
    if (current->leftChild != nullptr) q.push(current->leftChild);
    else std::cout << "there is no leftChild" << std::endl << std::flush;

    // Enqueue right child
    if (current->rightChild != nullptr) q.push(current->rightChild);
    else std::cout << "there is no rightChild" << std::endl << std::flush;
  }
}

int main(){
  std::cout << "starting program" << std::endl <<  std::flush;
  std::vector<int> dictionary = {1,2,3,4,5,6,7,8,9,0};
  std::cout << "initializing tree" << std::endl <<  std::flush;
  Node<int>* tree = initialize_tree(dictionary);
  std::cout << "initialized tree" << std::endl <<  std::flush;
  //std::vector<int> data = {1,2,5,2,4,1,2,5,3,5,0,1};
  std::vector<int> data = {1,2,5,2};
  std::cout << "initialized dummy data, starting loop" << std::endl <<  std::flush;
  for(int i = 0; i < data.size(); i++){
    std::cout << "adding the value at " << i << std::endl <<  std::flush;
    update_tree(tree, data[i]);
    std::cout << "printing the tree at " << i << std::endl <<  std::flush;
    if(i == 0)
      print_tree(tree);
  }
}
