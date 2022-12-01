#include <iostream>
#include <vector>

class Predictor
{

	private: 
		int orderRsd;			// residual order 
		vector<short> sResidual;	// samples residuals
		vector<short> originalSamples;// originals samples (final) 	
		
	
	public:
	
	Predictor(int order)
	{
		orderRsd = order;
	}
	
	/******************************** LOSSLESS **************************************/
	
	/**
	* Polynomial predictor:
	* x(n)⁰ = 0
	* x(n)¹ = x(n-1)
	* x(n)² = 2x(n-1) - x(n-2)
	* x(n)³ = 3x(n-1) - 3x(n-2) + 3x(n-3)
	*/
	/*** Corresponding residuals
	* r(n)⁰ = x(n)
	* r(n)¹ = r(n)⁰ - r(n-1)⁰
	* r(n)² = r(n)¹ - r(n-1)¹
	* r(n)³ = r(n)² - r(n-1)²
	**/
	vector<short> predLossless_separateChannels(const vector<short> &origSamples)		// CORRETO !!!!!
	{		
		// Separate Samples
		vector <short> channel0;
		vector <short> channel1; 
		size_t n { };
		for (short s : origSamples){
			if (n%2 == 0){	// Channel 0
				channel0.push_back(s);			
			}else{	// Channel 1
				channel1.push_back(s);
			}
			n++;
		}
		
		// canais aos quais sao adicionais 0 no inicio
		vector <short> newchannel0;
		vector <short> newchannel1; 
		
		if (orderRsd == 1)		// rn_1 = x(n) - x(n-1) 
		{
			//cout << "Predictor 1" << endl;
			for (size_t n = 0; n < channel0.size(); n++)
			{
				if (n == 0)
				{
					// add a zero at the beginning
					newchannel0.push_back(0);	
					newchannel1.push_back(0);
				}else{
					newchannel0.push_back(channel0[n-1]);
					newchannel1.push_back(channel1[n-1]);
				}
				sResidual.push_back(channel0[n]-newchannel0[n]);  // left residual sample
				sResidual.push_back(channel1[n]-newchannel1[n]);  // right residual sample
			}
		}
		else if (orderRsd == 2)	// rn_2 = x(n) - 2*x(n-1) - x(n-2)
		{
			for (size_t n = 0; n < channel0.size(); n++)
			{
				if (n == 0 || n == 1)	// add a zero ate the index 0 and 1
				{
					newchannel0.push_back(0);
					newchannel1.push_back(0);	
				}
				else
				{ 
					newchannel0.push_back(2*channel0[n-1] - channel0[n-2]);
					newchannel1.push_back(2*channel1[n-1] - channel1[n-2]);
				}
				sResidual.push_back(channel0[n]-newchannel0[n]);
				sResidual.push_back(channel1[n]-newchannel1[n]);
			}
		}
		else // orderRsd == 3		// rn_3 = x(n) - 3*x(n-1) -3x(n-2) + x(n-3)
		{
			for (size_t n = 0; n < channel0.size(); n++)
			{
				if (n == 0 || n == 1 || n == 2)	// add a zero ate the index 0,1,2
				{
					newchannel0.push_back(0);
					newchannel1.push_back(0);
				}
				else
				{
					newchannel0.push_back(3*channel0[n-1] - 3*channel0[n-2] + channel0[n-3]);
					newchannel1.push_back(3*channel1[n-1] - 3*channel1[n-2] + channel1[n-3]);
				}
				sResidual.push_back(channel0[n]-newchannel0[n]);
				sResidual.push_back(channel1[n]-newchannel1[n]);
			}
		}
		/*
		for (int i = 0; i < sResidual.size(); i++)
		{
			cout << sResidual[i] << endl;
		}
		*/
		return sResidual;
	}
	
	/************************************** LOSSY *****************************************/
	
	vector<short> predLossy_separateChannels(const vector<short> &origSamples, int quantization_factor)
	{
		// Separate Samples
		vector <short> channel0;
		vector <short> channel1; 
		size_t n { };
		for (short s : origSamples){
			if (n%2 == 0){	// Channel 0
				channel0.push_back(s);			
			}else{	// Channel 1
				channel1.push_back(s);
			}
			n++;
		}
		
		// canais aos quais sao adicionais 0 no inicio
		vector <short> newchannel0;
		vector <short> newchannel1; 
	
		if (orderRsd == 1)		// rn_1 = (x(n) - x(n-1)) >>  resolutionBits
		{
			for (size_t n = 1; n < channel0.size(); n++)
			{
				if (n == 0)
				{
					// add a zero at the beginning
					newchannel0.push_back(0);	
					newchannel1.push_back(0);
				}else{
					newchannel0.push_back(channel0[n-1]);
					newchannel1.push_back(channel1[n-1]);
				}
				
				// left/right residual sample com a redução dos bits de resolução da amostra 
				sResidual.push_back((channel0[n]-newchannel0[n]) >> quantization_factor);  
				sResidual.push_back((channel1[n]-newchannel1[n]) >> quantization_factor); 
				
				// atualiza o valor da amostra para realizar o sincronismo do erro
				// (residual sample quantizada + amostra da predição)
				channel0[n] = (sResidual[2*n] << quantization_factor) + newchannel0[n];
            			channel1[n] = (sResidual[2*n+1] << quantization_factor) + newchannel1[n];
			}
		}
		else if (orderRsd == 2)	// rn_2 = x(n) - 2*x(n-1) - x(n-2)
		{
			for (size_t n = 0; n < channel0.size(); n++)
			{
				if (n == 0 || n == 1)	// add a zero ate the index 0 and 1
				{
					newchannel0.push_back(0);
					newchannel1.push_back(0);	
				}
				else
				{ 
					newchannel0.push_back(2*channel0[n-1] - channel0[n-2]);
					newchannel1.push_back(2*channel1[n-1] - channel1[n-2]);
				}
				sResidual.push_back((channel0[n]-newchannel0[n]) >> quantization_factor);  // reduzir bits 
				sResidual.push_back((channel1[n]-newchannel1[n]) >> quantization_factor);  // reduzir bits 
				channel0[n] = (sResidual[2*n] << quantization_factor) + newchannel0[n];    // sincronismo do erro
            			channel1[n] = (sResidual[2*n+1] << quantization_factor) + newchannel1[n];	 // sincronismo do erro
			}
		}
		else // orderRsd == 3		// rn_3 = x(n) - 3*x(n-1) -3x(n-2) + x(n-3)
		{
			for (size_t n = 0; n < channel0.size(); n++)
			{
				if (n == 0 || n == 1 || n == 2)	// add a zero ate the index 0,1,2
				{
					newchannel0.push_back(0);
					newchannel1.push_back(0);
				}
				else
				{
					newchannel0.push_back(3*channel0[n-1] - 3*channel0[n-2] + channel0[n-3]);
					newchannel1.push_back(3*channel1[n-1] - 3*channel1[n-2] + channel1[n-3]);
				}
				sResidual.push_back((channel0[n]-newchannel0[n]) >> quantization_factor);  // reduzir bits 
				sResidual.push_back((channel1[n]-newchannel1[n]) >> quantization_factor);  // reduzir bits 
				channel0[n] = (sResidual[2*n] << quantization_factor) + newchannel0[n];    // sincronismo do erro
            			channel1[n] = (sResidual[2*n+1] << quantization_factor) + newchannel1[n];	 // sincronismo do erro
			}
		}
		return sResidual;
	}
	
	/****************************** Reverse predictor *****************************/
	
	
	vector<short> reversePred_separateChannels(vector<short> &residualSamples)
	{
		// separate channels -- residual samples
		vector<short> rChannel0;	
		vector<short> rChannel1;
		size_t n { };
		for (short s : residualSamples){
			//cout << s << endl;
			if (n%2 == 0){	// Channel 0
				rChannel0.push_back(s);			
			}else{	// Channel 1
				rChannel1.push_back(s);
			}
			n++;
		}
		
		vector<short> newRChannel0;
		vector<short> newRChannel1;	
		 
		if (orderRsd == 1)	// xn = r(n) + r(n-1)
		{
			//cout << "Preditor 1" << endl; 
			for(size_t i = 0; i < rChannel0.size(); i++) 
			{
				if (i == 0) 
				{
					newRChannel0.push_back(0);	
					newRChannel1.push_back(0);
				}else{
					//cout << rChannel0[i-1] << endl;
					newRChannel0.push_back(rChannel0[i-1]);
				    	newRChannel1.push_back(rChannel1[i-1]);
				}	
			    	originalSamples.push_back(rChannel0[i] + newRChannel0[i]);
			    	originalSamples.push_back(rChannel1[i] + newRChannel1[i]);
			}
		}else if (orderRsd == 2)	// xn = r(n) + 2*r(n-1) + r(n-2)
		{
			for(size_t i = 0; i < rChannel0.size(); i++) 
			{
				if (i == 0 || i == 1)
				{
					newRChannel0.push_back(0);	
					newRChannel1.push_back(0);
				}else{
					newRChannel0.push_back(2*rChannel0[i-1] - rChannel0[i-2]);
			    		newRChannel1.push_back(2*rChannel1[i-1] - rChannel1[i-2]);
				}
			    	originalSamples.push_back(rChannel0[i] + newRChannel0[i]);
			    	originalSamples.push_back(rChannel1[i] + newRChannel1[i]);
		
			}
		}else		// xn = r(n) + 3*r(n-1) + 3r(n-2) - r(n-3)
		{
			for (size_t n = 0; n < rChannel0.size(); n++)
			{
				if (n == 0 || n == 1 || n == 2)	// add a zero ate the index 0,1,2
				{
					newRChannel0.push_back(0);
					newRChannel1.push_back(0);
				}
				else
				{
					newRChannel0.push_back(3*rChannel0[n-1] - 3*rChannel0[n-2] + rChannel0[n-3]);
					newRChannel1.push_back(3*rChannel1[n-1] - 3*rChannel1[n-2] + rChannel1[n-3]);
				}
				originalSamples.push_back(rChannel0[n]+newRChannel0[n]);
				originalSamples.push_back(rChannel1[n]+newRChannel1[n]);
			}
		}
		
		/*
		for (int i = 0; i < originalSamples.size(); i++)
		{
			cout << originalSamples[i] << endl;
		}
		*/
		
		return originalSamples;
	}
};

