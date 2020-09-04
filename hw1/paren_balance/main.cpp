#include <iostream>
#include <string>

using namespace std;

// Implement below
bool are_paren_balanced(const string& expression);

// We have suggested a helper method, but feel free to write whatever you see fit
bool are_paren_balanced1(const string& expression, size_t& i, char expected_closing);

// Do not change this method
int main(int argc, char* argv[]) {

    // Check if an expression has been passed to the program
    if (argc != 2) {
        cerr << "error: expected expression to check!" << endl;
        return -1;
    }

    // The second argument is the expression string in the format specified and
    // promised in the assignment page. You do not need to do any error checking
    // except for checking the parentheses.
    string expression(argv[1]);
    if (are_paren_balanced(expression)) {
        cout << "correct" << endl;
    } else {
        cout << "incorrect" << endl;
    }

    return 0;
}

// Some notes:
// - This method must be implemented recursively.
// - When debugging, make sure to output to cerr rather than cout. This will
//   prevent any leftover debug statements from confusing the automated
//   grading and will make sure output is guaranteed to be written to the
//   terminal in case your program crashes.
bool are_paren_balanced(const string& expression) {
    // TODO: your code here
    size_t i = 0;
    return are_paren_balanced1(expression, i, 'p');  // Calls helper function, passing the input string,
                                                     // the starting index, and a dummy expected closing character
}

// Add any additional function implementations here.
//
// If you want to use the suggested helper method, you'll have to write its
// definition here.
bool are_paren_balanced1(const string& expression, size_t& i, char expected_closing) {

    while (expression[i] != '[' && expression[i] != '(' && expression[i] != ']' && expression[i] != ')'
           && i < expression.length()) {
        i++;  // This loop iterates over any character that is not a bracket or parentheses.
    }

    if (i >= expression.length()) {  // Base case. Executes when we have reached the end of our string
        if (expected_closing == ')'
            || expected_closing
                       == ']') {  // This checks the expected closing character of the current stack call.
                                  // If the parentheses and brackets are balanced, the only call left on the stack
                                  // will be the initial call to the helper function, with an expected closing of p
                                  // so if the ending expected closing is not p, then the expression is not balanced
            return false;
        }
        return true;
    }

    else {
        if (expression[i] == expected_closing) {  // If the character matches our expected closing, we increment our
                                                  // iterator and return true
            i++;
            return true;
        } else if (
                (expression[i] == ')' && expected_closing != ')')
                || (expression[i] == ']' && expected_closing != ']')) {
            return false;  // This expression checks if a closed parentheses/bracket does not match the expected closing
        } else if (expression[i] == '(') {
            i++;
            if (are_paren_balanced1(expression, i, ')')) {  // If we reach an open parentheses, we make another call to
                                                            // the helper function with an expected closed parentheses
                return are_paren_balanced1(
                        expression, i, expected_closing);  // If we successfully return true down the call stack, we
                                                           // must call again to check for the next expected closing
            } else {
                return false;  // If our recursive call returns false, we will return false
            }
        } else {  // else if(expression[i]=='['){
            i++;
            if (are_paren_balanced1(expression, i, ']')) {  // If we reach an open bracket, we make another call to the
                                                            // helper function with an expected closed bracket
                return are_paren_balanced1(
                        expression, i, expected_closing);  // If we successfully return true down the call stack, we
                                                           // must call again to check for the next expected closing
            } else {
                return false;  // If our recursive call returns false, we will return false
            }
        }
    }
}