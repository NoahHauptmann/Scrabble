#include "human_player.h"

#include "exceptions.h"
#include "formatting.h"
#include "move.h"
#include "place_result.h"
#include "rang.h"
#include "tile_kind.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace std;

// This method is fully implemented.
inline string& to_upper(string& str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

Move HumanPlayer::get_move(const Board& board, const Dictionary& dictionary) const {
    // TODO: begin your implementation here.
    string move_request;
    while (1) {  // iterates until player gives a valid move
        try {
            board.print(cout);  // prints board for player
            cout << "This is your hand:" << endl;
            this->print_hand(cout);  // prints players hand
            cout << "Please enter your desired move, " << this->get_name() << "." << endl;
            getline(cin, move_request);               // takes in an input string
            Move attempt = parse_move(move_request);  // parses move
            if (attempt.kind == MoveKind::PLACE) {
                PlaceResult place_check = board.test_place(attempt);  // checks if this is a valid place command
                if (!place_check.valid) {                             // checks for invalid move
                    throw MoveException("Word cannot be placed here");
                }
                for (unsigned int i = 0; i < place_check.words.size(); i++) {
                    if (!(dictionary.is_word(
                                place_check.words[i]))) {  // checks if all created words are in the dictionary
                        cout << place_check.words[i] << endl;
                        throw MoveException("Invalid word");
                    }
                }
            }
            return attempt;
        } catch (MoveException e) {
            cerr << e.what() << " "
                 << "please try again!" << endl;
        }
    }
}

vector<TileKind> HumanPlayer::parse_tiles(string& letters) const {
    vector<TileKind> returned_tiles;
    for (unsigned int i = 0; i < letters.size(); i++) {  // iterates over each character in the letters string
        char letter_parsed = letters[i];
        TileKind tile_parsed
                = this->player_tiles.lookup_tile(letter_parsed);  // checks if the player has the current tile
        if (find(returned_tiles.begin(), returned_tiles.end(), tile_parsed) != returned_tiles.end()) {
            if ((unsigned int)count(returned_tiles.begin(), returned_tiles.end(), tile_parsed)
                > player_tiles.count_tiles(tile_parsed) - 1) {  // checks if the player has enough of the current tile
                throw MoveException("You do not have enough of this tile!");
            }
        }
        if (tile_parsed.letter == tile_parsed.BLANK_LETTER) {  // assigns correct letter to blank tiles
            tile_parsed.assigned = letters[i + 1];
            i++;
        }
        returned_tiles.push_back(tile_parsed);  // pushes back new tile
    }
    return returned_tiles;
}

Move HumanPlayer::parse_move(string& move_string) const {
    // TODO: begin implementation here.
    stringstream move_read(move_string);  // creates string stream from the passed move string
    string move_type;
    move_read >> move_type;  // reads in move type
    move_type = to_upper(move_type);
    if (move_type == "PASS") {
        return Move();  // returns move type for pass
    } else if (move_type == "EXCHANGE") {
        string exchange_tiles;
        move_read >> exchange_tiles;
        return Move(parse_tiles(exchange_tiles));  // returns an exchange move with a vector of parsed tiles
    } else if (move_type == "PLACE") {
        string direction, letterstring;
        size_t row, column;
        Move place_attempt;

        move_read >> direction >> row >> column >> letterstring;  // reads in move dimesions
        if (direction == "|") {                                   // if direction is down, returns a down move
            place_attempt = Move(parse_tiles(letterstring), row - 1, column - 1, Direction::DOWN);
        } else if (direction == "-") {  // if direction is across, returns an across move
            place_attempt = Move(parse_tiles(letterstring), row - 1, column - 1, Direction::ACROSS);
        }
        return place_attempt;
    } else {
        throw MoveException("Must select a valid move type!");
    }
}

// This function is fully implemented.
void HumanPlayer::print_hand(ostream& out) const {
    const size_t tile_count = player_tiles.count_tiles();
    const size_t empty_tile_count = this->get_hand_size() - tile_count;
    const size_t empty_tile_width = empty_tile_count * (SQUARE_OUTER_WIDTH - 1);

    for (size_t i = 0; i < HAND_TOP_MARGIN - 2; ++i) {
        out << endl;
    }

    out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_HEADING << "Your Hand: " << endl << endl;

    // Draw top line
    out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE;
    print_horizontal(tile_count, L_TOP_LEFT, T_DOWN, L_TOP_RIGHT, out);
    out << repeat(SPACE, empty_tile_width) << BG_COLOR_OUTSIDE_BOARD << endl;

    // Draw middle 3 lines
    for (size_t line = 0; line < SQUARE_INNER_HEIGHT; ++line) {
        out << FG_COLOR_LABEL << BG_COLOR_OUTSIDE_BOARD << repeat(SPACE, HAND_LEFT_MARGIN);
        for (auto it = player_tiles.cbegin(); it != player_tiles.cend(); ++it) {
            out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL << BG_COLOR_PLAYER_HAND;

            // Print letter
            if (line == 1) {
                out << repeat(SPACE, 2) << FG_COLOR_LETTER << (char)toupper(it->letter) << repeat(SPACE, 2);

                // Print score in bottom right
            } else if (line == SQUARE_INNER_HEIGHT - 1) {
                out << FG_COLOR_SCORE << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << setw(2) << it->points;

            } else {
                out << repeat(SPACE, SQUARE_INNER_WIDTH);
            }
        }
        if (player_tiles.count_tiles() > 0) {
            out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL;
            out << repeat(SPACE, empty_tile_width) << BG_COLOR_OUTSIDE_BOARD << endl;
        }
    }

    // Draw bottom line
    out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE;
    print_horizontal(tile_count, L_BOTTOM_LEFT, T_UP, L_BOTTOM_RIGHT, out);
    out << repeat(SPACE, empty_tile_width) << rang::style::reset << endl;
}
