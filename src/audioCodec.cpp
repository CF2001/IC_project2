#include <iostream>
#include <sndfile.hh>
#include <vector>

#include "AudioCodec.h"


constexpr size_t FRAMES_BUFFER_SIZE = 65536;
int main (int argc, char** argv)
{
	
	if(argc != 6) {
		cerr << "Usage: " << argv[0] << " <wavfileIn> <wavfileOut> <binary file> <typeAudioCodec> <nPredictor>\n";
		return 1;
	}
	
	char *inputFileWav = argv[1];
	char *outputFileWav = argv[2];
	char *binaryFile = argv[3];
	int typeAudioCodec = stoi(argv[4]);
	int nPredictor = stoi(argv[5]);

	SndfileHandle sndFile { inputFileWav };
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
	
	if (typeAudioCodec < 0 || typeAudioCodec > 1)
	{
		cerr << "Error: Only lossless or lossy audio exists. Lossless=0 or Lossy=1.\n";
		return 1;
	}
	
	if (nPredictor <= 0 || nPredictor > 3)
	{
		cerr << "Error: Wrong choice of predictor! There is only predictor 1,2 or 3.\n";
		return 1;
	}
	
	/*
	int quantizationFactor; 
	if (typeAudioCodec == 1)
	{
		cout << "Enter the quantization factor: " << endl;
		cin >> quantizationFactor;
	}else{
		quantizationFactor = 1;	// initialization
	}
	
	if (quantizationFactor < 0 || quantizationFactor > 15)
	{
		cerr << "Error: Wrong quantization factor.\n\n";
		return 1;
	}*/
	
	size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels()); // Amostras do fichiro original
	vector<short> origSamples; // copia das amostras do fichiro original
	
	AudioCodec audioC {sndFile};
	
	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE)))
	{
		samples.resize(nFrames * sndFile.channels());
		for (size_t i = 0; i < samples.size(); i++)
		{
			origSamples.push_back(samples[i]);
		}
	}	
	
	/*
	for (size_t i = 0; i < origSamples.size(); i++)
	{
		cout << origSamples[i] << endl;
	}
	*/
	
	audioC.compress(binaryFile, origSamples, typeAudioCodec, nPredictor, 2);
	audioC.decompress(binaryFile, outputFileWav, nPredictor);
}


