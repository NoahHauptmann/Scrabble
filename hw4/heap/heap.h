#ifndef HEAP_H
#define HEAP_H

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <vector>

template<typename T, typename TComparator = std::less<T>>
class Heap {
  public:
    // Constructs an m-ary heap for any m >= 2
    Heap(size_t m1, TComparator comparator1 = TComparator());

    // Destructor as needed
    ~Heap();

    // Adds an item
    void push(const T& item);

    // returns the element at the top of the heap
    //  max (if max-heap) or min (if min-heap)
    const T& top() const;

    // Removes the top element
    void pop();

    // returns true if the heap is empty
    bool empty() const;

  public:
    // Add whatever helper functions and data members you need below
    void trickleUp(size_t loc);
    // heapify() helper function
    void heapify(size_t loc);

    // Data
    std::vector<T> store;
    size_t m;
    TComparator comparator;
};

template<class T, class TComparator>
Heap<T, TComparator>::Heap(size_t m1, TComparator comparator1) {
    m = m1;
    comparator = comparator1;
}

template<class T, class TComparator>
Heap<T, TComparator>::~Heap() {}

template<class T, class TComparator>
const T& Heap<T, TComparator>::top() const {
    // If nothing to top:
    if (store.size() == 0) {
        throw std::logic_error("can't top an empty heap");
    }
    return store[0];  // returns front of vector (top of heap)
}

template<class T, class TComparator>
void Heap<T, TComparator>::pop() {
    // If nothing to pop
    if (store.size() == 0) {
        throw std::logic_error("can't pop an empty heap");
    }
    store[0] = store[store.size() - 1];  // overwrites top node with last node
    store.pop_back();                    // gets rid of last node
    heapify(0);                          // calls trickle down on top node
}

template<class T, class TComparator>
bool Heap<T, TComparator>::empty() const {
    return store.size() == 0;  // returns if size is 0
}

template<class T, class TComparator>
void Heap<T, TComparator>::push(const T& item) {
    store.push_back(item);        // adds item to the end of heap
    trickleUp(store.size() - 1);  // calls trickle up on last item
}

template<class T, class TComparator>
void Heap<T, TComparator>::heapify(size_t loc) {
    if ((loc * m) + 1 >= store.size() || empty()) {  // base case, if we are looking at a leap node or a empty heap
        return;
    }
    int smallerChild = loc * m + 1;          // computes index of the first child
    for (int i = 2; i <= m; i++) {           // iterates over the rest of the potential children
        if ((loc * m) + i < store.size()) {  // checks if mth child exists
            if (comparator(store[loc * m + i], store[smallerChild])) {  // keeps track of smallest child
                smallerChild = loc * m + i;
            }
        }
    }
    if (comparator(store[smallerChild], store[loc])) {
        std::swap(store[loc], store[smallerChild]);  // swaps parent and child node
        heapify(smallerChild);
    }
}
template<class T, class TComparator>
void Heap<T, TComparator>::trickleUp(size_t loc) {
    if (loc == 0) {
        return;
    }
    size_t parent = (loc - 1) / m;  // calculates index of parent node
    while (parent >= 0 && (parent != loc)
           && comparator(store[loc], store[parent])) {  // loops until it has trickled to the correct spot
        std::swap(store[loc], store[parent]);           // swaps parent and child node
        if (parent == 0) {
            break;
        }
        loc = parent;            // updates current node
        parent = (loc - 1) / m;  // updates parent node
    }
}

#endif
