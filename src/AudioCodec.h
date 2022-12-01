#include <iostream>
#include <string.h>
#include <vector>
#include <sndfile.hh>

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
	string encode_HeaderOfAudioFile(int typeAudioCodec)
	{
		// The audio file header is composed of:
		//	- Lossless/Lossy 	: 1bit ( 0-lossless; 1-lossy)
		//	- number of frames 	: 32 bits
		//	- sample Rate		: 32 bits
		//	- format Type		: 32 bits
		//	- number of Channels	: 2 bits  (max-> 2 channels (10))
		
		string headerEncoded;	// header codificado (em binario)
		string nFrames;
		string sampleRate;
		string formatType;
		string nChannels;
		
		if (typeAudioCodec == 0)
			headerEncoded += '0';
		else 
			headerEncoded += '1';

		cout << "nFrames_original: " << audioFile.frames() << endl;
		nFrames = auxF.positiveInt_to_binary(audioFile.frames(), audioFile.frames()%2);
		nFrames = auxF.padding_32bitBinary(nFrames);
		//cout << "nFrames_binaryNaoOrdem: " << nFrames << endl;
		
		cout << "sample_rate_org: " << audioFile.samplerate() << endl;
		sampleRate = auxF.positiveInt_to_binary(audioFile.samplerate(), audioFile.samplerate()%2);
		sampleRate = auxF.padding_32bitBinary(sampleRate);
		//cout << "sampleRate: " << sampleRate << endl;
		
		cout << "format: " << audioFile.format() << endl;
		formatType = auxF.positiveInt_to_binary(audioFile.format(), audioFile.format()%2);
		formatType = auxF.padding_32bitBinary(formatType);
		//cout << "formatType: " << formatType << endl;
		
		cout << "nChannels_original: " << audioFile.channels() << endl;
		nChannels = auxF.positiveInt_to_binary(audioFile.channels(), audioFile.channels()%2);
		//cout << "nChannels: " << nChannels << endl;
		
		headerEncoded += (nFrames + sampleRate + formatType + nChannels);
		//cout << headerEncoded << endl;
		
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
		
		string compressQuantFactor;
		
		string compressHeader;
		string payload;
		string compressedValues;	// header + data compressed
		
		if (typeAudioCodec == 0)
			// Lossless audio codec
			sResidual = pred.predLossless_separateChannels(origSamples);
		else
			// Lossy audio
			sResidual = pred.predLossy_separateChannels(origSamples, quantization_factor);
		
		// Obtain the optimal value of m
		optimalM = g.getOptimalm(sResidual);
		g.set_m(optimalM);
		cout << "mOptimal: " << optimalM << endl; 
		encOptimalM = auxF.positiveInt_to_binary(optimalM, optimalM%2); // optimalM to binary
		//cout << "binary_naoOrdem: " << encOptimalM << endl; 
		encOptimalM = auxF.padding_32bitBinary(encOptimalM);	// 32-bit binary
		
		// compress audio file header = encoded Optimal m + encoded audio file header (131bits)
		compressHeader = encOptimalM + encode_HeaderOfAudioFile(typeAudioCodec);
		
		if (typeAudioCodec == 1)
		{
			cout << "Fator de quantização: " << quantization_factor << endl;
			//cout << "Lossy: determinar fator de quantizacao" << endl;
			compressQuantFactor = auxF.positiveInt_to_binary(quantization_factor, quantization_factor%2);
			
			while(compressQuantFactor.size() != 4)
			{
				compressQuantFactor += '0';	// padding para ter 4 bits de representação
			}
			compressHeader += compressQuantFactor;	// add quantization factor compressed
		}
			
		
		cout << "sResidual.size():" << sResidual.size() << endl;
		// Encode the residual values with the Golomb code
		for (size_t i = 0; i < sResidual.size(); i++)
		{
			payload += g.encoder(sResidual[i]);
			//cout << sResidual[i] << " : " << g.encoder(sResidual[i]) << endl;
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
		cout << "nFrames: " << infoAudioHeader[1] << endl;
		
		
		// sample rate
		vector<int> sampleRate;
		sampleRate.insert(sampleRate.begin(), audioHeader.begin()+33, audioHeader.begin()+65);
		infoAudioHeader.push_back(auxF.binary_to_integer(sampleRate, 32));
		cout << "sampleRate: " << infoAudioHeader[2] << endl;
		
		
		// format type
		vector<int> formatType;
		formatType.insert(formatType.begin(), audioHeader.begin()+65, audioHeader.begin()+97);
		infoAudioHeader.push_back(auxF.binary_to_integer(formatType, 32));
		cout << "formatType: " << infoAudioHeader[3] << endl;
		
		// number of channels
		vector<int> nChannels;
		nChannels.insert(nChannels.begin(), audioHeader.begin()+97, audioHeader.begin()+99);
		infoAudioHeader.push_back(auxF.binary_to_integer(nChannels, 2));
		cout << "nChannels: " << infoAudioHeader[4] << endl;
		 
		
		return infoAudioHeader;
	}	
	
	void decompress(char *compressFile, char *wavOutputFile, int predType)
	{
		BitStream bsR { compressFile, 'r'};
		Golomb g {compressFile, 1}; 
		Predictor pred { predType };
		
		cout << "\n----------------- DECONDING -------------------------\n" << endl;
		
		int totalBits = (bsR.binaryFile_size()*8) - padding;
		vector<int> totalData = bsR.read_Nbits(totalBits);	// header + payload
		cout << "totalData_size: " << totalData.size() << endl;
		
		vector<int> compressHeader;	// read compressed Header 
		compressHeader.insert(compressHeader.begin(), totalData.begin(), totalData.begin()+131);
		
		// Decoding Golomb's m-value 
		int mGolomb;
		vector<int> m;
		m.insert(m.begin(), compressHeader.begin(), compressHeader.begin()+32);
		mGolomb = auxF.binary_to_integer(m, 32);
		g.set_m(mGolomb);
		cout << "mGolomb: " << mGolomb << endl;
		
		
		// Decoding header of the audio file
		vector<int> infoAudioCompress;
		infoAudioCompress.insert(infoAudioCompress.begin(), compressHeader.begin()+32, compressHeader.begin()+131);
		vector<int> infoAudioFile = decode_HeaderOfAudioFile(infoAudioCompress);
		
		
		vector<short> payload;
		if (infoAudioFile[0] == 0)
		{
			// Lossless
			
			// Decoding residual samples
			vector<int> compressPayload;
			compressPayload.insert(compressPayload.begin(), totalData.begin()+131, totalData.begin()+totalBits);
			payload = g.decoder(compressPayload); 
			//cout << "payload.size():" << payload.size() << endl;
			
			for (int i = 0; payload.size(); i++)
			{
				cout << payload[i]<< endl;
			}
			
		
		}else{
			// Lossy
			
			// É necessário obter mais 5 bits do header => fator de quantizacao
			vector<int> compressQFactor;
			compressQFactor.insert(compressQFactor.begin(), totalData.begin()+131, totalData.begin()+135);
			
			int quantizationFactor = auxF.binary_to_integer(compressQFactor, 4);
			cout << "fator quant: " << quantizationFactor << endl;
			
			vector<int> compressPayload;
			compressPayload.insert(compressPayload.begin(), totalData.begin()+135, totalData.begin()+totalBits);
			vector<short> payloadWithError = g.decoder(compressPayload);	
			
			// atualizar o valor da amostra residual/sincronismo do erro
			for (size_t i = 0; i < payloadWithError.size(); i++)
			{
				//cout << payloadWithError[i]  << endl;
				payload.push_back(payloadWithError[i] << quantizationFactor);
			}
		}
		
		//vector<short> orignalSamples_Xn;
		//orignalSamples_Xn = pred.reversePred_separateChannels(payload); 
		
		//cout << "orignalSamples_Xn.size():" << orignalSamples_Xn.size() << endl;
		/*
		for (int i = 0; i < orignalSamples_Xn.size(); i++)
		{
			cout << orignalSamples_Xn[i] << endl;
		}*/
		
		/*
		SF_INFO sfinfoOut ;
		sfinfoOut.frames = infoAudioFile[1];
		sfinfoOut.samplerate = infoAudioFile[2];
		sfinfoOut.format = infoAudioFile[3];
		sfinfoOut.channels = infoAudioFile[4];
		
		SNDFILE * outfile = sf_open(wavOutputFile, SFM_WRITE, &sfinfoOut);
		sf_count_t count = sf_write_short(outfile, &orignalSamples_Xn[0], orignalSamples_Xn.size()); 
		sf_write_sync(outfile);
		sf_close(outfile);*/
	}
};





