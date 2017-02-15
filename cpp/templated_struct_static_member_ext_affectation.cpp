#include <iostream>
using namespace std;

template<typename T>
struct A {
  static int id;
};

// static templated struct parameter
// must be declared globally once
template<typename T>
int A<T>::id = 2;

void init() {
  A<int>::id = 3;
}

int main(int argc, char** argv) {
  init();
  cout << A<int>::id << endl;
  return 0;
}
