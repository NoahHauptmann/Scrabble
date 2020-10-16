#include "hypercube.h"

using namespace std;
#include <map>

void HyperCube::read(istream& input) {
    // TODO: implement
    int num_nodes;
    input >> num_nodes;  // reads in the number of nodes
    for (int i = 0; i < num_nodes; i++) {
        string curr_node;
        input >> curr_node;
        permitted.insert(curr_node);  // pushes each string into the set permitted
    }
}

SearchResult HyperCube::search(const string& start) const {
    // TODO: implement

    // Initialize ending node of all 1's.
    std::string end(start.size(), '1');

    // Instantiate priority queue to use for A* algorithm.
    std::priority_queue<Node, std::vector<Node>, NodePriority> unvisited;

    // Initialize the number of expansions used by the algorithm and path.
    size_t expansions = 0;
    std::vector<std::string> path;

    // Add any variables needed for A* algorithm
    std::map<string, string> closed;  // map to track visited nodes and their predecessors
    // Implement A* algorithm. Remember to add node label strings on the path
    // to the results vector. Order of the strings in the vector matters. Make
    // sure to return a SearchResult object.
    Node starting_node = Node(start, "start", 0);  // creates starting node w g-value of 0
    unvisited.push(starting_node);                 // pushes starting node into heap
    while (!unvisited.empty()) {                   // iterates until heap is empty
        Node curr = unvisited.top();
        if (closed.find(curr.word) != closed.end()) {  // skips over any nodes that have been visited
            unvisited.pop();
            continue;
        }
        closed[curr.word] = curr.pred;  // adds current node to the closed map
        unvisited.pop();
        expansions++;            // increments expansions
        if (curr.word == end) {  // checks to see if you have reached the end node
            expansions--;        // decrements expansions as end node is not counted
            string current_node = curr.word;
            std::vector<std::string> reverse_path;
            while (current_node != "start") {          // backtraces until the start node is reached
                reverse_path.push_back(current_node);  // pushes nodes into reverse path
                current_node = closed[current_node];
            }
            for (int i = reverse_path.size() - 1; i >= 0; i--) {  // reverses path vector
                path.push_back(reverse_path.back());
                reverse_path.pop_back();
            }
            return SearchResult(path, expansions);  // returns successful search result
        }
        string current_word = curr.word;
        set<string> digit_flips = all_combos(current_word);  // returns a set of all possible neighbor nodes
        set<string>::iterator it;
        for (it = digit_flips.begin(); it != digit_flips.end(); it++) {  // iterates over set of neighbor nodes
            if (permitted.find(*it) == permitted.end()) {  // if the neighbor is not in the valid set, skip it
                continue;
            }
            Node new_expansion = Node(*it, curr.word, curr.g + 1);  // creates a new node, incrementing g
            unvisited.push(new_expansion);
        }
    }
    return SearchResult(false, expansions);
}

set<string> HyperCube::all_combos(const string base) const {
    set<string> combinations;
    for (int i = 0; i < base.size(); i++) {  // iterates over the string
        string digit_flip = base;
        if (base[i] == '1') {
            digit_flip[i] = '0';  // flips a 1 to a 0
        } else {
            digit_flip[i] = '1';  // flips a 0 to a 1
        }
        combinations.insert(digit_flip);
    }
    return combinations;
}
