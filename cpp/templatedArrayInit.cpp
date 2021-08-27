#include <iostream>
using namespace std;

template<int ... args>
void functionArray() {
        static int data[sizeof...(args)] = {int(args)...};
	static int indiceL = 0;
	// for (int i = 0; i < sizeof...(args); i++) {
	//   cout << data[i] << endl;
	// }
	cout << data[indiceL] << endl;
	indiceL = (indiceL + 1) % (sizeof...(args));
	// getting the n-th argument seems harder
};



int main(int argc, char** argv) {
  functionArray<3,10,9>();
  functionArray<3,10,9>();
  functionArray<3,10,9>();
  functionArray<3,10,9>();
  functionArray<3,10,7>();
  functionArray<3,10,7>();
  functionArray<3,10,7>();
  functionArray<3,10,7>();
  return 0;
}
