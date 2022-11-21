#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <fstream>

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
	
	
	/*** Convert a positive number to binary ***/
	string positiveInt_to_binary(int quotient, int remainder)
	{
		string binNumber { };
		string binTmp;
		string bit { };
		
		if (quotient == 0)
			return "0";
		/*else if (quotient == 1)
			return "01";*/
		
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
		int weigth {0};
		// Applying the sum of the bit weights
		for (int i = b; i >= 0; i--)
		{
			number += (pow(2,weigth) * binary[i]);
			weigth++;
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
			//cout << "Codificador - potencia de 2" << endl;
		        //unaryC = unaryCode(q);
			binaryC = positiveInt_to_binary(r,r%2);
			//nBits = ceil(log2(mGolomb));
			
			// Garantir que binaryC para a potencia de 2 tenha exatamente ceil(log2(mGolomb)) bits
			if ((int)binaryC.size() < nBits)
			{
				nBits = nBits - binaryC.size();
				string binaryC_tmp {};
				for (int i = 0; i < nBits; i++)
				{
					binaryC_tmp += "0";
				}
				binaryC = binaryC_tmp + binaryC;
			}
			//cout << "binary Code: " << binaryC << endl; 
			
			
		}else{			// mGolomb is not a power of 2
			//unaryC = unaryCode(q);
			//cout << "unary Code: " << unaryC << endl;
			
			// 1º - Determine the first values of r (0,1,m-1)	
			//nValuesR = pow(2,ceil(log2(mGolomb))) - mGolomb;
			//cout << "r: " << r << " nValuesR: " << nValuesR << endl;
			if (r < nValuesR)
			{
				// The binary code must have nBits of representation 
				nBits = floor(log2(mGolomb));	// Para potencias != 2 tem-se b-1 bits
				binaryC = positiveInt_to_binary(r, r%2);
				
				//cout << "Binary code initial: " << binaryC << endl;
				//cout << "nBits binary Code: " << binaryC.size() << endl;
				
				// The first nValuesR values of r are represented with nBits.
				// If binaryCode does not have the sufficient number 
				//   of bits it is necessary to add zeros behind.
				if ((int)binaryC.size() < nBits)
				{
					nBits = nBits - binaryC.size();
					//cout << "bits que faltam: " << nBits << endl;
					string binaryC_tmp {};
					for (int i = 0; i < nBits; i++)
					{
						binaryC_tmp += "0";
					}
					binaryC = binaryC_tmp + binaryC;
				}
				
			}else{	
				
				//nBits = ceil(log2(mGolomb));
				//cout << "nBits: " << nBits << endl;
				//r = r + nBits;  // New value of r (new binary Code)
				int newR = r + pow(2, nBits) - mGolomb;
				//cout << "new R: " << r << endl;
				binaryC = positiveInt_to_binary(newR, newR%2);
				
				//cout << "Binary code initial: " << binaryC << endl;
				//cout << "nBits binary Code: " << binaryC.size() << endl;
				
				// If binaryCode does not have the sufficient number 
				//   of bits it is necessary to add zeros behind.
				if ((int)binaryC.size() < nBits)
				{
					nBits = nBits - binaryC.size();
					string binaryC_tmp = {};
					for (int i = 0; i < nBits; i++)
					{
						binaryC_tmp += "0";
					}
					binaryC = binaryC_tmp + binaryC;
				}
			}
		}
		codeWord = unaryC + binaryC;
		
		return codeWord;
	}
	
	
	void encoder_writeToBinFile (string codeWord)
	{
		BitStream bsW { binaryFile, 'w'}; 
		
		while (codeWord.size() % 8 != 0)
		{
			codeWord += "0"; // add padding 
		}
		bsW.write_Nbits(codeWord);
	}
	
	
	int decoder()
	{
		BitStream bsR { binaryFile, 'r'}; 
		int q { 0 }; 
		int r { 0 };
		int n { 0 };
		
		// Determine unary Code
		int bitUnary {};
		while((bitUnary = bsR.read_bit()) == 1)
		{
			//cout << "unaryCode: " << bitUnary << endl; 
			q++; 
		}
		//cout << "q: " << q << endl;
		
		// mGolomb is a power of 2
		if ((mGolomb & (mGolomb-1)) == 0)
		{
			cout << "DESCODIFICADOR - potencia de 2" << endl;
			int b = ceil(log2(mGolomb));		// bit number of the binary code (b bits)
			cout << "b: " << b << endl;
			//vector<int> binary = bsR.read_Nbits(b);
			vector<int> binary;
			int BITS = b;
			while(BITS != 0)
			{
				binary.push_back(bsR.read_bit());
				BITS--;
			}
			
			//r = binary_to_integer(binary, b-1); // Get the integer(remainder) of the binary code
			
			int weigth {b-1};	// peso mais alto (se b=3-->weightMSigBit = 2)
			// Applying the sum of the bit weights
			for (int i = 0; i < b; i++)
			{
				cout << binary[i];
				r += (pow(2,weigth) * binary[i]);
				weigth--;
			}
			cout << endl;
			cout << "r_final: " << r << endl;
			
			n = q * mGolomb + r;			
			n = unfold_n(n);

			return n;
			
		}else{	// mGolomb is not a power of 2 
		
			cout << "\nNAO potencia de 2:\n";
		
			int b = ceil(log2(mGolomb));   // bit number of the binary code 
			cout << "b: " << b << endl;
			//binaryCode of power diff of 2 has less than one bit
			// le os primeiros valores da codeword , mas n le o ultimo bit do binary Code
			//vector<int> binary = bsR.read_Nbits(b-1);  
			vector<int> binary;
			int BITS = b-1;
			while(BITS != 0)
			{
				binary.push_back(bsR.read_bit());
				BITS--;
			}
			//binary[b-1] = 0;	// add bit to binary code
			
			int weigth {b-2};
			for (int i = 0; i < b-1; i++)
			{
				cout << binary[i];
				r += (pow(2,weigth) * binary[i]);
				weigth--;
			}
			cout << endl;
			cout << "r_final: " << r << endl;
			
			//r = binary_to_integer(binary, b-2);
			//cout << "r: " << r << endl;
			
			/** nota: 
			* pow(2,b)-mGolomb: representa os primeiros valores de r que  
			*	       contem b bits de representacao 
			*/           
			if (r < (pow(2,b)-mGolomb))
			{
				cout << "aqui_first_R" << endl;
				n = q * mGolomb + r;			
				n = unfold_n(n);
				cout << "n: " << n << endl;
				return n;
			
			}else{
				cout << "aqui_LAST_R" << endl;
				
				// Como r > pow(2,b)-mGolomb, entao a representacao do binary Code passa a ter
				// mais um bit !!!! Logo, falta-nos ler 1 bit para juntar ao vetor binary
				
				cout << "size: " << binary.size() << endl;
				//binary[0] = bsR.read_bit(); 
				binary.push_back(bsR.read_bit());
				cout << "size: " << binary.size() << endl;
				
				//binary.push_back(bit);
				
				//binary.push_back(bsR.read_bit());	// ler o bit de menor peso
				

				r = 0; // r passa a ser diferente 
				int weigth {b-1};
				for (int i = 0; i < b; i++)
				{
					cout << binary[i];
					r += (pow(2,weigth) * binary[i]);
					weigth--;
				}
				cout << endl;
				cout << "r_final: " << r << endl;
				
				//n = mGolomb*q + (r-b);	// r nao e diretamente o binary Code 
				n = mGolomb*q + r;
				n = unfold_n(n - (pow(2, b) - mGolomb));
				//n = unfold_n(n);
				cout << "n: " << n << endl;
				
				/*
				r = 0;
				int weigth {0};
				// Applying the sum of the bit weights
				for (int i = b-1; i >= 0; i--) // da erro se n fizer aqui diretamente por causa do r
				{
					cout << binary[i] << endl;
					r += (pow(2,weigth) * binary[i]);
					weigth++;
				}
				n = mGolomb*q + r;
				n = unfold_n(n - (pow(2, b) - mGolomb));
				cout << "n: " << n << endl;*/
				return n; 
			}
		}
		
		return 0;
	}

};






