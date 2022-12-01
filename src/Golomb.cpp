#include <iostream>
#include <fstream>
#include <string>

#include "Golomb.h"

using namespace std;


int main(int argc, char *argv[]) {

	if(argc != 5) {
		cerr << "Usage: " << argv[0] << " <inFileTxt> <outFileTxt> <binary File> <m>\n"; 
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
	
	/***** Encoding ******/
	
	while(ifs >> integer)
	{
		codeWords += g.encoder(integer);	// encode the integer values 
	}
	
	int padding = g.encoder_writeToBinFile(codeWords);	// write the encoded values to the binary file
	
	
	/***** Decoding ******/
	
	BitStream bsR { binaryFile , 'r'}; 
	
	vector<int> g_codeWords = bsR.read_Nbits((bsR.binaryFile_size()*8)-padding);	// Read all encoded values from the file 
	vector<short> originalWords = g.decoder(g_codeWords);	// Decoding the values

	for (size_t i = 0; i < originalWords.size(); i++)
	{
		ofs << originalWords[i] << endl;	
	}	
	return 0;
}
