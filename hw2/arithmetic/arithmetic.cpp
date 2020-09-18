#include "token.h"
#include "token_stack.h"
#include <cctype>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

// THROW ALL EXCEPTIONS EXACTLY AS GIVEN IN THE COMMENTS
// You need only uncomment them to throw the exceptions
// under the correct conditons
// You can check the conditions using an if statement or
// switch statement
// For any problems with the expression format as well as attempting
// division by zero, throw the exception invalid_argument("Malformed");

TokenStack tokenize(std::string expression) {
    TokenStack stack;

    stringstream nowhitespace(expression);  // initially stores expression with whitespace in a stringstream
    string temp;
    string s = "";
    while (nowhitespace >> temp) {  // parses out indivual strings (no whitespace)
        s += temp;                  // appends parced pieces to a single string
    }
    stringstream stream(s);  // creates a stringstream consisting of expression with no whitespace

    int num;           // used to write into an int
    char character;    // used to write into a char
    int lp_count = 0;  // counts the number of left parentheses
    int rp_count = 0;  // counts the number of right parentheses
                       // Loop while there are still characters in the stream.
    while (stream.rdbuf()->in_avail()) {
        // TODO: push your tokens to the stack.
        if (stream.peek() >= 48
            && stream.peek() <= 57) {  // checks if the character at the front of the stringstream is an integer
            stream >> num;             // reads in the number
            stack.push(Token(TokenKind::NUMBER, num));  // pushes number onto the TokenStack
        } else {
            stream >> character;  // reads non-integer character into variable "character"
            if (character == '(') {
                stack.push(Token(TokenKind::LP));  // pushes a left parentheses onto the stack
                lp_count++;                        // increments number of left parentheses
            } else if (character == ')') {
                stack.push(Token(TokenKind::RP));  // pushes a right parentheses onto the stack
                rp_count++;                        // increments number of right parentheses
            } else if (character == '+') {
                stack.push(Token(TokenKind::PLUS));  // pushes a plus operator onto the stack
            } else if (character == '-') {
                stack.push(Token(TokenKind::MINUS));  // pushes a minus operator onto the stack
            } else if (character == '*') {
                stack.push(Token(TokenKind::TIMES));  // pushes a times operator onto the stack
            } else if (character == '/') {
                stack.push(Token(TokenKind::DIVIDE));  // pushes a division operator onto the stack
            } else if (character == ' ') {
                continue;
            } else {                                    // if character does not match any of the valid types
                stack.push(Token(TokenKind::INVALID));  // pushes invalid token on stack
                throw invalid_argument("Malformed");    // throws malformed error
            }
        }
        // If we find an invalid token, we can simply:
        // throw invalid_argument("Malformed");
    }

    // We pushed the tokens into the stack from left to right, which means
    // we'll pop them back out backwards. For convenience, we'll return the
    // reverse of our current stack so that we can pop the tokens out from
    // left to right, as you'd read the expression.
    if (lp_count
        != rp_count) {  // checks to make sure that the number of right parentheses and left parentheses are the same
        throw invalid_argument("Malformed");
    }
    stack.reverse();  // reverses the stack
    return stack;
}

// Reduce is called when we encounter closing parentheses. If the expression
// is validly formed, we can expect there to be either a binary operation on
// the work stack. This method should pop the tokens in that operation,
// compute the result, and push it back onto the work stack as a new token.
void reduce(TokenStack& work) {
    TokenStack immediate;

    // TODO: pop tokens off of work into immediate until we hit TokenKind::LP.
    // TODO: if there's no TokenKind::LP, throw:
    // throw invalid_argument("Malformed");
    while (work.top().kind
           != TokenKind::LP) {  // iterates through work stack until it gets to a left parentheses or the stack is empty
        immediate.push(work.top());  // pushes top token onto immediate
        work.pop();                  // pops token off of work
    }
    if (work.size() == 0) {  // if the work stack ends empty, then there was no left parentheses in the expression, and
                             // it should throw an error
        throw invalid_argument("Malformed");
    }
    work.pop();  // pops the left parentheses

    // TODO: check whether we have "-p" or "p @ q @ ..." and handle accordingly
    // TODO: if you are expecting a token but immediate is empty, throw:
    // throw invalid_argument("Malformed");
    // TODO: if you are expecting a token of a different kind, throw:
    // throw invalid_argument("Malformed");
    // TODO: if you encounter a differing chained binary operation, throw:
    // throw invalid_argument("Malformed");

    if (immediate.size() == 2) {  // if the size is 2, it is either a negative expression, or a malformed expression

        Token negcheck = immediate.top();
        immediate.pop();
        Token negsign = immediate.top();
        if (negcheck.kind == TokenKind::MINUS) {  // checks if the expression is negative
            negsign.value *= -1;
            work.push(negsign);  // pushes the new, negative token back onto the work stack
            return;
        } else {  // if the 2 item stack is not a negative number, it is malformed
            throw invalid_argument("Malformed");
        }
    }

    Token chainOperatorCheck(TokenKind::INVALID);  // Token to ensure that the chained operations are all the same
    int chainCount = 0;                            // used to implement chained checking after first iteration
    while (immediate.size()
           >= 3) {  // only enters if there are 3 items on the immediate stack (2 numbers and an operation)

        if (immediate.top().kind != TokenKind::NUMBER) {  // if the top item is not a number, the expression is
                                                          // malformed
            throw invalid_argument("Malformed");
        }
        Token num1 = immediate.top();  // reads in the top number
        immediate.pop();

        Token op = immediate.top();
        if (chainCount == 0) {  // in first iteration, saves the operation used
            chainOperatorCheck.kind = op.kind;
        } else {  // in subsequent iterations, checks to see if the current operation matches the first one
            if (chainOperatorCheck.kind != op.kind) {
                throw invalid_argument("Malformed");
            }
        }
        immediate.pop();

        if (immediate.top().kind != TokenKind::NUMBER) {  // if top item is not a number, the epression is malformed
            throw invalid_argument("Malformed");
        }
        Token num2 = immediate.top();  // reads in the second number
        immediate.pop();

        if (op.kind == TokenKind::PLUS) {
            num1.value += num2.value;  // if the operation is a plus, add the two numbers
        } else if (op.kind == TokenKind::MINUS) {
            num1.value -= num2.value;  // if the operation is a minus, subtract the two numbers
        } else if (op.kind == TokenKind::TIMES) {
            num1.value *= num2.value;  // if the operation is a multiplication, multiply the two numbers
        } else if (op.kind == TokenKind::DIVIDE) {
            if (num2.value == 0) {  // if the divisor is 0, return malformed
                throw invalid_argument("Malformed");
            }
            num1.value /= num2.value;  // if the operation is division, divide the two numbers
        } else {
            throw invalid_argument("Malformed");
        }
        chainCount++;          // increments chainCount;
        immediate.push(num1);  // pushes new operated number onto the stack
    }
    if (immediate.size() != 1) {  // if the expression was not reduced to a single number, return malformed
        throw invalid_argument("Malformed");
    }

    work.push(immediate.top());  // pushes the resolved expression back onto the work Stack

    // TODO: use integer math to do your computations.
    // TODO: if the operator is not of kind +, -, *, or /, throw:
    // throw invalid_argument("Malformed");
    // TODO: if dividing by zero, throw:
    // throw invalid_argument("Malformed");

    // TODO: push the result of either branch back onto work
}

int evaluate(std::string expression) {
    TokenStack tokens = tokenize(expression);
    TokenStack work;

    // TODO: while the stack is not empty, pop the top token.
    // TODO: if the top token is of TokenKind::RP, reduce(work).
    // TODO: otherwise, push the top token onto work.
    while (!tokens.empty()) {                      // runs until all tokens have been popped off
        if (tokens.top().kind == TokenKind::RP) {  // checks for right parentheses
            tokens.pop();                          // pop off the right parentheses
            reduce(work);
        } else {
            work.push(tokens.top());  // push item onto work stack
            tokens.pop();
        }
    }

    // TODO: once done, check there is exactly one TokenKind::NUMBER token.
    // TODO: if so, return its value, otherwise throw:
    // throw invalid_argument("Malformed");

    if (work.size() == 1
        && work.top().kind == TokenKind::NUMBER) {  // checks that the work stack only has one, number token
        return work.top().value;
    } else {
        throw invalid_argument("Malformed");  // if not, return malformed
    }
}

// Please DO NOT CHANGE MAIN
int main(int argc, char** argv) {
    if (argc <= 1) {
        cerr << "Expect an expression..." << endl;
        return -1;
    }

    try {
        cout << evaluate(argv[1]) << endl;
    } catch (invalid_argument& e) {
        cerr << "error: " << e.what() << endl;
    }
    return 0;
}

// If you're interested in extending your parser to support things like order
// of operations, unary operators, and single values, feel free to ping me
// on Discord (noahbkim) once you've finished the assignment as is. This will
// not give you any extra credit or improve your score in any way; it's just
// something fun and cool.
