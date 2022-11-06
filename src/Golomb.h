#include <iostream>
#include <string>
#include <cstring>

using namespace std;

class Golomb {
	
	private: 
	
	public: 
	
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
	
	
	/*** Convert a positive number to 32-bit binary ***/
	string positiveInt_to_binary(int quotient, int remainder)
	{
		string binNumber { };
		string binTmp;
		string bit { };
		int nPadding;		// To get 32 bits
		
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
		
		// Get a 32-bit binary value 
		nPadding = 32 - binTmp.size();
		while(nPadding != 0)
		{
			binTmp += "0";
			nPadding--;
		}
		
		binNumber = mostSigtBit_to_leastSigBit(binTmp);
		
		return binNumber;
	}
	
	/*** Apply the one's complement ***/
	string oneComplement(string binTmp)
	{
		string binOneComplement = { };
		string notBit { };
		for (size_t i = 0; i < binTmp.size(); i++)
		{
			notBit = (binTmp[i] == '0') ? "1" : "0";
			binOneComplement += notBit;
		}
		return binOneComplement;
	}
	
	/*** Apply the two's complement ***/
	string twoComplement(string binTmp)
	{	
		int carry_out { 0 };
		int bitAux { 0 };
		int bitRes { 0 };
		string binTwoComplement { };
		string binOneComplement { };
		
		binOneComplement = oneComplement(binTmp);
		int lastSig_bit { (int)binOneComplement.size()-1 };
		
		
		for (int i = lastSig_bit; i >= 0; i--)
		{
			cout << "Iteracao: " << i << " Bit: "<< binOneComplement[i] << endl;
			
			if(i == lastSig_bit)
			{
				cout << "last BIT" << endl;
				//bitAux = ((binOneComplement[i] == '0') ? 0 : 1) + 1 + carry_out;
				bitAux = (binOneComplement[i]-'0') + 1 + carry_out;
				cout << "Bit aux: " << bitAux << endl;
			}else{
				cout << "another BIT" << bitAux << endl;
				//bitAux = ((binOneComplement[i] == '0') ? 0 : 1) + 0 + carry_out;
				bitAux = (binOneComplement[i]-'0') + 0 + carry_out;
				cout << "Bit aux: " << bitAux << endl;
			}
			
			if (bitAux == 2) 
			{
				bitRes = 2 - bitAux;
				cout << "bitRES: " << bitRes << endl;
				carry_out = 1;
				cout << "Carry_out: " << carry_out << endl;
			}else{
				bitRes = bitAux;
				cout << "bitRES: " << bitRes << endl;
				carry_out = 0;
				cout << "Carry_out: " << carry_out << endl;
			}
			//binTwoComplement += ((bitRes == 0) ? "0" : "1");
			binTwoComplement += (bitRes + '0');
		}
	
		return binTwoComplement;
	}
	
	/***  
	*    ENCODER
	*	
	*    Encodes an integer => generating the corresponding string of bits
	*    int => 4 bytes => 32 bits
	***/
	string encoder(int integer)
	{
		string binNumber { };
		string bit { };
		string binTmp { };
		string binN_1complement { };
		string binTwoComplement { };
		string notBit { };	// To obtain one's complement
		
		int q = integer ;	// quotient
		int r = integer % 2;	// remainder
		
		// Check if the integer value is positive or negative or 0
		if (integer >= 0)
		{
			binNumber = positiveInt_to_binary(q, r);
			
			cout << binNumber << endl;
			
			return binNumber;
		}else{

			binTmp = positiveInt_to_binary(q, r);
			binTwoComplement = twoComplement(binTmp);
			binNumber = mostSigtBit_to_leastSigBit(binTwoComplement);

			cout << binNumber << endl;
			
			return binNumber;
		}
		return " ";
	}
	
	
	
	/***  
	*    DECODER
	*	
	*    Decodes a string of bits => generating the corresponding integer
	***/
	int decoder()
	{
		return 0;
	}

};



