#include "token_stack.h"

#include <stdexcept>

// TODO: return the top value on the stack.
const Token& TokenStack::top() const {
    if (this->empty()) {
        throw std::out_of_range("empty stack");
    }

    // TODO: complete function here
    return this->list.at(this->list.size() - 1);  // returns the item at the end of the list
}

// TODO: implement the TokenStack methods.
size_t TokenStack::size() const {
    return this->list.size();  // returns size of member list
}
bool TokenStack::empty() const {
    return (this->list.size() == 0);  // checks if the size of the member list is 0
}
void TokenStack::push(const Token& t) {
    this->list.push_back(t);  // adds passed token to the end of the list
    return;
}
void TokenStack::pop() {
    this->list.remove(this->list.size() - 1);  // removes item at the end of the list
    return;
}
void TokenStack::reverse() {
    list.reverse();  // calls reverse on member list
    return;
}
std::ostream& operator<<(std::ostream& out, const TokenStack& stack) {
    return out << stack.list << std::endl;  // prints member list
}