#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <fstream>
#include <vector>

#include "BitStream.h"

using namespace std;


class Golomb {

	private: 
	        char *binaryFile;
		int mGolomb;	// mGolomb > 0
	
	public: 
	
	/***
	* fname - binary file name
	* M - golomb argument
	*/
	Golomb(char *fname, int M)	
	{	
		if (M < 0)
		{
			cerr << " m is greater than 0 \n";
			exit(1);
		}
		
		mGolomb = M;
		binaryFile = fname;
	}
	
	void set_m(int M)
	{
		mGolomb = M;
	}
	
	
	/*** Put from the most significant bit to the least significant bit ***/
	string mostSigtBit_to_leastSigBit(string binTmp)
	{
		string binNumber { };
		int idx_lastSigBit = binTmp.size() - 1;
		for (int i = idx_lastSigBit; i >= 0 ; i--)
		{	
			binNumber += binTmp[i]; 
		}
		return binNumber;
	}
	
	
	/*** Convert a positive number to binary -- from most to least significant bit ***/
	string positiveInt_to_binary(int quotient, int remainder)
	{
		string binNumber { };
		string binTmp;
		string bit { };
		
		if (quotient == 0)
			return "0";
		
		// successive divisions to obtain the binary value
		// Note: the bits are still not in the right order
		while(quotient != 0)
		{
			//bit = (remainder == 0) ? "0" : "1"; 
			bit = (remainder + '0');
			binTmp.append(bit);
			quotient = quotient / 2;
			remainder = quotient % 2;	
		}
		
		binNumber = mostSigtBit_to_leastSigBit(binTmp);
		
		return binNumber;
	}
	
	/*** Convert a integer to binary ***/
	int binary_to_integer(vector<int> binary, int b)
	{
		int number {0};
		int weigth {b-1};	// peso mais alto (se b=3-->weightMSigBit = 2)
		// Applying the sum of the bit weights
		for (int i = 0; i < b; i++)
		{
			number += (pow(2,weigth) * binary[i]);
			weigth--;
		}
		return number;
	}
	
	/*** Build the unary code ***/
	string unaryCode(int quotient)
	{
		string unaryCode {};
		if (quotient == 0)
		{
			unaryCode = "0";
		}else{
			for (int i = 0; i < quotient; i++)
				unaryCode += "1";
			unaryCode += "0";
		}
		return unaryCode; 
	}
	
	string paddingBinaryCode(int nBits, int binaryCodeBits, string binaryC)
	{
		int nbits = nBits - binaryCodeBits;
		string binaryC_tmp {};
		string newBinaryC {};
		
		for (int i = 0; i < nbits; i++)
		{
			binaryC_tmp += "0";
		}
		newBinaryC = binaryC_tmp + binaryC;
		
		return newBinaryC;
	}
	
	/*** 
	* It converts positive numbers to even numbers 
	* and negative numbers to odd numbers. 
	*/
	int fold_n(int n){
	    if (n >= 0)
		return n*2;
	    else
		return abs(n)*2-1;
	}

	/*** 
	* Unfolds the even numbers into their positive number 
	* and the odd numbers into their negative number
	*/
	int unfold_n(int n){
	    if (n % 2 == 0)
		return n/2;
	    else
		return (-1)*ceil(n/2)-1;
	}
	
	string encoder(int n)
	{
		n = fold_n(n);	
		int q = floor(n/mGolomb);	// quotient -- unary Code
		int r = n - q*mGolomb;		// remainder -- binary Code
		
		string unaryC = unaryCode(q);
		string binaryC {};
		string codeWord { }; // unaryCode + binaryCode 
		
		int nBits = ceil(log2(mGolomb));	// // the number of bits that represent the binary code
		int nValuesR = pow(2,ceil(log2(mGolomb))) - mGolomb; // values of r (r -> 0,1,m-1)
		
		if ((mGolomb & (mGolomb-1)) == 0)	// mGolomb is a power of 2
		{
			binaryC = positiveInt_to_binary(r,r%2);
			// Garantir que binaryC para a potencia de 2 tenha exatamente 'nBits' bits
			if ((int)binaryC.size() < nBits)
			{
				binaryC = paddingBinaryCode(nBits, binaryC.size(), binaryC);
			}
		}else{			// mGolomb is not a power of 2
			
			// 1º - Determine the first values of r (0,1,m-1)	
			if (r < nValuesR)
			{
				// The binary code must have 'nBits' of representation 
				nBits = floor(log2(mGolomb));	// Para potencias != 2 tem-se b-1 bits
				binaryC = positiveInt_to_binary(r, r%2);
				
				// The first nValuesR values of r are represented with nBits.
				// If binaryCode does not have the sufficient number 
				//   of bits it is necessary to add zeros behind.
				if ((int)binaryC.size() < nBits)
				{
					binaryC = paddingBinaryCode(nBits, binaryC.size(), binaryC);
				}
			}else{	
				int newR = r + pow(2, nBits) - mGolomb;
				binaryC = positiveInt_to_binary(newR, newR%2);
				
				// If binaryCode does not have the sufficient number 
				//   of bits it is necessary to add zeros behind.
				if ((int)binaryC.size() < nBits)
				{
					binaryC = paddingBinaryCode(nBits, binaryC.size(), binaryC); 
				}
			}
		}
		codeWord = unaryC + binaryC;
		
		return codeWord;
	}
	
	
	int encoder_writeToBinFile (string codeWord)
	{
		int padding = 0;
		BitStream bsW { binaryFile, 'w'}; 
		
		while (codeWord.size() % 8 != 0)
		{
			codeWord += "0"; // add padding 
			padding++;
		}
		bsW.write_Nbits(codeWord);
		
		return padding;
	}
	
	
	vector<short> decoder(vector<int> &codeWords)
	{	
		int q { 0 }; 
		int r { 0 };
		int n { 0 };
		
		vector<short> samples;
		
		
		int b = ceil(log2(mGolomb));
		vector<int> binary;
		bool unaryC = true;
		int readBits_powerTwo = b;
		int readBits_notPowerTwo = b-1;
		
		int bit0_unaryC = 0;
		int auxCount = 0;
		
		for (size_t bit = 0; bit < codeWords.size(); bit++)	
		{
			if (codeWords[bit] == 1 && unaryC == true)
			{
				q++;
				bit0_unaryC = q;	// fixo 
				auxCount = q;
			}else{
				unaryC = false;
				auxCount++;	// Se auxCout++
				
				if (auxCount != (bit0_unaryC+1))
				{
					// mGolomb is a power of 2
					if ((mGolomb & (mGolomb-1)) == 0)
					{
						// ler b bits !!! 
						binary.push_back(codeWords[bit]);
						readBits_powerTwo--;
						
						if (readBits_powerTwo == 0)
						{
							r = binary_to_integer(binary, b);
							
							n = q * mGolomb + r;			
							n = unfold_n(n);
							samples.push_back(n); 
							
							// Initialize all values
							unaryC = true;
							readBits_powerTwo = b;
							bit0_unaryC = 0;
							auxCount = 0;
							binary.clear();
							q = 0;
							r = 0;
							n = 0;
						}
					}else{
						// ler b-1 bits !!! 
						binary.push_back(codeWords[bit]);
						readBits_notPowerTwo--;
						
						if (readBits_notPowerTwo == 0)	// apenas para preencher binary com a parteBinaria
						{
							r = binary_to_integer(binary, b-1);
						
							/** 
							* pow(2,b)-mGolomb: representa os primeiros valores de r que  
							*	       contem b bits de representacao 
							*/       
							if (r < (pow(2,b)-mGolomb))
							{
								n = q * mGolomb + r;			
								n = unfold_n(n);
								
								// Initialize all values
								samples.push_back(n); 
								unaryC = true;
								readBits_notPowerTwo = b-1;
								bit0_unaryC = 0;
								auxCount = 0;
								binary.clear();
								q = 0;
								r = 0;
								n = 0;
							
							}
						}else if (readBits_notPowerTwo == -1)	// If we have to read +1 bit 
						{
							
							r = 0; // r passa a ser diferente 
							r = binary_to_integer(binary, b);
							
							n = mGolomb*q + r;
							n = unfold_n(n - (pow(2, b) - mGolomb));
							
							// Initialize all values
							samples.push_back(n); 
							unaryC = true;
							readBits_notPowerTwo = b-1;
							bit0_unaryC = 0;
							auxCount = 0;
							binary.clear();
							q = 0;
							r = 0;
							n = 0;
						}			
					}
				}
			}
		}
		return samples;
	}
	
	int getOptimalm(const vector<short> samples)
	{
		double sum_m = 0;
		double mean = 0;
		int m = 0;
		
		for (size_t i = 0; i < samples.size(); i++)
		{
			sum_m += fold_n(samples[i]);
		}
		
		mean = sum_m / samples.size();
		m = ceil(-1/log2(mean/(mean+1)));
		
		return m;
	}
	

};

