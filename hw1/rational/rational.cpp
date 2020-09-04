#include "rational.h"

#include <stdexcept>
// Libraries may be included as necessary
#include <algorithm>
#include <cmath>
#include <numeric>
using namespace std;

Rational::Rational() : n(0), d(1) {}

Rational::Rational(int n, int d) {
    // Since the denominator cannot be zero an exception is thrown
    if (d == 0) {
        throw std::invalid_argument("division by zero");
    }

    // TODO: continue your implementation below.
    // You should likely set the member variables ad then call this->reduce()
    // and this->normalize().
    this->n = n;  // Sets private data members
    this->d = d;
    this->normalize();
    this->reduce();
}

void Rational::normalize() {
    // In C++ you don't always have to use this-> to denote a member variable.
    // If you omit it, the compiler will infer that n and d are indeed members
    // of Rational. There are two reasons you will see or use this->:
    //
    // 1. If you have a local variable that has the same name as a member
    //    variable, you need to use this-> to differentiate the two. If you
    //    created `int n` on this next line, the compiler would decide that
    //    any consequent uses of that variable refer to the local n, not our
    //    member variable. To specifically access the member n in this
    //    scenario, we'd use this->n.
    //
    // 2. Style. Some people (like me, Noah, the guy writing this) prefer to
    //    to use this-> to make it clear what variables are members when
    //    reading through the code. Others use a prefix (m_, i.e. m_member) or
    //    a suffix (_, i.e. member_) to denote it. In assignments where you
    //    write your own classes, this will be up to you. For now, do as you
    //    wish, but don't forget readability :)
    //
    // And feel free to delete this huge comment.
    if (n == 0 && d != 1) {
        d = 1;
    }
}

Rational Rational::operator+(const Rational& rhs) const {  // overloaded the + operator to add two of my rational
                                                           // objects
    int d = this->d * rhs.d;
    int n = (this->n * rhs.d) + (this->d * rhs.n);
    Rational temp(n, d);
    temp.reduce();
    return temp;
}

Rational& Rational::operator+=(const Rational& rhs) {  // overloaded the += operator to add two of my rational objects
                                                       // and return the result by reference to the calling object
    int temp = this->d;
    this->d = this->d * rhs.d;
    this->n = (this->n * rhs.d) + (temp * rhs.n);
    this->reduce();
    return *this;
}

Rational Rational::operator*(int rhs) const {  // overloaded the * operator to multiply a rational object by an integer
    int n = this->n * rhs;
    Rational temp(n, d);
    // REDUCE TEMP
    temp.reduce();
    return temp;
}

// This operator isn't prefixed by Rational:: because it's not actually a
// member of Rational. It just tells the compiler what to do when it sees
// multiplication between an int on the left and a Rational on the right. Our
// friend declaration simply enabled us to access private members on the const
// Rational& rhs in the function body.
//
// Note that we could implement any of the other operators above this same way
// using the corresponding global operator and passing a const Rational& lhs as
// the first argument, it's just a matter of convenience.
Rational operator*(int lhs, const Rational& rhs) {
    int n = lhs * rhs.n;
    Rational temp(n, rhs.d);
    // REDUCE TEMP
    temp.reduce();
    return temp;
}

bool Rational::
operator==(const Rational& other) const {  // overloaded the == operator to check for equality between two raitonal
                                           // objects (checks for equality in both numerator and denominator)
    // REDUCE BOTH OTHER AND THIS
    return (this->d == other.d) && (this->n == other.n);
}

// TODO: implement the rest of your prototyped methods
ostream& operator<<(ostream& out, const Rational& r) {  // overloaded the insertion operator to correctly write my
                                                        // rational numbers into an output stream (n/d)
    out << (r.n) << "/" << (r.d);
    return out;
}

istream& operator>>(istream& in, Rational& r) {  // overloaded the extraction operator to correctly read rational
                                                 // numbers from an input stream, making sure to account for the '/'
    char slash;
    in >> (r.n) >> slash >> (r.d);
    r.reduce();
    return in;
}

Rational operator+(const int& rhsint, const Rational& lhsrational) {  // overloaded the + operator to add a left hand
                                                                      // integer and a right hand rational
    Rational temp;
    temp.n = lhsrational.n + rhsint * lhsrational.d;
    temp.d = lhsrational.d;
    temp.reduce();
    return temp;
}

Rational Rational::
operator+(const int& rhs) {  // overloaded the + operator to add a left hand rational and a right had integer
    Rational temp;
    temp.n = this->n + rhs * this->d;
    temp.d = this->d;
    // REDUCE THIS
    temp.reduce();
    return temp;
}

Rational Rational::operator*(const Rational& rhs) {  // overloaded the + function to multiply two rational numbers
    Rational temp;
    temp.n = this->n * rhs.n;
    temp.d = this->d * rhs.d;
    temp.reduce();
    return temp;
}

Rational Rational::operator^(const int& rhs) const {  // overloaded the ^ operator
    Rational temp;
    if (rhs < 0) {  // If the exponent is negative, the numerator and denominator must be flipped
        int rhs1 = -rhs;
        temp.n = pow(d, rhs1);
        temp.d = pow(n, rhs1);
        temp.reduce();
        return temp;
    }
    temp.n = pow(n, rhs);
    temp.d = pow(d, rhs);
    temp.reduce();
    return temp;
}

bool Rational::operator!=(const Rational& other)
        const {  // overloaded the != operator to check for inequality between two rational objects
    return !(this->n == other.n && this->d == other.d);
}

bool Rational::operator<(const Rational& other)
        const {  // overloaded the < operator to check if a left hand rational is less than a right hand rational
    return (this->n * other.d < other.n * this->d);
}

Rational& Rational::
operator*=(const Rational& rhs) {  // overloaded the *= operator to allow me to multiply a left hand rational by a right
                                   // hand rational and return the value to the calling object by reference
    this->n *= rhs.n;
    this->d *= rhs.d;
    this->reduce();
    return *this;
}

Rational& Rational::
operator*=(const int& rhs) {  // overloaded the *= operator to allow me to multiply a left hand rational by a right hand
                              // integer and return the value to the calling object by reference
    this->n *= rhs;
    this->reduce();
    return *this;
}

Rational& Rational::
operator+=(const int& rhs) {  // overloaded the += operator to allow me to add a left hand rational and a right hand
                              // integer and return the value to the calling object by reference
    this->n += rhs * this->d;
    this->reduce();
    return *this;
}

void Rational::reduce() {
    int gcd1 = __gcd(this->n, this->d);  // Finds the GCD of the numerator and the denominator
    this->d /= gcd1;
    this->n /= gcd1;
    if (this->d
        < 0) {  // If the denominator is negative, the signs of both the numerator and denominator must be flipped

        this->d *= -1;
        this->n *= -1;
    }
}
