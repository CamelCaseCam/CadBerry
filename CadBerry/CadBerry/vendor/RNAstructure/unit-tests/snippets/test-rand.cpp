#include "../src/random.cpp"
#include <iostream>
#include <vector>

<<<<<<< HEAD
using namespace std;
=======
//
using namespace std;
//
>>>>>>> 361492b0f6e9a29bb88098eeab4d8ec72d2d1807

int main() {
	rand64 r(234);
	RandomOutput::Int<rand64> n(r);

	vector<string> s;
	s.push_back("hello world");
	s.push_back("nice day!");
	s.push_back("popcorn!!!");

	for(int i = 0; i < 100; i++) {
		cout << r(s)  << endl;
	}
	for(int i = 0; i < 100; i++) {
		cout << n()  << endl;
	}
	
	return 0;
}