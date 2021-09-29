#include "player.h"

using namespace std;

// TODO: implement member functions
// Adds points to player's score
void Player::add_points(size_t points) {
    this->points += points;  // adds passed points to the players points
}

// Subtracts points from player's score
void Player::subtract_points(size_t points) {
    this->points -= points;  // subtracts passed points from the players points
}

size_t Player::get_points() const { return points; }

const std::string& Player::get_name() const { return name; }

// Returns the number of tiles in a player's hand.
size_t Player::count_tiles() const {
    return this->player_tiles.count_tiles();  // calls count tiles on the players tile collection
}

// Removes tiles from player's hand.
void Player::remove_tiles(const std::vector<TileKind>& tiles) {
    vector<TileKind>::const_iterator it;
    for (it = tiles.begin(); it != tiles.end(); it++) {  // iterates through vector of tiles
        this->player_tiles.remove_tile(*it);             // removes individual tiles
    }
}

// Adds tiles to player's hand.
void Player::add_tiles(const std::vector<TileKind>& tiles) {
    vector<TileKind>::const_iterator it;
    for (it = tiles.begin(); it != tiles.end(); it++) {  // iterates through vector of tiles
        this->player_tiles.add_tile(*it);                // adds individual tiles
    }
}

// Checks if player has a matching tile.
bool Player::has_tile(TileKind tile) {
    try {
        this->player_tiles.lookup_tile(tile.letter);  // attempts to look up tile
        return true;
    } catch (...) {
        return false;  // returns false if exception is thrown
    }
}

// Returns the total points of all tiles in the players hand.
unsigned int Player::get_hand_value() const { return this->player_tiles.total_points(); }

size_t Player::get_hand_size() const { return hand_size; }