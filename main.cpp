#include "test/addition.hpp"
#include "test/constructor.hpp"
#include "test/subtraction.hpp"
#include "test/multiplication.hpp"
#include "test/division.hpp"

int main() {
     testConstructor();
     testAddition();
     testSubtraction();
     testMultiplication();
     testDivision();

    return 0;
}
