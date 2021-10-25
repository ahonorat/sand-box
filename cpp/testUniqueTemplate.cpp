#include <iostream>

template<int id, int static_value_max>
bool countToStatic() {
  static int value = 0;
  //  std::cerr << "id "<< id << " : "<< value << std::endl;
  std::cerr << value << std::endl;
  if (value < static_value_max) {
    value++;
  } else {
    return true;
  }
  // did not reach maximum yet, should continue
  return false;
}

int main(int argc, char** argv) {
  bool shouldContinue = true;
  while (shouldContinue) {
    shouldContinue = false;
    shouldContinue |= !countToStatic<0,5>();
    shouldContinue |= !countToStatic<0,5>();
    //    shouldContinue |= !countToStatic<1,5>();
  }
}
