#include "token_list.h"

#include <stdexcept>
using namespace std;

size_t TokenList::size() const { return this->_size; }

// TODO: implement the TokenList methods.

Token& TokenList::at(size_t index) {
    if (this->size() == 0) {
        throw std::out_of_range("no items");
    }
    shared_ptr<Item> temp = this->head;
    for (size_t i = 0; i < (index % this->_size); i++) {  // iterates through list to desired item
        temp = temp->next;
    }
    return temp->value;
    // TODO: complete this function here
}

const Token& TokenList::at(size_t index) const {
    if (this->size() == 0) {
        throw std::out_of_range("no items");
    }
    shared_ptr<Item> temp = this->head;
    for (size_t i = 0; i < (index % this->_size); i++) {  // iterates through list to desired item
        temp = temp->next;
    }
    return temp->value;
    // TODO: complete this function here
}

std::ostream& operator<<(std::ostream& out, const TokenList& list) {
    out << "{";
    std::shared_ptr<TokenList::Item> cursor = list.head;
    if (cursor != nullptr) {
        for (size_t i = 0; i < list.size() - 1; i++) {
            out << cursor->value << ", ";
            cursor = cursor->next;
        }
        out << cursor->value;
    }
    return out << "}";
}

void TokenList ::push_back(Token value) {
    if (this->size() == 0) {                    // checks for an empty list
        this->head = make_shared<Item>(value);  // initializes head pointer
        this->head->next = this->head;
        this->head->previous = this->head;
        this->_size++;  // increments size of list
        return;
    }

    shared_ptr<Item> temp = this->head->previous;
    temp->next = make_shared<Item>(value);  // creates new item at the back of the array
    temp->next->next = this->head;
    temp->next->previous = temp;
    this->head->previous = temp->next;

    this->_size++;  // increments size of list
}

void TokenList ::insert(size_t index, Token value) {
    this->_size++;  // initially increment size for proper indexing
    shared_ptr<Item> temp = this->head;
    for (size_t i = 0; i < (index % this->_size) - 1; i++) {  // iterates to correct address
        temp = temp->next;
    }
    temp->next->previous = make_shared<Item>(value);  // instantiates new item
    temp->next->previous->next = temp->next;
    temp->next = temp->next->previous;
    temp->next->previous = temp;
}
void TokenList ::remove(size_t index) {
    if (this->size() == 0) {  // checks for empty list
        return;
    }
    shared_ptr<Item> temp = this->head;
    for (size_t i = 0; i < (index % this->_size); i++) {  // iterates to the correct item
        temp = temp->next;
    }
    if (temp == this->head) {                  // checks for removing the head of the list
        if (this->head->next == this->head) {  // checks for lists of size 1
            this->head->previous = nullptr;
            this->head->next = nullptr;
            this->head = nullptr;
            temp = nullptr;  // destroys the head item
            _size--;         // decrements size
            return;
        }
        temp->previous->next = temp->next;
        temp->next->previous = temp->previous;
        head = temp->next;
        temp = nullptr;  // destroys item
        this->_size--;   // decrements size
        return;
    }
    temp->previous->next = temp->next;
    temp->next->previous = temp->previous;
    temp = nullptr;  // destroys item
    this->_size--;   // decrements size
    return;
}
void TokenList ::reverse() {
    TokenList reverse;                             // creates a new TokenList to store reversed list
    shared_ptr<Item> temp = this->head->previous;  // points to last item on list
    while (temp != this->head) {
        reverse.push_back(temp->value);  // pushes calling list onto reverse one item at a time
        temp = temp->previous;
    }
    reverse.push_back(temp->value);
    *this = reverse;  // sets calling list equal to reverse
}

TokenList ::TokenList(TokenList const& other) {
    shared_ptr<Item> temp = other.head;
    for (size_t i = 0; i < other._size; i++) {
        this->push_back(temp->value);  // pushes values of other onto newly constructed list
        temp = temp->next;
    }
}
TokenList& TokenList ::operator=(TokenList const& other) {
    if (this == &other) {  // checks for self-assignment
        return *this;
    }
    while (this->size() != 0) {  // empties the calling list
        this->remove(0);
    }
    shared_ptr<Item> temp = other.head;
    for (size_t i = 0; i < other._size; i++) {
        this->push_back(temp->value);  // pushes the passed list onto the calling list
        temp = temp->next;
    }
    return *this;  // returns calling list
}
TokenList ::~TokenList() {
    while (this->size() != 0) {  // removes all items in the list
        this->remove(0);
    }
}
