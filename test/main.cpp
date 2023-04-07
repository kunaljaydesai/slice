#include "../src/scanner.h"
#include <assert.h>
#include <iostream>
#include <string>

void runScannerTests() {
  // test 1
  std::string basic = "# Compute the x'th fibonacci number.\n"
                      "def fib(x){\n"
                      "if x < 3 {\n"
                      "1\n"
                      "} else {\n"
                      "fib(x-1)+fib(x-2)\n"
                      "}\n"
                      "}\n"
                      "# This expression will compute the 40th number.\n"
                      "fib(40)";
  Scanner scanner(basic);
  scanner.scanTokens();
  const std::vector<Token> expected = {
      Token(TokenType::tok_def),
      Token(TokenType::tok_identifier, "fib"),
      Token(TokenType::tok_lpar),
      Token(TokenType::tok_identifier, "x"),
      Token(TokenType::tok_rpar),
      Token(TokenType::tok_lbrak),
      Token(TokenType::tok_if),
      Token(TokenType::tok_identifier, "x"),
      Token(TokenType::tok_lt),
      Token(TokenType::tok_number, 3),
      Token(TokenType::tok_lbrak),
      Token(TokenType::tok_number, 1),
      Token(TokenType::tok_rbrak),
      Token(TokenType::tok_else),
      Token(TokenType::tok_lbrak),
      Token(TokenType::tok_identifier, "fib"),
      Token(TokenType::tok_lpar),
      Token(TokenType::tok_identifier, "x"),
      Token(TokenType::tok_sub),
      Token(TokenType::tok_number, 1),
      Token(TokenType::tok_rpar),
      Token(TokenType::tok_add),
      Token(TokenType::tok_identifier, "fib"),
      Token(TokenType::tok_lpar),
      Token(TokenType::tok_identifier, "x"),
      Token(TokenType::tok_sub),
      Token(TokenType::tok_number, 2),
      Token(TokenType::tok_rpar),
      Token(TokenType::tok_rbrak),
      Token(TokenType::tok_rbrak),
      Token(TokenType::tok_identifier, "fib"),
      Token(TokenType::tok_lpar),
      Token(TokenType::tok_number, 40),
      Token(TokenType::tok_rpar),
  };
  std::vector<Token> actual = scanner.tokens();
  for (size_t i = 0; i < expected.size(); i++) {
    Token actual_token = actual[i];
    Token expected_token = expected[i];
    if (actual_token != expected_token) {
      std::cout << "Expected " << expected[i].toString() << ", got "
                << scanner.tokens()[i].toString() << std::endl;
      throw std::runtime_error("Test failed");
    }
  }
}
int main(int argc, char **argv) {
  runScannerTests();
  std::cout << "Tests succeeded!" << std::endl;
  return 0;
}