#include <iostream>
#include <string.h>
#include <vector>
#include <sndfile.hh>
#include <map>

#include "AuxiliaryFunc.h"

#include "Golomb.h"
#include "Predictor.h"

class AudioCodec
{
	private: 
		SndfileHandle audioFile;
		AuxiliarFunc auxF {};
		vector<short> sResidual;	// samples residuals
		int padding = 0;	
		

	public:
	
	AudioCodec(const SndfileHandle& sfh)
	{
		audioFile = sfh;
	}
	
	/** Encode the audio file header in binary **/
	string encode_HeaderOfAudioFile(int typeAudioCodec, int quantization_factor)
	{
		// The audio file header is composed of:
		//	- Lossless/Lossy 	: 1bit ( 0-lossless; 1-lossy)
		//	- number of frames 	: 32 bits
		//	- sample Rate		: 32 bits
		//	- format Type		: 32 bits
		//	- number of Channels	: 2 bits  (max-> 2 channels (10))
		//      - quantization Factor  : 4 bits (0 até 15, sendo que = 0 é lossless por defeito)
		
		string headerEncoded;	// header codificado (em binario)
		string nFrames;
		string sampleRate;
		string formatType;
		string nChannels;
		string quantFactor;
		
		//cout << "quantFactor: " << quantization_factor << endl;
		if (typeAudioCodec == 0)
		{
			headerEncoded += '0';
			quantFactor = "0000";
		}else{ 
			headerEncoded += '1';
			quantFactor = auxF.positiveInt_to_binary(quantization_factor, quantization_factor%2);
			quantFactor = auxF.paddingBits(quantFactor, 4);
			//cout << "quantFactor: " << quantFactor << endl;
		}
		
		//cout << "nFrames_original: " << audioFile.frames() << endl;
		nFrames = auxF.positiveInt_to_binary(audioFile.frames(), audioFile.frames()%2);
		nFrames = auxF.paddingBits(nFrames,32);
		//cout << "nFrames_binaryNaoOrdem: " << nFrames << endl;
		
		//cout << "sample_rate_org: " << audioFile.samplerate() << endl;
		sampleRate = auxF.positiveInt_to_binary(audioFile.samplerate(), audioFile.samplerate()%2);
		sampleRate = auxF.paddingBits(sampleRate,32);
		//cout << "sampleRate: " << sampleRate << endl;
		
		//cout << "format: " << audioFile.format() << endl;
		formatType = auxF.positiveInt_to_binary(audioFile.format(), audioFile.format()%2);
		formatType = auxF.paddingBits(formatType,32);
		//cout << "formatType: " << formatType << endl;
		
		//cout << "nChannels_original: " << audioFile.channels() << endl;
		nChannels = auxF.positiveInt_to_binary(audioFile.channels(), audioFile.channels()%2);
		nChannels = auxF.paddingBits(nChannels, 2);
		//cout << "nChannels: " << nChannels << endl;
		
		headerEncoded += (nFrames + sampleRate + formatType + nChannels + quantFactor);
		//cout << "headerEncoded: " << headerEncoded << endl;
		
		return headerEncoded;
	}
	
	/**
	* fname - nome ficheiro binario com os dados comprimidos
	* origSamples - amostras originais do ficheiro de audio 
	* typeAudioCodec - 0:Lossless; 1:Lossy
	* predictorType - 0,1,2,3
	*/
	void compress(char *fname, const vector<short> &origSamples, int typeAudioCodec, int predType, int quantization_factor)
	{
		Predictor pred { predType };
		Golomb g {fname, 1}; // m = 1: initialization only
		
		int optimalM;
		string encOptimalM;
		
		string compressHeader;
		string payload;
		string compressedValues;	// header + data compressed
		
		/******* Applying sample prediction *****/
		if (typeAudioCodec == 0)
			// Lossless audio codec
			sResidual = pred.predLossless_separateChannels(origSamples); // --- ORIGINAL
			//sResidual = pred.predLossless_joinChannels(origSamples);	// --- TESTE
			//sResidual = pred.predLossless_MidSideChannels(origSamples);	// -- TESTE
		else
			// Lossy audio
			sResidual = pred.predLossy_separateChannels(origSamples, quantization_factor); // --- ORIGINAL
			//sResidual = pred.predLossy_joinChannels(origSamples, quantization_factor); // --- TESTE
			//sResidual = pred.predLossy_MidSideChannels(origSamples, quantization_factor); // -- TESTE
		
		
		//createHist(sResidual);
		
		/**** Determinar a entropia ***/
		//cout << "Original Entropy: " << calcEntropy(origSamples) << " Residual Entropy: " << calcEntropy(sResidual) << endl;
		
		cout << "Compressing..." << endl;
		
		// Obtain the optimal value of m
		optimalM = g.getOptimalm(sResidual);
		g.set_m(optimalM);
		//cout << "mOptimal: " << optimalM << endl; 
		encOptimalM = auxF.positiveInt_to_binary(optimalM, optimalM%2); // Encode m Golomb to binary
		//cout << "binary_mOptimal: " << encOptimalM << endl;
		encOptimalM = auxF.paddingBits(encOptimalM,32);		  // 32-bit binary
		
		// compress audio file header = encoded Optimal m + encoded audio file header (131bits)
		compressHeader = encOptimalM + encode_HeaderOfAudioFile(typeAudioCodec, quantization_factor);
		
		// Encode the residual values with the Golomb code
		for (size_t i = 0; i < sResidual.size(); i++)
		{
			payload += g.encoder(sResidual[i]);
		}
		
		compressedValues = compressHeader + payload;
		padding = g.encoder_writeToBinFile(compressedValues);
	}
	
	
	/** Decode the audio file header **/
	vector<int> decode_HeaderOfAudioFile(vector<int> audioHeader)
	{
		vector<int> infoAudioHeader; 
	
		// Lossless/Lossy --> infoAudioHeader[0]
		infoAudioHeader.push_back(audioHeader[0]); 
		//cout << "typeAudioCodec: " << infoAudioHeader[0] << endl;
		
		// number of frames
		vector<int> nFrames;
		nFrames.insert(nFrames.begin(), audioHeader.begin()+1, audioHeader.begin()+33);
		infoAudioHeader.push_back(auxF.binary_to_integer(nFrames, 32));
		//cout << "nFrames: " << infoAudioHeader[1] << endl;
		
		
		// sample rate
		vector<int> sampleRate;
		sampleRate.insert(sampleRate.begin(), audioHeader.begin()+33, audioHeader.begin()+65);
		infoAudioHeader.push_back(auxF.binary_to_integer(sampleRate, 32));
		//cout << "sampleRate: " << infoAudioHeader[2] << endl;
		
		
		// format type
		vector<int> formatType;
		formatType.insert(formatType.begin(), audioHeader.begin()+65, audioHeader.begin()+97);
		infoAudioHeader.push_back(auxF.binary_to_integer(formatType, 32));
		//cout << "formatType: " << infoAudioHeader[3] << endl;
		
		// number of channels
		vector<int> nChannels;
		nChannels.insert(nChannels.begin(), audioHeader.begin()+97, audioHeader.begin()+99);
		infoAudioHeader.push_back(auxF.binary_to_integer(nChannels, 2));
		//cout << "nChannels: " << infoAudioHeader[4] << endl;
		
		// quantization factor
		vector<int> quantFactor;
		quantFactor.insert(quantFactor.begin(), audioHeader.begin()+99, audioHeader.begin()+103);
		infoAudioHeader.push_back(auxF.binary_to_integer(quantFactor, 4));
		//cout << "quantFactor: " << infoAudioHeader[5] << endl;
		 
		
		return infoAudioHeader;
	}	
	
	void decompress(char *compressFile, char *wavOutputFile, int predType)
	{
		BitStream bsR { compressFile, 'r'};
		Golomb g {compressFile, 1}; 
		Predictor pred { predType };
		
		//cout << "\n\n----------------- DECONDING -------------------------\n" << endl;
		
		
		int totalBits = (bsR.binaryFile_size()*8) - padding;
		vector<int> totalData = bsR.read_Nbits(totalBits);	// header + payload
		//cout << "totalData_size: " << totalData.size() << endl;
		
		// Read compressed Header -- 135 bits
		vector<int> compressHeader;	
		compressHeader.insert(compressHeader.begin(), totalData.begin(), totalData.begin()+135);
		
		// Decoding Golomb's m-value 
		int mGolomb;
		vector<int> m;
		m.insert(m.begin(), compressHeader.begin(), compressHeader.begin()+32);
		mGolomb = auxF.binary_to_integer(m, 32);
		g.set_m(mGolomb);
		//cout << "mGolomb: " << mGolomb << endl;
		
		
		// Decoding header of the audio file	-- 103 bits
		vector<int> infoAudioCompress;
		infoAudioCompress.insert(infoAudioCompress.begin(), compressHeader.begin()+32, compressHeader.begin()+135);
		vector<int> infoAudioFile = decode_HeaderOfAudioFile(infoAudioCompress);
		
		// Decoding residual samples
		vector<int> compressPayload;
		compressPayload.insert(compressPayload.begin(), totalData.begin()+135, totalData.begin()+totalBits);
		
		
		int quantFactor = infoAudioFile[5];
		vector<short> payload;
		if (infoAudioFile[0] == 0) // Lossless
		{
			payload = g.decoder(compressPayload); // Decoding residual samples
			//cout << "payload.size():" << payload.size() << endl;
		
		}else{	// Lossy
			vector<short> payloadWithError = g.decoder(compressPayload);	
			
			// atualizar o valor da amostra residual/sincronismo do erro
			for (size_t i = 0; i < payloadWithError.size(); i++)
			{
				payload.push_back(payloadWithError[i] << quantFactor);
			}
		}
		
		
		vector<short> orignalSamples_Xn;
		orignalSamples_Xn = pred.reversePred_separateChannels(payload); 	// -- ORIGINAL
		//orignalSamples_Xn = pred.reversePred_joinChannels(payload); 		// -- TESTE 
		//orignalSamples_Xn = pred.reversePred_MidSideChannels(payload); 	// -- TESTE
		
		// BUILD the wav audio file 
		SF_INFO sfinfoOut ;
		sfinfoOut.frames = infoAudioFile[1];
		sfinfoOut.samplerate = infoAudioFile[2];
		sfinfoOut.format = infoAudioFile[3];
		sfinfoOut.channels = infoAudioFile[4];
		
		SNDFILE * outfile = sf_open(wavOutputFile, SFM_WRITE, &sfinfoOut);
		sf_count_t count = sf_write_short(outfile, &orignalSamples_Xn[0], orignalSamples_Xn.size()); 
		sf_write_sync(outfile);
		sf_close(outfile);
	}
	
	/*** Determine the entropy of the originals and residual samples ***/
	double calcEntropy(const vector<short> &samples)
	{
		double entropy = 0;
		double prob = 0;
		map<short,size_t> res;
		
		for(size_t i = 0; i < samples.size(); i++) {
			res[samples[i]]++;
		}

		for (auto n : res)
		{
			prob = (double) n.second / (double)samples.size();
        		entropy += prob * (-log2(prob));
		}
		return entropy;
	}
	
	void createHist(const vector<short> &samples)
	{
		std::map<short,size_t> hist;
		
		for(size_t i = 0; i < samples.size(); i++)
		{
			hist[samples[i]]++;
		}
		
		for (auto [value, counter] : hist)
		{
			cout << value << '\t' << counter << '\n';
		}
	}
};







