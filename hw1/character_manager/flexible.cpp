#include "flexible.h"

using namespace std;

// The constructor creates a unique_ptr to a dynamically allocated array of two
// CharacterBlocks using the initialization list syntax. No need to change this
// unless you add fields to the FlexibleCharacterManager.
FlexibleCharacterManager::FlexibleCharacterManager() : blocks(new CharacterBlock[2]{}) {}

char* FlexibleCharacterManager::alloc_chars(size_t n) {
    // TODO: your implementation here
    // Scan buffer for place to put the new word
    char* spot;
    bool space = true;
    bool spotfound = false;
    for (size_t i = 0; i < BUFFER_SIZE - n; i++) {  // iterates through all of buffer

        if (this->buffer[i] == '\0') {  // checks if the current element of buffer is '\0'
            for (size_t j = 0; j < n;
                 j++) {  // if the current element i '\0', this loop iterates to see if there is space for n characters
                if (this->buffer[i + j]
                    != '\0') {  // if it finds a non-free character, it sets space to null and breaks out of the loop;
                    space = false;
                    break;
                }
            }
            if (space) {  // if the loop completed without space being set to false we found a space for our word and we
                          // will set spot to be the correct starting address
                spot = buffer + i;
                spotfound = true;
                break;  // breaks out of buffer iteration
            }
        }

        space = true;  // resets the space flag
    }
    if (!spotfound) {
        return nullptr;  // returns null if the word cannot fit
    }
    CharacterBlock newword;  // constructs the new character block
    newword.address = spot;
    newword.size = n;
    blocks[blocks_in_use] = newword;  // places the block at the end of the blocks array
    blocks_in_use++;
    if (blocks_in_use == blocks_size) {  // doubles size of blocks array
        unique_ptr<CharacterBlock[]> old_blocks(blocks.release());
        blocks = make_unique<CharacterBlock[]>(blocks_size * 2);
        for (size_t i = 0; i < blocks_in_use; i++) {
            blocks[i] = old_blocks[i];
        }
        blocks_size *= 2;
    } else if (blocks_in_use < blocks_size / 2 && blocks_size != 2) {  // halves the size of the blocks array
        unique_ptr<CharacterBlock[]> old_blocks(blocks.release());
        blocks = make_unique<CharacterBlock[]>(blocks_size / 2);
        for (size_t i = 0; i < blocks_in_use; i++) {
            blocks[i] = old_blocks[i];
        }
        blocks_size /= 2;
    }

    return spot;
}

void FlexibleCharacterManager::free_chars(char* p) {
    for (size_t i = 0; i < blocks_in_use; i++) {  // iterates through the characterblocks in the blocks array
        if (p == blocks[i].address) {  // Checks if the passed address matches one of the characterblocks in the blocks
                                       // array
            std::fill(p, p + blocks[i].size, '\0');  // Empties chosen character block and fills it with null characters

            for (size_t j = i + 1; j < blocks_in_use;
                 j++) {  // Gets rid of the blank Characterblock to keep blocks array contiguous
                blocks[j - 1].address = blocks[j].address;
                blocks[j - 1].size = blocks[j].size;
            }
            blocks_in_use--;

            if (blocks_in_use < blocks_size / 2 && blocks_size != 2) {  // Resizes the array if the array is too large
                unique_ptr<CharacterBlock[]> old_blocks(blocks.release());
                blocks = make_unique<CharacterBlock[]>(blocks_size / 2);
                for (size_t i = 0; i < blocks_in_use; i++) {
                    blocks[i] = old_blocks[i];
                }
                blocks_size /= 2;
            }

            return;
        }
    }
    return;
}
