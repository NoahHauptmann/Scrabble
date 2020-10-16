#ifndef QSORT_H
#define QSORT_H

#include <algorithm>
#include <vector>

template<class T, class TComparator>
T& median_of_three(T& a, T& b, T& c, TComparator comparator) {
    if ((comparator(a, b) && !comparator(a, c))
        || (comparator(b, a) && !comparator(c, a))) {  // checks if a is between b and c
        return a;
    } else if ((comparator(b, a) && !comparator(b, c)) || (comparator(a, b) && !comparator(c, b))) {  // checks if b is
                                                                                                      // between a and c
        return b;
    } else {  // checks if c is between a and b
        return c;
    }
}

template<class T, class TComparator>
size_t partition(std::vector<T>& vec, TComparator& comparator, size_t low, size_t high) {
    size_t i = low;
    T pivot = median_of_three(vec[low], vec[(low + high) / 2], vec[high], comparator);  // calculates pivot value
    size_t pivot_loc;
    for (size_t j = low; j <= high; j++) {  // iterates over designated vetor component
        if (vec[j] == pivot) {              // checks if the loop has found the pivot value
            pivot_loc = j;
            continue;
        }
        if (comparator(vec[j], pivot)) {  // checks the current value against the pivot using the comparator
            if (vec[i] == pivot) {        // updates pivot_loc
                pivot_loc = j;
            } else if (vec[j] == pivot) {  // updates pivot_loc
                pivot_loc = i;
            }
            T temp = vec[i];  // performs swap
            vec[i] = vec[j];
            vec[j] = temp;
            i++;
        }
    }
    T temp = vec[i];  // swaps pivot into the correct spot
    vec[i] = vec[pivot_loc];
    vec[pivot_loc] = temp;
    return i;
}

template<class T, class TComparator>
void quicksort_help(std::vector<T>& vec, TComparator comparator, int start, int end) {
    if (start >= end) {  // base case
        return;
    }
    int partition_loc = partition(vec, comparator, start, end);  // finds the partition index
    quicksort_help(vec, comparator, start, partition_loc - 1);   // recurses on front half of vector segment
    quicksort_help(vec, comparator, partition_loc + 1, end);     // recurses on back half of vector segment
}

template<class T, class TComparator>
void quicksort(std::vector<T>& vec, TComparator comparator) {
    quicksort_help(vec, comparator, 0, vec.size() - 1);  // makes call to the helper function
}

#endif
