#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <typeinfo>

/* xxd -b out.bin */

using namespace std;

class BitStream 
{
	private:
		fstream file;	// A combination of ofstream and ifstream: creates, reads, and writes to files
		char	mode;	// mode => r (read) or w (write)
		
		unsigned char buffer = 0;
		unsigned int count = 0;
		unsigned int sizeBinaryFile = 0;
	
	public:
	
		BitStream (char *fname, char m)
		{
			if (m == 'w')
			{
				/*
				* ios::out -> write to a file
				* ios::binary -> Writing to a file without translating any characters.
				*/
				file = fstream(fname, ios::out);	
				mode = m;
			}else {	// m == 'r'
			
				/*
				* ios::in -> read from a file
				*/
				file = fstream(fname, ios::in);	
				mode = m;
        		}
		}
		
		/* method to write one bit */
		void write_bit(int bit)
		{
			if (mode != 'w')
			{	
				cerr << " Can't write in \'r\' mode ";
				exit(1);  
			}
                	
			buffer = buffer << 1; // Make room for next bit 
			// Colocar o bit 1 no buffer, se for 0 já lá está
			if(bit)
			{
				buffer = buffer | 1;	// buffer | 00000001
			}
			count ++; // We have added a bit
			
			// Se buffer estiver cheio (8 bits) escreve-se no ficheiro binário.
			if(count == 8)
			{
				file << buffer;
				buffer = 0;
				count = 0;
			}
		}
		
		
		/* method to read one bit */
		char read_bit()
		{
			if (mode != 'r')
			{	
				cerr << " Can't read in \'w\' mode ";
				exit(1);  
			}

			if (count == 0)
			{
				file.read(reinterpret_cast<char*>(&buffer), 1);
				
				if (file.eof())
				{
					return '0';
				}

				count = 8;
			}
			
				
			count--;
			char bit =  (buffer >> count) & 1; // ficar com o valor 0 ou 1 do bit respetivo 
			//cout << "Bit lido na função read_bit(): " << unsigned(bit) << endl;
			return bit ;
		}
		

		
		/* method to write N bits */
		void write_Nbits(string bits)
		{
			if (mode != 'w')
			{	
				cerr << "Can't write in \'r\' mode ";
				exit(1); 
			}
			
			//cout << "bits.size(): " <<  bits.size() << endl;
			for (size_t i = 0; i < bits.size(); i++)
			{
				// '0'->0 , '1'->1
				write_bit(bits[i] - '0');	// bits[i] - '0' convert to 0 or 1 (integers) 
			} 
		}
		
		
		/* method to read N bits */
		vector <int> read_Nbits(int nBits)
		{
			if (mode != 'r')
			{	
				cerr << "Can't read in \'w\' mode ";
				exit(1);
			}
			
			int totalBits_file = binaryFile_size()*8;	
			if (nBits > totalBits_file)
			{
				cerr << "The binary file has only " << binaryFile_size()*8 << " bits.\n";
				exit(1);
			}
			
			vector<int> bits;
			int bit;
			for(int i = 0; i < nBits; i++)
			{
				bit = read_bit();
				bits.push_back(bit);
			}
			return bits;
		}
		
		/* Total size of the binary file in bytes */ 
		unsigned int binaryFile_size()
		{
			// Update the size of the binary file */
			file.seekg(0, ios::end);
			sizeBinaryFile = file.tellg();
			file.seekg(0, ios::beg);
			return sizeBinaryFile;
		}
};


