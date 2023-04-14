#include <iostream>
extern "C" {
double fib(double);
}

int main() {
  std::cout << "FIB " << fib(10.0) << std::endl;
  return 0;
}