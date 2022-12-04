#include <string>
#include <cstring>
#include <cmath>

using namespace std;

class AuxiliarFunc 
{
	public: 
		
	/*** Convert a positive number to binary ***/
	string positiveInt_to_binary(int quotient, int remainder)
	{
		string binNumber { };
		string bit { };
		
		if (quotient == 0)
			return "0";
		
		// successive divisions to obtain the binary value
		// Note: the bits are still not in the right order
		while(quotient != 0)
		{
			//bit = (remainder == 0) ? "0" : "1"; 
			bit = (remainder + '0');
			binNumber.append(bit);
			quotient = quotient / 2;
			remainder = quotient % 2;	
		}
		
		return binNumber;
	}
	
	/*** Convert a integer to binary ***/
	int binary_to_integer(vector<int> binary, int b)
	{
		int number {0};
		size_t weigth {0};
		// Applying the sum of the bit weights
		for (int i = 0; i < b; i++)
		{
			number += (pow(2,weigth) * binary[i]);
			weigth++;
		}
		return number;
	}
	
	/** Get a padding-bit binary value **/
	string paddingBits(string binary, int padding)
	{
		size_t nPadding = padding - binary.size();
		while(nPadding != 0)
		{
			binary += "0";
			nPadding--;
		}
		return binary;
	}

};



