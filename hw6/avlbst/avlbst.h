#ifndef RBBST_H
#define RBBST_H

#include "bst.h"
#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>

struct KeyError {};

/**
 * A special kind of node for an AVL tree, which adds the balance as a data member, plus
 * other additional helper functions. You do NOT need to implement any functionality or
 * add additional data members or helper functions.
 */
template<typename Key, typename Value>
class AVLNode : public Node<Key, Value> {
  public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, std::shared_ptr<AVLNode<Key, Value>> parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    char getBalance() const;
    void setBalance(char balance);
    void updateBalance(char diff);

    std::shared_ptr<AVLNode<Key, Value>> getParent_AVL() const;
    std::shared_ptr<AVLNode<Key, Value>> getLeft_AVL() const;
    std::shared_ptr<AVLNode<Key, Value>> getRight_AVL() const;

  protected:
    // to store the balance of a given node
    char balance_;
};

// -------------------------------------------------
// Begin implementations for the AVLNode class.
// -------------------------------------------------

// An explicit constructor to initialize the elements by calling the base class constructor and setting
// the color to red since every new node will be red when it is first inserted.

template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, std::shared_ptr<AVLNode<Key, Value>> parent)
        : Node<Key, Value>(key, value, parent), balance_(0) {}

// A destructor which does nothing.
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode() {}

// A getter for the balance of a AVLNode.
template<class Key, class Value>
char AVLNode<Key, Value>::getBalance() const {
    return balance_;
}

// A setter for the balance of a AVLNode.
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(char balance) {
    balance_ = balance;
}

// Adds diff to the balance of a AVLNode.
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(char diff) {
    balance_ += diff;
}

// A separate getParent_AVL function other than the base class function due to covariant return types
template<class Key, class Value>
std::shared_ptr<AVLNode<Key, Value>> AVLNode<Key, Value>::getParent_AVL() const {
    return std::static_pointer_cast<AVLNode<Key, Value>>(this->parent_);
}

// Similar getLeft_AVL function
template<class Key, class Value>
std::shared_ptr<AVLNode<Key, Value>> AVLNode<Key, Value>::getLeft_AVL() const {
    return std::static_pointer_cast<AVLNode<Key, Value>>(this->left_);
}

// Similar getRight_AVL function
template<class Key, class Value>
std::shared_ptr<AVLNode<Key, Value>> AVLNode<Key, Value>::getRight_AVL() const {
    return std::static_pointer_cast<AVLNode<Key, Value>>(this->right_);
}

// -----------------------------------------------
// End implementations for the AVLNode class.
// -----------------------------------------------

template<class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value> {
  public:
    void rotateLeft(std::shared_ptr<AVLNode<Key, Value>> p, std::shared_ptr<AVLNode<Key, Value>> n);   // TODO
    void rotateRight(std::shared_ptr<AVLNode<Key, Value>> p, std::shared_ptr<AVLNode<Key, Value>> n);  // TODO
    void print_balances();
    // Remember, AVL is a self-balancing BST
    // Resultant tree after the insert and remove function should be a balanced tree
    // Make appropriate calls to rotateLeft(...) and rotateRight(...)
    // in insert and remove for balancing the height of the AVLTree
    virtual void insert(const std::pair<const Key, Value>& new_item);  // TODO
    virtual void remove(const Key& key);                               // TODO

  protected:
    // Helper function already provided to you.
    virtual void nodeSwap(std::shared_ptr<AVLNode<Key, Value>> n1, std::shared_ptr<AVLNode<Key, Value>> n2);
    void insert_fix(std::shared_ptr<AVLNode<Key, Value>> n, std::shared_ptr<AVLNode<Key, Value>> p);
    void remove_fix(std::shared_ptr<AVLNode<Key, Value>> n, char diff);
    // Add helper functions here
    // Consider adding functions like getBalance(...) given a key in the Tree
    // setBalance(...) given a key to a node and balance value, etc
    bool isLeftChild(std::shared_ptr<AVLNode<Key, Value>> current);
    bool isRightChild(std::shared_ptr<AVLNode<Key, Value>> current);
    bool isLeafNode(std::shared_ptr<AVLNode<Key, Value>> current);
    void print_balances_help(std::shared_ptr<AVLNode<Key, Value>> current);
    // You may implement a printRootAVL(...)
    // using the printRoot() function from the BST implementation
};
template<class Key, class Value>
bool AVLTree<Key, Value>::isLeftChild(
        std::shared_ptr<AVLNode<Key, Value>> current) {  // checks if the given node is a left child
    std::shared_ptr<AVLNode<Key, Value>> parent_node = current->getParent_AVL();  // sets parent node
    if (parent_node == nullptr) {  // checks for a null parent, indicating that our node is not a child node
        return false;
    }
    return parent_node->getLeft_AVL() == current;  // checks if our node is a left child
}
template<class Key, class Value>
bool AVLTree<Key, Value>::isRightChild(
        std::shared_ptr<AVLNode<Key, Value>> current) {  // checks if the given node is a right child
    std::shared_ptr<AVLNode<Key, Value>> parent_node = current->getParent_AVL();  // sets parent node
    if (parent_node == nullptr) {  // checks for a null parent, indicating that our node is not a child node
        return false;
    }
    return parent_node->getRight_AVL() == current;  // checks if our node is a right child
}
template<class Key, class Value>
bool AVLTree<Key, Value>::isLeafNode(
        std::shared_ptr<AVLNode<Key, Value>> current) {  // checks if the given node is a leaf node
    if (current == nullptr) {                            // checks for a null node
        return false;
    }
    return current->getLeft_AVL() == nullptr
           && current->getRight_AVL() == nullptr;  // returns true if both of the nodes children are null
}

template<class Key, class Value>
void AVLTree<Key, Value>::print_balances_help(std::shared_ptr<AVLNode<Key, Value>> current) {
    if (current == nullptr) {  // base case, inorder traversal
        return;
    }
    print_balances_help(current->getLeft_AVL());                                       // recursive call to left subtree
    std::cout << current->getKey() << " " << int(current->getBalance()) << std::endl;  // prints out node key and
                                                                                       // balance
    print_balances_help(current->getRight_AVL());  // recursive call to right subtree
}

template<class Key, class Value>
void AVLTree<Key, Value>::print_balances() {  // helper function to print balances
    print_balances_help(std::static_pointer_cast<AVLNode<Key, Value>>(this->root_));  // calls helper function on root
}
// Pre condition: p is the parent of n
// Post condition: p is the left child of n
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(std::shared_ptr<AVLNode<Key, Value>> p, std::shared_ptr<AVLNode<Key, Value>> n) {
    if (p == nullptr || n == nullptr) {  // if either node is null, do not rotate
        return;
    }
    if (p->getParent_AVL() != nullptr) {  // if p is not the root
        if (isLeftChild(p)) {
            p->getParent_AVL()->setLeft(n);  // set p's parent's left pointer to n
        } else {
            p->getParent_AVL()->setRight(n);  // set p's parent's right pointer to n
        }
        n->setParent(p->getParent());  // set n's parent to p's parent
    } else {
        this->root_ = n;        // set n to be the root
        n->setParent(nullptr);  // update n's parent to be null
    }
    p->setRight(n->getLeft_AVL());       // sets n's left subtree to be p's right subtree
    if (n->getLeft_AVL() != nullptr) {   // if n has a left subtree
        n->getLeft_AVL()->setParent(p);  // set its parent node to be p
    }
    n->setLeft(p);    // set n's left to be p
    p->setParent(n);  // set p's parent to be n
}

// Pre condition: p is the parent of n
// Post condition: p is the right child of n
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(std::shared_ptr<AVLNode<Key, Value>> p, std::shared_ptr<AVLNode<Key, Value>> n) {
    if (p == nullptr || n == nullptr) {  // if either node is null, do not rotate
        return;
    }
    if (p->getParent_AVL() != nullptr) {  // if p is not the root
        if (isLeftChild(p)) {
            p->getParent_AVL()->setLeft(n);  // set p's parent's left pointer to n
        } else {
            p->getParent_AVL()->setRight(n);  // set p's parent's right pointer to n
        }
        n->setParent(p->getParent_AVL());  // set n's parent to p's parent
    } else {
        this->root_ = n;        // set n to be the root
        n->setParent(nullptr);  // update n's parent to be null
    }
    p->setLeft(n->getRight_AVL());  // sets p's left subtree to be n's right subtree
    if (n->getRight_AVL() != nullptr) {
        n->getRight_AVL()->setParent(p);
    }
    n->setRight(p);   // set n's right to be p
    p->setParent(n);  // set p's parent to be n
}

template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item) {
    if (this->empty()) {  // checks for empty trees
        this->root_ = std::make_shared<AVLNode<Key, Value>>(
                new_item.first, new_item.second, nullptr);  // sets root to be the new node
        return;
    }
    std::shared_ptr<AVLNode<Key, Value>> curr_node
            = std::static_pointer_cast<AVLNode<Key, Value>>(this->root_);  // sets traversal node to the root
    while (curr_node != nullptr) {                                         // while we are still on the tree
        if (curr_node->getKey() == new_item.first) {  // if a node with the given key already exists
            curr_node->setValue(new_item.second);     // rewrite the existing key's value
            return;
        }
        if (new_item.first < curr_node->getKey()) {     // if given key is greater than traversal key
            if (curr_node->getLeft_AVL() == nullptr) {  // if space is open for insertion
                curr_node->setLeft(std::make_shared<AVLNode<Key, Value>>(
                        new_item.first, new_item.second, curr_node));  // set traversal's left pointer to the new node
                if (curr_node->getBalance() == -1 || curr_node->getBalance() == 1) {  // if node already has a balance
                    curr_node->setBalance(0);                                         // set it to 0
                } else {
                    curr_node->setBalance(-1);                        // set traversal node's balance ot negative 1
                    insert_fix(curr_node->getLeft_AVL(), curr_node);  // calls insert fix on new node and parent
                }
                return;
            }
            curr_node = curr_node->getLeft_AVL();        // traverse left
        } else {                                         // if given key is less than traversal key
            if (curr_node->getRight_AVL() == nullptr) {  // if space is open for insertion
                curr_node->setRight(std::make_shared<AVLNode<Key, Value>>(
                        new_item.first, new_item.second, curr_node));  // set traversal's left pointer to the new node
                if (curr_node->getBalance() == -1 || curr_node->getBalance() == 1) {  // if node already has a balance
                    curr_node->setBalance(0);                                         // set it to 0
                } else {
                    curr_node->setBalance(1);                          // set traversal node's balance to positive 1
                    insert_fix(curr_node->getRight_AVL(), curr_node);  // calls insert fix on new node and parent
                }
                return;
            }
            curr_node = curr_node->getRight_AVL();  // traverse right
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
    if (this->empty()) {  // if empty, no nodes to remove
        return;
    }
    std::shared_ptr<AVLNode<Key, Value>> curr_node = std::static_pointer_cast<AVLNode<Key, Value>>(
            BinarySearchTree<Key, Value>::internalFind(key));  // finds node with given key in the tree
    if (curr_node == nullptr) {                                // if node not found, return
        return;
    }
    if (curr_node == this->root_) {   // if node is the root
        if (isLeafNode(curr_node)) {  // if root is a leaf
            this->root_ = nullptr;    // set root to be null
            return;
        } else if (curr_node->getLeft() == nullptr) {  // if root has only a right child
            std::static_pointer_cast<AVLNode<Key, Value>>(this->root_)
                    = curr_node->getRight_AVL();  // set root to nodes right child
            this->root_->setParent(nullptr);

            return;
        } else if (curr_node->getRight_AVL() == nullptr) {  // if root only has a left child
            std::static_pointer_cast<AVLNode<Key, Value>>(this->root_)
                    = curr_node->getLeft_AVL();  // set root to nodes left child
            this->root_->setParent(nullptr);
            return;
        } else {  // if root has two children, must swap with predecessor
            if (std::static_pointer_cast<AVLNode<Key, Value>>(BinarySearchTree<Key, Value>::predecessor(curr_node))
                == nullptr) {  // if no predecessor, return
                return;
            }
            nodeSwap(
                    curr_node,
                    std::static_pointer_cast<AVLNode<Key, Value>>(
                            BinarySearchTree<Key, Value>::predecessor(curr_node)));  // swap node with predecessor
            if (isLeafNode(curr_node)) {  // if node is a leaf node, remove it
                if (isRightChild(curr_node)) {
                    curr_node->getParent_AVL()->setRight(nullptr);  // sets nodes parents right pointer equal to null
                    remove_fix(
                            curr_node->getParent_AVL(),
                            -1);  // calls remove fix on the nodes parent with a balance of -1
                } else {
                    curr_node->getParent_AVL()->setLeft(nullptr);  // sets nodes parents left pointer equal to null
                    remove_fix(
                            curr_node->getParent_AVL(), 1);  // calls remove fix on the nodes parent with a balance of 1
                }
                return;
            } else if (curr_node->getLeft_AVL() == nullptr) {  // if node has only a right child
                if (isRightChild(curr_node)) {
                    curr_node->getParent_AVL()->setRight(
                            curr_node->getRight_AVL());  // set nodes parents right pointer to the nodes right child
                    curr_node->getRight_AVL()->setParent(
                            curr_node->getParent_AVL());  // sets nodes right childs parent equal to the nodes parent
                    remove_fix(
                            curr_node->getParent_AVL(),
                            -1);  // calls remove fix on the parent node with a difference of -1
                } else {
                    curr_node->getParent_AVL()->setLeft(
                            curr_node->getRight_AVL());  // set nodes parents left pointer to the nodes right child
                    curr_node->getRight_AVL()->setParent(
                            curr_node->getParent_AVL());  // sets nodes right childs parent equal to the nodes parent
                    remove_fix(curr_node->getParent_AVL(), 1);  // calls remove fix on the parent with a difference of 1
                }
                return;
            } else {  // if node has only a left child
                if (isRightChild(curr_node)) {
                    curr_node->getParent_AVL()->setRight(
                            curr_node->getLeft_AVL());  // set nodes parents right pointer to the nodes left child
                    curr_node->getLeft_AVL()->setParent(
                            curr_node->getParent_AVL());  // sets nodes left childs parent equal to the nodes parent
                    remove_fix(
                            curr_node->getParent_AVL(),
                            -1);  // calls remove fix on the parent node with a difference of -1
                } else {
                    curr_node->getParent_AVL()->setLeft(
                            curr_node->getLeft_AVL());  // sets nodes parents left pointer to the nodes left child
                    curr_node->getLeft_AVL()->setParent(
                            curr_node->getParent_AVL());  // sets nodes left childs parent equal to the nodes parent
                    remove_fix(curr_node->getParent_AVL(), 1);  // calls remove fix on the parent with a difference of 1
                }
                return;
            }
        }
    }
    if (isLeafNode(curr_node)) {  // if leaf node, remove it
        if (isRightChild(curr_node)) {
            curr_node->getParent_AVL()->setRight(nullptr);  // sets parents right pointer to null
            remove_fix(curr_node->getParent_AVL(), -1);  // calls remove fix on the parent node with a difference of -1
        } else {
            curr_node->getParent_AVL()->setLeft(nullptr);  // sets parents left pointer to null
            remove_fix(curr_node->getParent_AVL(), 1);     // calls remove fix on the parent with a difference of 1
        }
        return;
    } else if (curr_node->getLeft_AVL() == nullptr) {  // if the node only has a right child, promote the child
        if (isRightChild(curr_node)) {
            curr_node->getParent_AVL()->setRight(
                    curr_node->getRight_AVL());  // set nodes parents right pointer to the nodes right child
            curr_node->getRight_AVL()->setParent(
                    curr_node->getParent_AVL());         // sets nodes right childs parent equal to the nodes parentv
            remove_fix(curr_node->getParent_AVL(), -1);  // calls remove fix on the parent node with a difference of -1
        } else {                                         // if the node only has a left child, promote the child
            curr_node->getParent_AVL()->setLeft(
                    curr_node->getRight_AVL());  // set nodes parents left pointer to the nodes right child
            curr_node->getRight_AVL()->setParent(
                    curr_node->getParent_AVL());        // sets nodes right childs parent equal to the nodes parent
            remove_fix(curr_node->getParent_AVL(), 1);  // calls remove fix on the parent with a difference of 1
        }
        return;
    } else if (curr_node->getRight() == nullptr) {  // if the node only has a left child, promote the child
        if (isRightChild(curr_node)) {
            curr_node->getParent_AVL()->setRight(
                    curr_node->getLeft_AVL());  // set nodes parents right pointer to the nodes left child
            curr_node->getLeft_AVL()->setParent(
                    curr_node->getParent_AVL());         // sets nodes left childs parent equal to the nodes parent
            remove_fix(curr_node->getParent_AVL(), -1);  // calls remove fix on the parent node with a difference of -1
        } else {
            curr_node->getParent_AVL()->setLeft(
                    curr_node->getLeft_AVL());  // sets nodes parents left pointer to the nodes left child
            curr_node->getLeft_AVL()->setParent(
                    curr_node->getParent_AVL());        // sets nodes left childs parent equal to the nodes parent
            remove_fix(curr_node->getParent_AVL(), 1);  // calls remove fix on the parent with a difference of 1
        }
        return;
    } else {  // If the node has two children, swap it with its predecessor
        if (std::static_pointer_cast<AVLNode<Key, Value>>(BinarySearchTree<Key, Value>::predecessor(curr_node))
            == nullptr) {  // if the node has no predecessor, return
            return;
        }
        this->nodeSwap(
                curr_node,
                std::static_pointer_cast<AVLNode<Key, Value>>(
                        BinarySearchTree<Key, Value>::predecessor(curr_node)));  // swaps node with its predecessor

        if (isLeafNode(curr_node)) {  // if the node is a leaf node, simply remove it
            if (isRightChild(curr_node)) {
                curr_node->getParent_AVL()->setRight(nullptr);  // sets parents right pointer to null
                remove_fix(
                        curr_node->getParent_AVL(), -1);  // calls remove fix on the parent node with a difference of -1
            } else {
                curr_node->getParent_AVL()->setLeft(nullptr);  // sets parents left pointer to null
                remove_fix(curr_node->getParent_AVL(), 1);     // calls remove fix on the parent with a difference of 1
            }
            return;
        } else if (curr_node->getLeft_AVL() == nullptr) {  // if the node only has a right child, promote the child
            if (isRightChild(curr_node)) {
                curr_node->getParent_AVL()->setRight(
                        curr_node->getRight_AVL());  // set nodes parents right pointer to the nodes right child
                curr_node->getRight_AVL()->setParent(
                        curr_node->getParent_AVL());  // sets nodes right childs parent equal to the nodes parentv
                remove_fix(
                        curr_node->getParent_AVL(), -1);  // calls remove fix on the parent node with a difference of -1
            } else {
                curr_node->getParent_AVL()->setLeft(
                        curr_node->getRight_AVL());  // set nodes parents left pointer to the nodes right child
                curr_node->getRight_AVL()->setParent(
                        curr_node->getParent_AVL());        // sets nodes right childs parent equal to the nodes parent
                remove_fix(curr_node->getParent_AVL(), 1);  // calls remove fix on the parent with a difference of 1
            }
            return;
        } else {  // if the node only has a left child, promote the child
            if (isRightChild(curr_node)) {
                curr_node->getParent_AVL()->setRight(
                        curr_node->getLeft_AVL());  // set nodes parents right pointer to the nodes left child
                curr_node->getLeft_AVL()->setParent(
                        curr_node->getParent_AVL());  // sets nodes left childs parent equal to the nodes parent
                remove_fix(
                        curr_node->getParent_AVL(), -1);  // calls remove fix on the parent node with a difference of -1
            } else {
                curr_node->getParent_AVL()->setLeft(
                        curr_node->getLeft_AVL());  // sets nodes parents left pointer to the nodes left child
                curr_node->getLeft_AVL()->setParent(
                        curr_node->getParent_AVL());        // sets nodes left childs parent equal to the nodes parent
                remove_fix(curr_node->getParent_AVL(), 1);  // calls remove fix on the parent with a difference of 1
            }
            return;
        }
    }  // END PRED SWAP
}
template<class Key, class Value>
void AVLTree<Key, Value>::insert_fix(std::shared_ptr<AVLNode<Key, Value>> n, std::shared_ptr<AVLNode<Key, Value>> p) {
    if (p == nullptr || p->getParent_AVL() == nullptr) {  // if p or p's parent is null, return
        return;
    }
    std::shared_ptr<AVLNode<Key, Value>> g = p->getParent_AVL();  // set g equal to p's parent
    if (isLeftChild(p)) {                                         // if p is a left child
        g->updateBalance(-1);                                     // subtract one from the balance of g
        if (g->getBalance() == 0) {
            return;  // if g is balanced return
        }
        if (g->getBalance() == -1) {
            insert_fix(p, g);       // if g has a balance of -1, make a recursive call up the tree
        } else {                    // g has a balance of -2
            if (isLeftChild(n)) {   // if n is a left child of p, zigzig
                rotateRight(g, p);  // rotate right about g, and set node balances
                g->setBalance(0);
                p->setBalance(0);
            } else {                          // n is a right child of p, zigzag
                rotateLeft(p, n);             // rotate left about p
                rotateRight(g, n);            // rotate right about g
                if (n->getBalance() == -1) {  // if n had a balance of -1, update the balances accordingly
                    p->setBalance(0);
                    g->setBalance(1);
                    n->setBalance(0);
                } else if (n->getBalance() == 0) {  // if n had a balance of -0, update the balances accordingly
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                } else {  // if n had a balance of 1, update the balances accordingly
                    p->setBalance(-1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
            }
        }
    } else {                         // p is a right child
        g->updateBalance(1);         // add 1 to the balance of g
        if (g->getBalance() == 0) {  // if g is balanced, return
            return;
        }
        if (g->getBalance()
            == 1) {  // if g has a balance of 1, make a recursive call up the tree (might still not be balanced)
            insert_fix(p, g);
        } else {                    // if g has a balance of 2, not balanced
            if (isRightChild(n)) {  // if n is a right child, zigzig
                rotateLeft(g, p);   // rotate left about g, and set node balances
                g->setBalance(0);
                p->setBalance(0);
            } else {                         // if n is a left child, zigzag
                rotateRight(p, n);           // rotate right about p
                rotateLeft(g, n);            // rotate left about g
                if (n->getBalance() == 1) {  // if n had a balance of 1, update the balances accordingly
                    p->setBalance(0);
                    g->setBalance(-1);
                    n->setBalance(0);
                } else if (n->getBalance() == 0) {  // if n had a balance of 0, update the balances accordingly
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                } else {  // if n had a balance of -1, update the balances accordingly
                    p->setBalance(1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
            }
        }
    }
}
template<class Key, class Value>
void AVLTree<Key, Value>::remove_fix(std::shared_ptr<AVLNode<Key, Value>> n, char diff) {
    if (n == nullptr) {  // if the node is null, return
        return;
    }
    std::shared_ptr<AVLNode<Key, Value>> p = n->getParent_AVL();  // set p to be n's parent
    char next_diff = 0;                                           // initialize the next diff, passed to recursive calls
    if (p != nullptr) {                                           // if p is not null
        if (isLeftChild(n)) {                                     // if n is a left child of p
            next_diff = 1;                                        // set next diff to 1, removing from p's left subtree
        } else {                                                  // if n is a right child of p
            next_diff = -1;  // set next diff to -1, removing from p's right subtree
        }
    }
    n->updateBalance(diff);  // update n's balance by diff
    if (diff == -1) {        // if an element was removed from n's right subtree
        if (n->getBalance() == -2) {
            std::shared_ptr<AVLNode<Key, Value>> c = n->getLeft_AVL();  // set c to n's larger subtree, the left one
            if (c->getBalance() == -1) {                                // if the child has a balance of -1, zigzig
                rotateRight(n, c);                                      // rotate right about n, and set balances
                n->setBalance(0);
                c->setBalance(0);
                remove_fix(p, next_diff);       // recursive call to parent
            } else if (c->getBalance() == 0) {  // zigzig
                rotateRight(n, c);              // rotate right about n and set balances
                n->setBalance(-1);
                c->setBalance(1);
                return;
            } else {                                                         // c had a balance of 1, zigzag
                std::shared_ptr<AVLNode<Key, Value>> g = c->getRight_AVL();  // set g to be c's right subtree
                rotateLeft(c, g);                                            // rotate left about c
                rotateRight(n, g);                                           // rotate rigth about n
                if (g->getBalance() == 1) {  // if g had a balance of 1, update the balances accordingly
                    n->setBalance(0);
                    c->setBalance(-1);
                    g->setBalance(0);
                } else if (g->getBalance() == 0) {  // if g had a balance of 0, update the balances accordingly
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                } else {  // if g had a balance of -1, update the balances accordingly
                    n->setBalance(1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                remove_fix(p, next_diff);  // recursive call to parent
            }
        } else if (n->getBalance() == -1) {  // if n has an updated balance of -1, the tree is balanced
            return;
        } else {  // if n has an updated balance of 0, make a recursive call
            remove_fix(p, next_diff);
        }
    } else {                                                             // element removed from n's subtree
        if (n->getBalance() == 2) {                                      // n has a balance of 2, unbalanced
            std::shared_ptr<AVLNode<Key, Value>> c = n->getRight_AVL();  // set c to n's bigger subtree, the right one
            if (c->getBalance() == 1) {                                  // if c has a balance of 1, zigzig
                rotateLeft(n, c);                                        // rotate left about n and set balances
                n->setBalance(0);
                c->setBalance(0);
                remove_fix(p, next_diff);       // make recursive call to the parent
            } else if (c->getBalance() == 0) {  // zigzig
                rotateLeft(n, c);               // rotate left about n and set balances
                n->setBalance(1);
                c->setBalance(-1);
                return;
            } else {                                                        // c had a balance of -1, zigzag
                std::shared_ptr<AVLNode<Key, Value>> g = c->getLeft_AVL();  // set g to c's left subtree
                rotateRight(c, g);                                          // rotate right about c
                rotateLeft(n, g);                                           // rotate left about n
                if (g->getBalance() == -1) {  // if g had a balance of 1, update the balances accordingly
                    n->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);
                } else if (g->getBalance() == 0) {  // if g had a balance of 1, update the balances accordingly
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                } else {  // if g had a balance of 1, update the balances accordingly
                    n->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                remove_fix(p, next_diff);  // make recursive call to the parent
            }
        } else if (n->getBalance() == 1) {  // if n has an updated balance of 1, the tree is balanced
            return;
        } else {
            remove_fix(p, next_diff);  // make recursive call to the parent
        }
    }
}

// Function already completed for you
template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(std::shared_ptr<AVLNode<Key, Value>> n1, std::shared_ptr<AVLNode<Key, Value>> n2) {
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    char tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif
