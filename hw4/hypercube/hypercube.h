#ifndef HYPER_CUBE_H
#define HYPER_CUBE_H

#include <fstream>
#include <queue>
#include <set>
#include <string>
#include <vector>

struct Node {
    std::string word;
    std::string pred;
    int g;
    int h;

    Node(const std::string& word, const std::string pred, int g) : word(word), pred(pred), g(g), h(0) {
        h = 0;
        for (int i = 0; i < word.size(); i++) {  // iterates over the word
            if (word[i] == '0') {                // counts how many bits are wrong/how many zeroes are in the word
                h++;
            }
        }
    }
};

struct NodePriority {
    bool operator()(const Node& lhs, const Node& rhs) {
        // TODO: implement priority for nodes.
        if ((lhs.g + lhs.h) == (rhs.g + rhs.h)) {
            if (lhs.h == rhs.h) {
                return stoi(lhs.word, nullptr, 2)
                       > stoi(rhs.word,
                              nullptr,
                              2);  // if the f and h values are the same, compare the binary contents of the string
            }
            return lhs.h > rhs.h;  // if f values are the same, compare the h values
        }
        return (lhs.g + lhs.h) > (rhs.g + rhs.h);  // compares the 2 f values
    }
};

struct SearchResult {
    // Whether or not there is a path.
    bool found;

    // The node labels on the path.
    std::vector<std::string> path;

    // The number of expansions made during search.
    size_t expansions;

    // Convenience for if found.
    SearchResult(std::vector<std::string> path, size_t expansions) : found(true), path(path), expansions(expansions) {}

    // Convenience for if not found.
    SearchResult(bool found, size_t expansions) : found(found), expansions(expansions) {}
};

class HyperCube {
  public:
    // TODO: take an input file stream containing the permitted nodes for the
    // n-hypercube and populate the hypercube permitted node labels with the
    // set of nodes it lists. Do not change the signature.
    void read(std::istream& input);

    // TODO: take the starting node label string and return a vector of strings
    // containing the node labels on the optimal path from the starting node to
    // the ending node. If not path is found, the returned vector should
    // contain the string "no path". The last string in the returned vector
    // should otherwise be the number of expansions.
    SearchResult search(const std::string& start) const;

  private:
    std::set<std::string> permitted;
    std::set<std::string> all_combos(const std::string base) const;
    // TODO: optionally add any helper methods.
};

#endif
