
#include "computer_player.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>

bool ComputerPlayer::is_human() const { return false; }

void ComputerPlayer::left_part(
        Board::Position anchor_pos,
        std::string partial_word,
        Move partial_move,
        std::shared_ptr<Dictionary::TrieNode> node,
        size_t limit,
        TileCollection& remaining_tiles,
        std::vector<Move>& legal_moves,
        const Board& board) const {
    // HW5: IMPLEMENT THIS

    extend_right(
            anchor_pos,
            partial_word,
            partial_move,
            node,
            remaining_tiles,
            legal_moves,
            board);  // calls extend right on the partially made moves

    if (partial_word.size() == limit || partial_word.size() == this->count_tiles() - 1 || partial_move.column == 0
        || partial_move.row == 0 || partial_move.column == board.columns - 1 || partial_move.row == board.rows - 1
        || node == nullptr) {  // base case, checks for prefixes as big as the anchor limit, or if the prefix is going
                               // to go off the board
        return;
    }

    TileCollection::const_iterator it
            = TileCollection::const_iterator(remaining_tiles.cbegin());  // iterates over the remaining tiles
    for (; it != remaining_tiles.cend(); it++) {
        TileCollection remove = remaining_tiles;  // makes a copy of remaining tiles to remove the current tile
        remove.remove_tile(*it);                  // removes current tile
        if (partial_move.direction == Direction::ACROSS) {
            partial_move.column--;  // decrements column of current move as prefix gets longer
        } else {
            partial_move.row--;  // decrements row of current move as prefix gets longer
        }
        if (it->letter == it->BLANK_LETTER) {  // checks for blank tiles
            std::map<char, std::shared_ptr<Dictionary::TrieNode>>::iterator
                    it2;  // iterates over the possibilities of the blank tile
            for (it2 = node->nexts.begin(); it2 != node->nexts.end(); it2++) {
                partial_word.push_back(it2->first);   // pushes back current blank letter tile
                TileKind blank = *it;                 // makes a copy of the read only blank tile
                blank.assigned = it2->first;          // sets assigned of the copied blank tile
                partial_move.tiles.push_back(blank);  // pushes blank tile to currnet move
                left_part(
                        anchor_pos,
                        partial_word,
                        partial_move,
                        it2->second,
                        limit,
                        remove,
                        legal_moves,
                        board);           // recursive call with new extended word
                partial_word.pop_back();  // pops word and move to backtrack
                partial_move.tiles.pop_back();
            }
            if (partial_move.direction == Direction::ACROSS) {  // reincrements row or column to backtrack
                partial_move.column++;
            } else {
                partial_move.row++;
            }
            continue;
        }
        partial_word.push_back(it->letter);  // pushes current non-blank letter to the word

        if (node->nexts.find(it->letter)
            != node->nexts.end()) {             // if this letter makes a valid prefix with the current partial word
            partial_move.tiles.push_back(*it);  // pushes current tile to partial move
            left_part(
                    anchor_pos,
                    partial_word,
                    partial_move,
                    node->nexts[it->letter],
                    limit,
                    remove,
                    legal_moves,
                    board);                 // recursive call with new extended word
            partial_move.tiles.pop_back();  // pops partial move to backtrack
        }

        if (partial_move.direction == Direction::ACROSS) {  // reincrements row or column to backtrack
            partial_move.column++;
        } else {
            partial_move.row++;
        }
        partial_word.pop_back();  // pops current letter from partial word to backtrack
    }
}

void ComputerPlayer::extend_right(
        Board::Position square,
        std::string partial_word,
        Move partial_move,
        std::shared_ptr<Dictionary::TrieNode> node,
        TileCollection& remaining_tiles,
        std::vector<Move>& legal_moves,
        const Board& board) const {
    // HW5: IMPLEMENT THIS

    if (node->is_final
        && partial_move.tiles.size() != 0) {  // checks for valid partial words that have placed at least one tile
        legal_moves.push_back(partial_move);
    }
    if (!board.is_in_bounds(square) || node == nullptr) {  // basecase for out of bounds squares or null pointers
        return;
    }
    if (remaining_tiles.count_tiles() == 0
        && !board.in_bounds_and_has_tile(square)) {  // base case for when there are no more tiles to place and we are
                                                     // not on an already placed tile
        return;
    }
    if (node->nexts.empty()) {  // if there are no more valid words to be made with this prefix, return
        return;
    }

    if (board.in_bounds_and_has_tile(square)) {  // checks for already placed board squares
        if (node->nexts.find(board.letter_at(square))
            == node->nexts.end()) {  // if no valid word can be made with this letter and prefix, return
            return;
        }
        partial_word.push_back(board.letter_at(square));  // pushes current letter to partial word
        std::shared_ptr<Dictionary::TrieNode> next_node
                = node->nexts[board.letter_at(square)];  // increments the dictionary node

        if (partial_move.direction == Direction::ACROSS) {
            Board::Position right = Board::Position(square.row, square.column + 1);  // creates new right position
            extend_right(
                    right,
                    partial_word,
                    partial_move,
                    next_node,
                    remaining_tiles,
                    legal_moves,
                    board);  // recursive call to new extended word
        } else {
            Board::Position down = Board::Position(square.row + 1, square.column);  // creates new down position
            extend_right(
                    down,
                    partial_word,
                    partial_move,
                    next_node,
                    remaining_tiles,
                    legal_moves,
                    board);  // recursive call to new extended word
        }
        return;
    }

    TileCollection::const_iterator it = TileCollection::const_iterator(remaining_tiles.cbegin());
    for (; it != remaining_tiles.cend(); it++) {  // iterates over remaining tiles
        TileCollection remove = remaining_tiles;  // makes a copy of remaining tiles to remove
        remove.remove_tile(*it);                  // removes current tile from copy

        if (it->letter == it->BLANK_LETTER) {  // if the current tile is blank
            std::map<char, std::shared_ptr<Dictionary::TrieNode>>::iterator it2;
            std::shared_ptr<Dictionary::TrieNode> next_node;
            for (it2 = node->nexts.begin(); it2 != node->nexts.end(); it2++) {  // iterates over the possible letters of
                partial_word.push_back(it2->first);                             // pushes current letter to partial word
                TileKind blank = *it;                 // makes a copy of the blank read-only tile
                blank.assigned = it2->first;          // assigns the current iterated character
                partial_move.tiles.push_back(blank);  // pushes copied blank tile to partial move
                next_node = it2->second;              // updates dictionary node

                if (partial_move.direction == Direction::ACROSS) {
                    Board::Position right
                            = Board::Position(square.row, square.column + 1);  // creates new right position
                    extend_right(
                            right,
                            partial_word,
                            partial_move,
                            next_node,
                            remove,
                            legal_moves,
                            board);  // recursive call to new extended move
                } else {
                    Board::Position down = Board::Position(square.row + 1, square.column);  // creates new down position
                    extend_right(
                            down,
                            partial_word,
                            partial_move,
                            next_node,
                            remove,
                            legal_moves,
                            board);  // recursive call to new extended word
                }

                partial_word.pop_back();        // pops partial word to backtrack
                partial_move.tiles.pop_back();  // pops partial move to backtrack
            }
            continue;
        }

        else if (node->nexts.find(it->letter) != node->nexts.end()) {  // if the new letter creates a valid prefix
            std::shared_ptr<Dictionary::TrieNode> next_node = node->nexts[it->letter];  // updates dictionary node
            partial_word.push_back(it->letter);  // pushes current letter to partial word
            partial_move.tiles.push_back(*it);   // pushes current tile to partial move
            if (partial_move.direction == Direction::ACROSS) {
                Board::Position right = Board::Position(square.row, square.column + 1);  // creates new right position
                extend_right(
                        right,
                        partial_word,
                        partial_move,
                        next_node,
                        remove,
                        legal_moves,
                        board);  // recursive call to new extended word
            } else {
                Board::Position down = Board::Position(square.row + 1, square.column);  // creates new down position
                extend_right(
                        down,
                        partial_word,
                        partial_move,
                        next_node,
                        remove,
                        legal_moves,
                        board);  // recursive call to new extended word
            }
            partial_move.tiles.pop_back();  // pops partial move to backtrack
            partial_word.pop_back();        // pops partial move to backtrack
        }
    }
}

Move ComputerPlayer::get_move(const Board& board, const Dictionary& dictionary) const {
    std::vector<Move> legal_moves;
    std::vector<Board::Anchor> anchors
            = board.get_anchors();  // calls get anchors to get a vector of anchors given the board
    // HW5: IMPLEMENT THIS
    std::vector<Board::Anchor>::iterator it;
    TileCollection tiles = this->player_tiles;

    for (it = anchors.begin(); it != anchors.end(); it++) {  // iterates over anchors
        Move move_part;
        move_part.direction = it->direction;
        std::string string_part = "";
        std::shared_ptr<Dictionary::TrieNode> node = dictionary.get_root();
        move_part.column = it->position.column;
        move_part.row = it->position.row;
        move_part.kind = MoveKind::PLACE;
        if (it->limit > 0) {  // if this anchor has a non-zero limit, you must call left part
            left_part(it->position, string_part, move_part, node, it->limit, tiles, legal_moves, board);
        } else {  // if this anchor has a liimt of zero, you do not call left part
            if (it->direction == Direction::ACROSS) {
                bool valid_read = true;  // bool flag to check for valid board prefixes
                Board::Position temp = Board::Position(
                        it->position.row, it->position.column - 1);  // creates a temporary, iterated position
                while (board.in_bounds_and_has_tile(temp)
                       && !board.is_anchor_spot(temp)) {  // iterates over pre placed prefix
                    temp.column--;
                }
                temp.column++;
                while (temp != it->position) {                     // while you are still on a preplaced tile
                    string_part.push_back(board.letter_at(temp));  // pushes current letter to string part
                    if (node->nexts.find(board.letter_at(temp)) != node->nexts.end()) {  // checks for valid prefix
                        node = node->nexts[board.letter_at(temp)];                       // updates dictionary node
                    } else {
                        valid_read = false;  // if no valid prefix, set flag to false
                    }
                    temp.column++;
                }
                if (!valid_read) {  // if non valid word, return
                    continue;
                }
                extend_right(
                        it->position,
                        string_part,
                        move_part,
                        node,
                        tiles,
                        legal_moves,
                        board);  // call extend right with built prefix
            } else {
                bool valid_read = true;  // bool flag to check for valid board prefixes
                Board::Position up = Board::Position(it->position.row - 1, it->position.column);
                Board::Position temp = up;  // creates temporary, iterated position
                while (board.in_bounds_and_has_tile(temp)
                       && !board.is_anchor_spot(temp)) {  // iterates over pre placed board tiles
                    temp.row--;
                }
                temp.row++;
                while (temp != it->position) {                     // while you are still on a preplaced tile
                    string_part.push_back(board.letter_at(temp));  // pushes current letter to string part
                    if (node->nexts.find(board.letter_at(temp)) != node->nexts.end()) {  // checks for valid prefix
                        node = node->nexts[board.letter_at(temp)];                       // updates dictionary node
                    } else {
                        valid_read = false;  // if no valid prefix, set flag to false
                    }
                    temp.row++;
                }
                if (!valid_read) {  // if non valid word, return
                    continue;
                }
                extend_right(
                        it->position,
                        string_part,
                        move_part,
                        node,
                        tiles,
                        legal_moves,
                        board);  // call extend right with built prefix
            }
        }
    }
    return get_best_move(legal_moves, board, dictionary);
}

Move ComputerPlayer::get_best_move(
        std::vector<Move> legal_moves, const Board& board, const Dictionary& dictionary) const {
    Move best_move = Move();              // Pass if no move found
                                          // HW5: IMPLEMENT THIS
    unsigned int highest_move_score = 0;  // sets a variable to keep track of the current highest move score
    std::vector<Move>::iterator it;
    for (it = legal_moves.begin(); it != legal_moves.end(); it++) {  // iterates over legal moves made in get move
        bool dict_fail = false;  // sets flag to check if all words made are in the dicitonary
        try {
            PlaceResult test_move = board.test_place(*it);  // attempts to play the current word on the board
        } catch (...) {
            continue;  // if the move is not valid, move on to the next one
        }
        PlaceResult current_move = board.test_place(*it);  // if the word is valid, create its place result

        if (!current_move.valid) {  // check if the word is valid
            continue;
        }
        for (unsigned int i = 0; i < current_move.words.size(); i++) {  // iterate over all created words
            if (!dictionary.is_word(current_move.words[i])) {           // check if word is not in the dictionary
                dict_fail = true;                                       // set fail flag to true
            }
        }
        if (dict_fail) {
            continue;  // if you made a non valid word, continue
        }

        if ((size_t)it->tiles.size() == this->get_hand_size()) {  // if they play their full hand, add the 50 point
                                                                  // bonus
            current_move.points += 50;
        }

        if (current_move.points > highest_move_score) {  // if the current move has more points than highest points
            highest_move_score = current_move.points;    // update hightest move score
            best_move = *it;                             // update best move
        }
    }
    return best_move;  // returns best move
}