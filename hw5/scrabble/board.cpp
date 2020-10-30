#include "board.h"

#include "board_square.h"
#include "exceptions.h"
#include "formatting.h"
#include <fstream>
#include <iomanip>
#include <vector>

using namespace std;

bool Board::Position::operator==(const Board::Position& other) const {
    return this->row == other.row && this->column == other.column;
}

bool Board::Position::operator!=(const Board::Position& other) const {
    return this->row != other.row || this->column != other.column;
}

Board::Position Board::Position::translate(Direction direction) const { return this->translate(direction, 1); }

Board::Position Board::Position::translate(Direction direction, ssize_t distance) const {
    if (direction == Direction::DOWN) {
        return Board::Position(this->row + distance, this->column);
    } else {
        return Board::Position(this->row, this->column + distance);
    }
}

char Board::letter_at(Position p) const {
    if (this->at(p).get_tile_kind().letter == this->at(p).get_tile_kind().BLANK_LETTER) {  // if the tile is blank
        return this->at(p).get_tile_kind().assigned;                                       // return the assigned letter
    }
    return this->at(p).get_tile_kind().letter;  // return the letter at the square
}

TileKind Board::tile_at(Position p) const {
    return this->at(p).get_tile_kind();  // returns the tilekind at the passed position
}

bool Board::is_anchor_spot(Position p) const {
    if (p == start && !(this->at(start).has_tile())) {  // checks if p is the start square and the start square has not
                                                        // been covered (first move)
        return true;
    }
    if (!is_in_bounds(p) || this->at(p).has_tile()) {  // checks if p is out of bounds or if p already has a tile
        return false;                                  // not an anchor square
    }
    Position left = Position(p.row, p.column - 1);
    Position right = Position(p.row, p.column + 1);
    Position up = Position(p.row - 1, p.column);
    Position down = Position(p.row + 1, p.column);
    if (in_bounds_and_has_tile(left) || in_bounds_and_has_tile(right) || in_bounds_and_has_tile(up)
        || in_bounds_and_has_tile(down)) {  // checks if our passed position has at least one adjacent tile
        return true;                        // is an anchor square
    }
    return false;
}

vector<Board::Anchor> Board::get_anchors() const {
    vector<Board::Anchor> anchor_vec;
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < columns; j++) {  // iterates over the entire board
            Position current = Position(i, j);  // creates a current position
            if (is_anchor_spot(current)) {      // checks if the current postiion is an anchor
                Position vertical_limit_check = Position(i - 1, j);
                size_t vertical_limit = 0;
                while (is_in_bounds(vertical_limit_check) && !is_anchor_spot(vertical_limit_check)
                       && !this->at(vertical_limit_check)
                                   .has_tile()) {  // while there are still empty, non anchor tiles above the tile
                    vertical_limit_check.row--;
                    vertical_limit++;  // increments anchor limit
                }
                anchor_vec.push_back(Anchor(current, Direction::DOWN, vertical_limit));  // pushes anchor to the vector

                Position horizontal_limit_check = Position(i, j - 1);
                size_t horizontal_limit = 0;
                while (is_in_bounds(horizontal_limit_check) && !is_anchor_spot(horizontal_limit_check)
                       && !this->at(horizontal_limit_check).has_tile()) {  // while there are still empty, non anchor
                                                                           // tiles to the left of the tile
                    horizontal_limit_check.column--;
                    horizontal_limit++;  // increments anchor limit
                }
                anchor_vec.push_back(
                        Anchor(current, Direction::ACROSS, horizontal_limit));  // pushes anchor to the vector
            }
        }
    }
    return anchor_vec;  // returns vector of anchors
}

Board Board::read(const string& file_path) {
    ifstream file(file_path);
    if (!file) {
        throw FileException("cannot open board file!");
    }

    size_t rows;
    size_t columns;
    size_t starting_row;
    size_t starting_column;
    file >> rows >> columns >> starting_row >> starting_column;  // reads in game data from the config file
    Board board(rows, columns, starting_row, starting_column);   // creates a board with given dimensions
    string tilerow;

    // TODO: complete implementation of reading in board from file here.
    BoardSquare triple_word = BoardSquare(1, 3);    // example of a triple word square
    BoardSquare double_word = BoardSquare(1, 2);    // double word square
    BoardSquare triple_letter = BoardSquare(3, 1);  // triple letter square
    BoardSquare double_letter = BoardSquare(2, 1);  // double letter square
    BoardSquare no_multiplier = BoardSquare(1, 1);  // normal square
    for (unsigned int i = 0; i < rows; i++) {
        vector<BoardSquare> row;  // creates a vector to store a row of the board
        file >> tilerow;
        for (unsigned int j = 0; j < columns; j++) {  // iterates through the row string
            if (tilerow[j] == 't') {
                row.push_back(triple_word);
            } else if (tilerow[j] == 'd') {
                row.push_back(double_word);
            } else if (tilerow[j] == '2') {
                row.push_back(double_letter);
            } else if (tilerow[j] == '3') {
                row.push_back(triple_letter);
            } else {
                row.push_back(no_multiplier);
            }
        }
        board.squares.push_back(row);  // pushes back complete row into the board vector
    }
    return board;
}

size_t Board::get_move_index() const { return this->move_index; }

PlaceResult Board::test_place(const Move& move) const {
    // TODO: complete implementation here
    vector<string> place_result_words;
    unsigned int place_result_points = 0;
    bool word_adjacent = false;
    int word_multiplier = 1;

    if (move.kind != MoveKind::PLACE) {
        return PlaceResult("Did not choose to place tiles!");
    }

    else {

        int placerange = move.tiles.size();  // calculates the size of the move

        if (!this->at(start).has_tile()) {  // checks to see if the starting square is still open, signalling that this
                                            // is the opening move
            if (move.column != this->start.column
                && move.row != this->start.row) {  // if the move doesn't share a starting row or column with start, it
                                                   // will never cover it
                return PlaceResult("Must place word on the starting tile!");
            }
            bool covers_start = false;  // boolean to check whether or not the word covers the start
            if (move.column == this->start.column
                && move.direction == Direction::DOWN) {  // case for when the word is in the same column as start
                for (unsigned int i = move.row; i < (move.row + placerange); i++) {
                    Position currentposition = Position(i, move.column);
                    if (!this->is_in_bounds(currentposition)) {
                        return PlaceResult("Word goes off the board");
                    }
                    if (currentposition == this->start) {  // checks to see if the word covers the start
                        covers_start = true;
                        word_adjacent = true;
                    }
                }
            } else if (move.row == this->start.row && move.direction == Direction::ACROSS) {  // case for when the
                                                                                              // placed word is in the
                                                                                              // same row as start
                for (unsigned int i = move.column; i < (move.column + placerange); i++) {
                    Position currentposition = Position(move.row, i);
                    if (!this->is_in_bounds(currentposition)) {
                        return PlaceResult("Word goes off the board");
                    }
                    if (currentposition == this->start) {  // checks to see if the word covers the start
                        covers_start = true;
                        word_adjacent = true;
                    }
                }
            }
            if (!covers_start) {  // checks to see if the boolean was flipped to true
                return PlaceResult("Must place word on the starting tile!");
            }
        }

        Position start = Position(move.row, move.column);
        if (this->at(start)
                    .has_tile()) {  // checks if the player is attempting to start their move on an occupied square
            return PlaceResult("Cannot place a word here!");
        }

        if (move.direction == Direction::ACROSS) {  // move across
            for (unsigned int i = move.column; i < (move.column + placerange); i++) {
                Position currentposition = Position(move.row, i);
                Position left = Position(move.row, i - 1);
                Position right = Position(move.row, i + 1);
                Position up = Position(move.row + 1, i);
                Position down = Position(move.row - 1, i);
                if (in_bounds_and_has_tile(left) || in_bounds_and_has_tile(right) || in_bounds_and_has_tile(up)
                    || in_bounds_and_has_tile(down)) {  // checks if the tile is adjacent to any placed tiles
                    word_adjacent = true;
                }
                if (this->at(currentposition)
                            .has_tile()) {  // if the move passes over a placed tile, increment the length of the word
                    placerange++;
                    word_adjacent = true;
                    continue;
                }
                if (!this->is_in_bounds(currentposition)) {  // checks if move goes off the board
                    return PlaceResult("Word goes off the board");
                }
                word_multiplier *= this->at(currentposition).word_multiplier;  // totals word multipliers
            }
            if (!word_adjacent) {  // returns invalid if the word is not adjacent to another word.
                return PlaceResult("Word must be adjacent to another word!");
            }

            Position across_word_check = Position(move.row, move.column - 1);
            while (in_bounds_and_has_tile(
                    across_word_check)) {  // iterates to the left to check for existing, prepending letters.
                across_word_check.column--;
            }
            across_word_check.column++;
            string wordacross;
            int word_across_points = 0;
            while (across_word_check.column != move.column) {  // iterates over letters coming before the placed tiles
                word_across_points
                        += this->at(across_word_check).get_tile_kind().points;  // adds tile points to word point total
                if (this->at(across_word_check).get_tile_kind().letter
                    == this->at(across_word_check).get_tile_kind().BLANK_LETTER) {  // checks for blank tile
                    wordacross.push_back(this->at(across_word_check).get_tile_kind().assigned);
                } else {
                    wordacross.push_back(this->at(across_word_check).get_tile_kind().letter);
                }
                across_word_check.column++;
            }
            for (unsigned int i = 0; i < move.tiles.size(); i++) {      // iterates over placed tiles
                if (this->in_bounds_and_has_tile(across_word_check)) {  // if crossing over a placed tile
                    word_across_points += this->at(across_word_check)
                                                  .get_tile_kind()
                                                  .points;  // sums points but WITHOUT multipliers
                    if (this->at(across_word_check).get_tile_kind().letter
                        == this->at(across_word_check).get_tile_kind().BLANK_LETTER) {
                        wordacross.push_back(this->at(across_word_check).get_tile_kind().assigned);
                    } else {
                        wordacross.push_back(this->at(across_word_check).get_tile_kind().letter);
                    }
                    i--;  // decrements i as we did not find a tile in the array
                } else {
                    if (move.tiles[i].letter == move.tiles[i].BLANK_LETTER) {  // checks for blank tiles
                        wordacross.push_back(move.tiles[i].assigned);
                    } else {
                        wordacross.push_back(move.tiles[i].letter);
                    }
                    word_across_points += move.tiles[i].points
                                          * this->at(across_word_check)
                                                    .letter_multiplier;  // adds tile points with letter multiplier
                }
                across_word_check.column++;  // increments position
            }
            while (in_bounds_and_has_tile(across_word_check)) {  // iterates over tiles following placed tiles
                word_across_points += this->at(across_word_check).get_points();
                if (this->at(across_word_check).get_tile_kind().letter
                    == this->at(across_word_check).get_tile_kind().BLANK_LETTER) {  // checks for blank tile
                    wordacross.push_back(this->at(across_word_check).get_tile_kind().assigned);
                } else {
                    wordacross.push_back(this->at(across_word_check).get_tile_kind().letter);
                }
                across_word_check.column++;
            }
            if (wordacross.size() > 1) {
                place_result_words.push_back(wordacross);                       // adds word to the word vector
                place_result_points += (word_across_points * word_multiplier);  // adds points times the word multiplier
            }
            word_multiplier = 1;
            int j = -1;
            for (unsigned int i = move.column; i < (move.column + placerange);
                 i++) {  // iterates over placed tiles to find vertical words
                Position currentposition = Position(move.row, i);
                j++;
                if (this->in_bounds_and_has_tile(currentposition)) {  // skip over already placed tiles
                    continue;
                    j--;
                }
                word_multiplier *= this->at(currentposition).word_multiplier;  // calculates word multiplier
                Position currentposition_up = Position(move.row - 1, i);
                Position currentposition_down = Position(move.row + 1, i);
                if (!in_bounds_and_has_tile(currentposition_up)
                    && !in_bounds_and_has_tile(currentposition_down)) {  // if there is no vertical word, continue
                    word_multiplier = 1;
                    continue;
                }

                while (in_bounds_and_has_tile(currentposition_up)) {  // backtraces to find the top of the vertical
                                                                      // word=
                    currentposition_up.row--;
                }

                currentposition_up.row++;
                string verticalword;
                int vertical_word_points = 0;
                while (currentposition_up.row != move.row) {  // iterates over preceding tiles
                    if (this->at(currentposition_up).get_tile_kind().letter
                        == this->at(currentposition_up).get_tile_kind().BLANK_LETTER) {  // checks for blank tiles
                        verticalword.push_back(this->at(currentposition_up).get_tile_kind().assigned);
                    } else {
                        verticalword.push_back(this->at(currentposition_up).get_tile_kind().letter);
                    }
                    vertical_word_points += this->at(currentposition_up).get_tile_kind().points;
                    currentposition_up.row++;
                }
                // CODE DELETION
                if (move.tiles[j].letter == move.tiles[j].BLANK_LETTER) {  // checks for blank tiles
                    verticalword.push_back(move.tiles[j].assigned);
                } else {
                    verticalword.push_back(move.tiles[j].letter);
                }
                vertical_word_points
                        += move.tiles[i - (move.column)].points
                           * (this->at(currentposition_up).letter_multiplier);  // Adds points for placed tiles
                currentposition_up.row++;

                while (in_bounds_and_has_tile(currentposition_up)) {  // iterates over succeeding tiles
                    vertical_word_points += this->at(currentposition_up).get_tile_kind().points;
                    if (this->at(currentposition_up).get_tile_kind().letter
                        == this->at(currentposition_up).get_tile_kind().BLANK_LETTER) {  // checks for blank tiles
                        verticalword.push_back(this->at(currentposition_up).get_tile_kind().assigned);
                    } else {
                        verticalword.push_back(this->at(currentposition_up).get_tile_kind().letter);
                    }
                    currentposition_up.row++;
                }

                place_result_words.push_back(verticalword);                       // pushes back vertical word
                place_result_points += (vertical_word_points * word_multiplier);  // multiplies word by word multiplier
                word_multiplier = 1;
            }
            return PlaceResult(place_result_words, place_result_points);
        } else {  // vertical move
            for (unsigned int i = move.row; i < (move.row + placerange); i++) {
                Position currentposition = Position(i, move.column);
                Position left = Position(i, move.column - 1);
                Position right = Position(i, move.column + 1);
                Position up = Position(i - 1, move.column);
                Position down = Position(i + 1, move.column);
                if (in_bounds_and_has_tile(left) || in_bounds_and_has_tile(right) || in_bounds_and_has_tile(up)
                    || in_bounds_and_has_tile(down)) {  // checks for tile adjacency
                    word_adjacent = true;
                }

                if (this->at(currentposition).has_tile()) {  // checks for already placed tiles, sets adjacency to true;
                    placerange++;
                    word_adjacent = true;
                    continue;
                }
                if (!this->is_in_bounds(currentposition)) {  // checks if the word goes off the board
                    return PlaceResult("Word goes off the board");
                }
                word_multiplier
                        *= this->at(currentposition).word_multiplier;  // calculates word multiplier of vertical word
            }
            if (!word_adjacent) {
                return PlaceResult("Word must be adjacent to another word!");
            }

            Position vertical_word_check = Position(move.row - 1, move.column);
            while (in_bounds_and_has_tile(vertical_word_check)) {  // backtraces to find the top of the vertical word
                vertical_word_check.row--;
            }
            vertical_word_check.row++;
            string wordvertical;
            int word_vertical_points = 0;
            while (vertical_word_check.row != move.row) {  // iterates over preceding tiles
                word_vertical_points += this->at(vertical_word_check).get_tile_kind().points;
                if (this->at(vertical_word_check).get_tile_kind().letter
                    == this->at(vertical_word_check).get_tile_kind().BLANK_LETTER) {  // checks for blank tiles
                    wordvertical.push_back(this->at(vertical_word_check).get_tile_kind().assigned);
                } else {
                    wordvertical.push_back(this->at(vertical_word_check).get_tile_kind().letter);
                }
                vertical_word_check.row++;
            }
            for (unsigned int i = 0; i < move.tiles.size(); i++) {        // iterates over the placed tiles
                if (this->in_bounds_and_has_tile(vertical_word_check)) {  // checks for existing tiles
                    word_vertical_points
                            += this->at(vertical_word_check).get_tile_kind().points;  // adds points without multipliers
                    if (this->at(vertical_word_check).get_tile_kind().letter
                        == this->at(vertical_word_check).get_tile_kind().BLANK_LETTER) {  // checks for blank tiles
                        wordvertical.push_back(this->at(vertical_word_check).get_tile_kind().assigned);
                    } else {
                        wordvertical.push_back(this->at(vertical_word_check).get_tile_kind().letter);
                    }
                    vertical_word_check.row++;
                    i--;  // decrements i because we have not checked a placed tile
                    continue;
                } else {
                    if (move.tiles[i].letter == move.tiles[i].BLANK_LETTER) {  // checks for blank tile
                        wordvertical.push_back(move.tiles[i].assigned);
                    } else {
                        wordvertical.push_back(move.tiles[i].letter);
                    }
                }
                word_vertical_points += move.tiles[i].points * this->at(vertical_word_check).letter_multiplier;
                vertical_word_check.row++;
            }
            while (in_bounds_and_has_tile(vertical_word_check)) {  // iterates over any succeeding tiles
                word_vertical_points += this->at(vertical_word_check).get_tile_kind().points;
                if (this->at(vertical_word_check).get_tile_kind().letter
                    == this->at(vertical_word_check).get_tile_kind().BLANK_LETTER) {
                    wordvertical.push_back(this->at(vertical_word_check).get_tile_kind().assigned);
                } else {
                    wordvertical.push_back(this->at(vertical_word_check).get_tile_kind().letter);
                }
                vertical_word_check.row++;
            }
            if (wordvertical.size() > 1) {
                place_result_words.push_back(wordvertical);                       // adds vertical word
                place_result_points += (word_vertical_points * word_multiplier);  // adds points times word multiplier
            }
            word_multiplier = 1;
            int j = -1;                                                   // ADDED
            for (unsigned int i = move.row; i < (move.row + placerange);  // CHANGED
                 i++) {  // iterates over placed tiles to check for horizontal words
                j++;     // ADDED
                Position currentposition = Position(i, move.column);
                if (this->in_bounds_and_has_tile(currentposition)) {  // if there is an existing tile, we will not
                    j--;                                              // calculate the horizontal word
                    continue;
                }
                word_multiplier *= this->at(currentposition).word_multiplier;
                Position currentposition_left = Position(i, move.column - 1);
                Position currentposition_right = Position(i, move.column + 1);
                if (!in_bounds_and_has_tile(currentposition_left)
                    && !in_bounds_and_has_tile(currentposition_right)) {  // if there is no horiontal word, continue
                    word_multiplier = 1;
                    continue;
                }
                while (in_bounds_and_has_tile(
                        currentposition_left)) {  // backtraces to find the beginning of the horizontal word
                    currentposition_left.column--;
                }
                currentposition_left.column++;
                string horizontalword;
                int horizontal_word_points = 0;
                while (currentposition_left.column != move.column) {  // iterates over preceding tiles
                    if (this->at(currentposition_left).get_tile_kind().letter
                        == this->at(currentposition_left).get_tile_kind().BLANK_LETTER) {  // checks for blank tiles
                        horizontalword.push_back(this->at(currentposition_left).get_tile_kind().assigned);
                    } else {
                        horizontalword.push_back(this->at(currentposition_left).get_tile_kind().letter);
                    }
                    horizontal_word_points += this->at(currentposition_left)
                                                      .get_tile_kind()
                                                      .points;  // adds points to the horizontal word
                    currentposition_left.column++;
                }
                // CODE DELETION
                if (move.tiles[j].letter == move.tiles[j].BLANK_LETTER) {  // checks for blank tile
                    horizontalword.push_back(move.tiles[j].assigned);
                } else {
                    horizontalword.push_back(move.tiles[j].letter);
                }
                horizontal_word_points += move.tiles[j].points * this->at(currentposition_left).letter_multiplier;
                currentposition_left.column++;
                while (in_bounds_and_has_tile(currentposition_left)) {  // iterates over succeeding tiles
                    horizontal_word_points += this->at(currentposition_left).get_tile_kind().points;
                    if (this->at(currentposition_left).get_tile_kind().letter
                        == this->at(currentposition_left).get_tile_kind().BLANK_LETTER) {  // checks for blank tiles
                        horizontalword.push_back(this->at(currentposition_left).get_tile_kind().assigned);
                    } else {
                        horizontalword.push_back(this->at(currentposition_left).get_tile_kind().letter);
                    }
                    currentposition_left.column++;
                }

                place_result_words.push_back(horizontalword);  // adds horizontal word to word vector
                place_result_points += (horizontal_word_points * word_multiplier);
                word_multiplier = 1;  // resets word multiplier for next word
            }
            return PlaceResult(
                    place_result_words,
                    place_result_points);  // Returns valid place result with vector of words and total points earned
        }
    }
}

PlaceResult Board::place(const Move& move) {
    // TODO: Complete implementation here

    PlaceResult test_result = test_place(move);  // calls test place on passed move to check for validity

    if (test_result.valid != true) {  // if move is not valid, return invalid place result
        return PlaceResult("Invalid Move!");
    }

    Position startingposition = Position(move.row, move.column);
    if (move.direction == Direction::ACROSS) {
        for (unsigned int i = 0; i < move.tiles.size(); i++) {  // iterates through placed tiles
            if (in_bounds_and_has_tile(startingposition)) {     // skips over existing tiles
                i--;
                startingposition.column++;
                continue;
            }
            this->at(startingposition).set_tile_kind(move.tiles[i]);  // sets tile on board
            startingposition.column++;
        }
    } else if (move.direction == Direction::DOWN) {  // iterates through placed tiles
        for (unsigned int i = 0; i < move.tiles.size(); i++) {
            if (in_bounds_and_has_tile(startingposition)) {  // skips over existing tiles
                i--;
                startingposition.row++;
                continue;
            }
            this->at(startingposition).set_tile_kind(move.tiles[i]);  // sets tile on board
            startingposition.row++;
        }
    }

    return test_result;
}

// The rest of this file is provided for you. No need to make changes.

BoardSquare& Board::at(const Board::Position& position) { return this->squares.at(position.row).at(position.column); }

const BoardSquare& Board::at(const Board::Position& position) const {
    return this->squares.at(position.row).at(position.column);
}

bool Board::is_in_bounds(const Board::Position& position) const {
    return position.row < this->rows && position.column < this->columns;
}

bool Board::in_bounds_and_has_tile(const Position& position) const {
    return is_in_bounds(position) && at(position).has_tile();
}

void Board::print(ostream& out) const {
    // Draw horizontal number labels
    for (size_t i = 0; i < BOARD_TOP_MARGIN - 2; ++i) {
        out << std::endl;
    }
    out << FG_COLOR_LABEL << repeat(SPACE, BOARD_LEFT_MARGIN);
    const size_t right_number_space = (SQUARE_OUTER_WIDTH - 3) / 2;
    const size_t left_number_space = (SQUARE_OUTER_WIDTH - 3) - right_number_space;
    for (size_t column = 0; column < this->columns; ++column) {
        out << repeat(SPACE, left_number_space) << std::setw(2) << column + 1 << repeat(SPACE, right_number_space);
    }
    out << std::endl;

    // Draw top line
    out << repeat(SPACE, BOARD_LEFT_MARGIN);
    print_horizontal(this->columns, L_TOP_LEFT, T_DOWN, L_TOP_RIGHT, out);
    out << endl;

    // Draw inner board
    for (size_t row = 0; row < this->rows; ++row) {
        if (row > 0) {
            out << repeat(SPACE, BOARD_LEFT_MARGIN);
            print_horizontal(this->columns, T_RIGHT, PLUS, T_LEFT, out);
            out << endl;
        }

        // Draw insides of squares
        for (size_t line = 0; line < SQUARE_INNER_HEIGHT; ++line) {
            out << FG_COLOR_LABEL << BG_COLOR_OUTSIDE_BOARD;

            // Output column number of left padding
            if (line == 1) {
                out << repeat(SPACE, BOARD_LEFT_MARGIN - 3);
                out << std::setw(2) << row + 1;
                out << SPACE;
            } else {
                out << repeat(SPACE, BOARD_LEFT_MARGIN);
            }

            // Iterate columns
            for (size_t column = 0; column < this->columns; ++column) {
                out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL;
                const BoardSquare& square = this->squares.at(row).at(column);
                bool is_start = this->start.row == row && this->start.column == column;

                // Figure out background color
                if (square.word_multiplier == 2) {
                    out << BG_COLOR_WORD_MULTIPLIER_2X;
                } else if (square.word_multiplier == 3) {
                    out << BG_COLOR_WORD_MULTIPLIER_3X;
                } else if (square.letter_multiplier == 2) {
                    out << BG_COLOR_LETTER_MULTIPLIER_2X;
                } else if (square.letter_multiplier == 3) {
                    out << BG_COLOR_LETTER_MULTIPLIER_3X;
                } else if (is_start) {
                    out << BG_COLOR_START_SQUARE;
                }

                // Text
                if (line == 0 && is_start) {
                    out << "  \u2605  ";
                } else if (line == 0 && square.word_multiplier > 1) {
                    out << FG_COLOR_MULTIPLIER << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << 'W' << std::setw(1)
                        << square.word_multiplier;
                } else if (line == 0 && square.letter_multiplier > 1) {
                    out << FG_COLOR_MULTIPLIER << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << 'L' << std::setw(1)
                        << square.letter_multiplier;
                } else if (line == 1 && square.has_tile()) {
                    char l = square.get_tile_kind().letter == TileKind::BLANK_LETTER ? square.get_tile_kind().assigned
                                                                                     : ' ';
                    out << repeat(SPACE, 2) << FG_COLOR_LETTER << square.get_tile_kind().letter << l
                        << repeat(SPACE, 1);
                } else if (line == SQUARE_INNER_HEIGHT - 1 && square.has_tile()) {
                    out << repeat(SPACE, SQUARE_INNER_WIDTH - 1) << FG_COLOR_SCORE << square.get_points();
                } else {
                    out << repeat(SPACE, SQUARE_INNER_WIDTH);
                }
            }

            // Add vertical line
            out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL << BG_COLOR_OUTSIDE_BOARD << std::endl;
        }
    }

    // Draw bottom line
    out << repeat(SPACE, BOARD_LEFT_MARGIN);
    print_horizontal(this->columns, L_BOTTOM_LEFT, T_UP, L_BOTTOM_RIGHT, out);
    out << endl << rang::style::reset << std::endl;
}
