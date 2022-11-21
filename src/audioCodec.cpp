#include <iostream>
#include "Golomb.h"
#include "Predictor.h"

constexpr size_t FRAMES_BUFFER_SIZE = 65536;
int main (int argc, char** argv)
{
	
	if(argc != 2) {
		cerr << "Usage: " << argv[0] << " <wav file> \n";
		return 1;
	}
	
	char *inputFile = argv[1];

	SndfileHandle sndFile { inputFile };
	if(sndFile.error()) {
		cerr << "Error: invalid input file\n";
		return 1;
    	}

	if((sndFile.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: file is not in WAV format\n";
		return 1;
	}

	if((sndFile.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
		cerr << "Error: file is not in PCM_16 format\n";
		return 1;
	}
	
	size_t nFrames;
	/* Amostras do fichiro original */
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels()); 
	
	Predictor pred { 2 };
	
	vector<short> xnS;
	
	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE)))
	{
		samples.resize(nFrames * sndFile.channels());
		//pred.printSamples(samples);
		pred.predictorLossless_allChannels(samples);
		pred.reversePredLosseless_allChannels(samples);
	}	
	
	/*
	for (size_t i = 0; i < xnS.size(); i++)
	{
		cout << xnS[i] << endl;
	}*/
	
	//pred.printPredictor();
	//pred.printSamples();

	
	
}
