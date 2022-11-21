#include <iostream>
#include <fstream>

#include "Golomb.h"

using namespace std;


int main(int argc, char *argv[]) {

	if(argc != 5) {
		cerr << "Usage: " << argv[0] << " <input txtfile> <output txtfile> <binary file> <m>\n";
		return 1;
	}
	
	char *input_txtFile = argv[1];
	char *output_txtFile = argv[2];
	char *binaryFile = argv[3];
	int mGolomb = stoi(argv[4]);
	

	Golomb g {binaryFile, mGolomb};
	
	ifstream ifs { input_txtFile };
	ofstream ofs { output_txtFile };
	int integer;
	string codeWords;
	
	while(ifs >> integer)
	{
		g.encoder_writeToBinFile(g.encoder(integer));	// encoder
		//cout << g.encoder(integer) << endl;
		//codeWords += g.encoder(integer);
		//g.encoder_writeToBinFile(codeWords);
		ofs << g.decoder() << endl;				// decoder  
	}
	
	return 0;
}
