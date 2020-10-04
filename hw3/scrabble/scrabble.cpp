#include "scrabble.h"

#include "formatting.h"
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

// Given to you. this does not need to be changed
Scrabble::Scrabble(const ScrabbleConfig& config)
        : hand_size(config.hand_size),
          minimum_word_length(config.minimum_word_length),
          tile_bag(TileBag::read(config.tile_bag_file_path, config.seed)),
          board(Board::read(config.board_file_path)),
          dictionary(Dictionary::read(config.dictionary_file_path)) {}

// Game Loop should cycle through players and get and execute that players move
// until the game is over.
void Scrabble::game_loop() {
    // TODO: implement this.
    int turn_count = 0;                       // variable to keep track of which player is up
    unsigned int consecutive_pass_count = 0;  // variable to see if each player has passed consecutively
    Move player_move;
    while (1) {
        shared_ptr<Player> current_player
                = this->players[turn_count % this->players.size()];  // sets current_player to the player of the current
                                                                     // move
        cout << "SCOREBOARD:" << endl;
        for (auto player : this->players) {  // prints scoreboard at the beginning of every turn;
            cout << SCORE_COLOR << player->get_points() << rang::style::reset << " | " << PLAYER_NAME_COLOR
                 << player->get_name() << rang::style::reset << endl;
        }
        while (1) {
            try {
                player_move = current_player->get_move(
                        this->board, this->dictionary);  // attempts to get a valid move from the player
                break;
            } catch (out_of_range e) {
                cerr << e.what()
                     << endl;  // if the player gives an invalid move, the player will be prompted for another move
            }
        }
        if (player_move.kind == MoveKind::PASS) {
            consecutive_pass_count++;  // if the player passes, increment the pass count
            cout << "Your current score: " << SCORE_COLOR << current_player->get_points() << rang::style::reset << endl;
            cout << endl << "Press [enter] to continue." << endl;
            ;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (consecutive_pass_count >= this->players.size()) {  // if everyone has passed, the game is over
                cout << "The game is over" << endl;
                break;
            }
            turn_count++;
        } else if (player_move.kind == MoveKind::EXCHANGE) {
            consecutive_pass_count = 0;

            current_player->remove_tiles(player_move.tiles);  // removes the passed tiles from the players hand
            current_player->add_tiles(this->tile_bag.remove_random_tiles(
                    player_move.tiles
                            .size()));  // adds the correct number of tiles to the players hand from the tile bag

            vector<TileKind>::iterator it;
            for (it = player_move.tiles.begin(); it != player_move.tiles.end(); it++) {
                this->tile_bag.add_tile(*it);  // adds exchanged tiles back to the tile bag
            }
            cout << "Your current score: " << SCORE_COLOR << current_player->get_points() << rang::style::reset << endl;
            cout << endl << "Press [enter] to continue." << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            turn_count++;
        } else if (player_move.kind == MoveKind::PLACE) {
            consecutive_pass_count = 0;

            PlaceResult move_result = board.place(player_move);  // places the valid move on the board
            current_player->add_points(move_result.points);      // adds the move's points to the correct player
            if (player_move.tiles.size()
                >= this->hand_size) {  // if the player plays their whole hand, they get the bonus points.
                current_player->add_points(this->EMPTY_HAND_BONUS);
            }
            try {
                current_player->remove_tiles(player_move.tiles);  // tries to remove the tiles from the player's hand
            } catch (out_of_range e) {
                cout << e.what() << endl;
                continue;
            }

            cout << "You gained " << SCORE_COLOR << move_result.points << rang::style::reset << " points!"
                 << endl;  // prints out summary of the player's move
            cout << "Your current score: " << SCORE_COLOR << current_player->get_points() << rang::style::reset << endl;
            cout << endl << "Press [enter] to continue." << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (current_player->count_tiles() == 0 && this->tile_bag.count_tiles() == 0) {
                cout << "Game is over" << endl;
                break;
            }
            current_player->add_tiles(this->tile_bag.remove_random_tiles(
                    player_move.tiles.size()));  // adds tiles back into the player's hand
            turn_count++;
        } else {
            cout << "Please enter a valid move!" << endl;
            continue;
        }
    }

    // Useful cout expressions with fancy colors. Expressions in curly braces, indicate values you supply.
}

void Scrabble::add_players() {
    cout << "Please input the number of players for this game (1-8)" << endl;
    int num_players;
    cin >> num_players;                        // reads in number of players
    if (num_players < 1 || num_players > 8) {  // checks if the number of players is valid.
        throw FileException("Incorrect number of players!");
    }
    string discard;
    getline(cin, discard);
    cout << "Please input the name of each player" << endl;
    for (int i = 0; i < num_players; i++) {
        string player_name;
        getline(cin, player_name);  // reads in a line for each player's name
        shared_ptr<Player> new_player = make_shared<HumanPlayer>(player_name, hand_size);  // makes a new player object
        new_player->add_tiles(this->tile_bag.remove_random_tiles(hand_size));              // gives the new player tiles
        this->players.push_back(new_player);  // pushes new player back onto the vector of players
    }
}

// Performs final score subtraction. Players lose points for each tile in their
// hand. The player who cleared their hand receives all the points lost by the
// other players.
void Scrabble::final_subtraction(vector<shared_ptr<Player>>& plrs) {
    // TODO: implement this method.
    // Do not change the method signature.
    int total_hand_value = 0;
    shared_ptr<Player> winning_player;
    vector<shared_ptr<Player>>::iterator it;
    for (it = plrs.begin(); it != plrs.end(); it++) {  // iterates through vector of players
        if ((*it)->get_hand_value()
            == 0) {  // if a player has no tiles at the end of the game, they recieve extra points
            winning_player = *it;
            continue;
        }
        if ((*it)->get_hand_value()
            <= (*it)->get_points()) {  // checks to see if the value of the hand is larger than the players points
            (*it)->subtract_points((*it)->get_hand_value());
        } else {
            (*it)->subtract_points((*it)->get_points());  // subtracts hand value from points
        }
        total_hand_value += (*it)->get_hand_value();  // totals all point values left in all players hands
    }
    if (winning_player) {  // if there is a winning player, they will recieve additional points equal to the
                           // total_hand_value
        winning_player->add_points(total_hand_value);
    }
    return;
}

// You should not need to change this function.
void Scrabble::print_result() {
    // Determine highest score

    size_t max_points = 0;
    for (auto player : this->players) {
        if (player->get_points() > max_points) {
            max_points = player->get_points();
        }
    }

    // Determine the winner(s) indexes
    vector<shared_ptr<Player>> winners;
    for (auto player : this->players) {
        if (player->get_points() >= max_points) {
            winners.push_back(player);
        }
    }

    cout << (winners.size() == 1 ? "Winner:" : "Winners: ");
    for (auto player : winners) {
        cout << SPACE << PLAYER_NAME_COLOR << player->get_name();
    }
    cout << rang::style::reset << endl;

    // now print score table
    cout << "Scores: " << endl;
    cout << "---------------------------------" << endl;

    // Justify all integers printed to have the same amount of character as the high score, left-padding with spaces
    cout << setw(static_cast<uint32_t>(floor(log10(max_points) + 1)));

    for (auto player : this->players) {
        cout << SCORE_COLOR << player->get_points() << rang::style::reset << " | " << PLAYER_NAME_COLOR
             << player->get_name() << rang::style::reset << endl;
    }
}

// You should not need to change this.
void Scrabble::main() {
    add_players();
    game_loop();
    final_subtraction(this->players);
    print_result();
}
