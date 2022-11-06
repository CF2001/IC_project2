#include <iostream>

#include "Golomb.h"

using namespace std;


int main() {

	// Just for test
	Golomb g { };
	
	cout << g.decoder(g.encoder(-17831)) << endl;
	
	
	
	return 0;
}
