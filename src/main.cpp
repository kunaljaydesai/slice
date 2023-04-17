#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "codegen.h"
#include "parser.h"
#include "scanner.h"

std::string readFile(std::string filepath) {
  std::ifstream f(filepath);
  std::stringstream ss;
  ss << f.rdbuf();
  return ss.str();
}

int main(int argc, char **argv) {
  if (argc < 2) {
    throw std::runtime_error("File to parse required");
  }
  std::string filepath = argv[1];
  std::string source = readFile(filepath);

  Scanner scanner(source);
  scanner.scanTokens();

  Parser parser(scanner.tokens());
  std::unique_ptr<Program> program = parser.parse();

  CodegenVisitor visitor;
  visitor.visitProgramNode(program.get());
  visitor.optimize();
  visitor.dump();

  return 0;
}