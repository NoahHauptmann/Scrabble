#include "simple.h"

#include <algorithm>
#include <iostream>

using namespace std;

char* SimpleCharacterManager::alloc_chars(size_t size) {
    if (*(first_available_address + size - 1)
        != '\0') {    // Checks to see if there are "size" chars available following first available address
        return NULL;  // If there is no available space, returns NULL
    }
    char* hold = first_available_address;
    first_available_address += size;  // Updates first available address
    return hold;                      // returns original first available address
}

void SimpleCharacterManager::free_chars(char* address) {
    if (address == NULL || address < buffer
        || address > buffer + BUFFER_SIZE - 1) {  // Checks to see if address if within the buffer
        return;
    } else {
        std::fill(address, buffer + BUFFER_SIZE - 1, '\0');  // Fills designated buffer space with null characters
        first_available_address = address;  // Updates first_abailable_address to account for freed memory
    }
}
