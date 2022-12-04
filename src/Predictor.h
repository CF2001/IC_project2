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
	
	/******************************** LOSSLESS -- separate Channels **************************************/
	
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
	vector<short> predLossless_separateChannels(const vector<short> &origSamples)		
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
	
	/************************************** LOSSY -- separate Channels *****************************************/
	
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
		
		/*
		for (size_t i = 0; i < sResidual.size(); i++)
		{
			cout << sResidual[i] << endl;
		}
		*/
		
		return sResidual;
	}
	
	
	/****************************** Reverse predictor -- separate Channels *****************************/
	
	
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
		
		vector<short> origChannel0;
		vector<short> origChannel1;	
		 
		if (orderRsd == 1)	// xn = r(n) + x(n-1)
		{
			//cout << "Preditor 1" << endl; 
			for(size_t n = 0; n < rChannel0.size(); n++) 
			{
				if (n == 0) 
				{
					origChannel0.push_back(rChannel0[n]);	
					origChannel1.push_back(rChannel1[n]);
					originalSamples.push_back(rChannel0[n]);	
					originalSamples.push_back(rChannel1[n]);	
				}else{
					origChannel0.push_back(rChannel0[n] + origChannel0[n-1]);
				    	origChannel1.push_back(rChannel1[n] + origChannel1[n-1]);
				    	originalSamples.push_back(origChannel0[n]);
				    	originalSamples.push_back(origChannel1[n]);
				}
			}
		}else if (orderRsd == 2)	// xn = r(n) + (2*x(n-1) - x(n-2))
		{
			
			for(size_t n = 0; n < rChannel0.size(); n++) 
			{
				if (n == 0 || n == 1)
				{
					origChannel0.push_back(rChannel0[n]);	
					origChannel1.push_back(rChannel1[n]);
					originalSamples.push_back(rChannel0[n]);	
					originalSamples.push_back(rChannel1[n]);	
				}else{
					origChannel0.push_back(rChannel0[n] + (2*origChannel0[n-1]-origChannel0[n-2]));
				    	origChannel1.push_back(rChannel1[n] + (2*origChannel1[n-1]-origChannel1[n-2]));
				    	originalSamples.push_back(origChannel0[n]);
				    	originalSamples.push_back(origChannel1[n]);
				}
			}
			
		}else		// xn = r(n) + (3*x(n-1) + 3x(n-2) - x(n-3))
		{
			for (size_t n = 0; n < rChannel0.size(); n++)
			{
				if (n == 0 || n == 1 || n == 2)	
				{
					origChannel0.push_back(rChannel0[n]);	
					origChannel1.push_back(rChannel1[n]);
					originalSamples.push_back(rChannel0[n]);	
					originalSamples.push_back(rChannel1[n]);	
				}
				else
				{
					origChannel0.push_back(rChannel0[n] + (3*origChannel0[n-1] - 3*origChannel0[n-2] + origChannel0[n-3]));
				    	origChannel1.push_back(rChannel1[n] + (3*origChannel1[n-1] - 3*origChannel1[n-2] + origChannel1[n-3]));
				    	originalSamples.push_back(origChannel0[n]);
				    	originalSamples.push_back(origChannel1[n]);
				}
			}
		}
		return originalSamples;
	}
	
	/******************************** LOSSLESS -- joined Channels **************************************/
	
	vector<short> predLossless_joinChannels(const vector<short> &origSamples)		
	{		
		// canal aos qual é adicionado 0 no inicio
		vector <short> newSamples;
		
		if (orderRsd == 1)		// rn_1 = x(n) - x(n-1) 
		{
			for (size_t n = 0; n < origSamples.size(); n++)
			{
				if (n == 0)
				{
					// add a zero at the beginning
					newSamples.push_back(0);	
				}else{
					newSamples.push_back(origSamples[n-1]);
				}
				sResidual.push_back(origSamples[n]-newSamples[n]);  // residual sample
			}
		}
		else if (orderRsd == 2)	// rn_2 = x(n) - 2*x(n-1) - x(n-2)
		{
			for (size_t n = 0; n < origSamples.size(); n++)
			{
				if (n == 0 || n == 1)	// add a zero ate the index 0 and 1
				{
					newSamples.push_back(0);	
				}
				else
				{ 
					newSamples.push_back(2*origSamples[n-1] - origSamples[n-2]);
				}
				sResidual.push_back(origSamples[n]-newSamples[n]);
			}
		}
		else // orderRsd == 3		// rn_3 = x(n) - 3*x(n-1) -3x(n-2) + x(n-3)
		{
			for (size_t n = 0; n < origSamples.size(); n++)
			{
				if (n == 0 || n == 1 || n == 2)	// add a zero ate the index 0,1,2
				{
					newSamples.push_back(0);
				}
				else
				{
					newSamples.push_back(3*origSamples[n-1] - 3*origSamples[n-2] + origSamples[n-3]);
				}
				sResidual.push_back(origSamples[n]-newSamples[n]);
			}
		}
		return sResidual;
	}
	
	/************************************** LOSSY -- joined Channels *****************************************/
	
	vector<short> predLossy_joinChannels(const vector<short> &origSamples, int quantization_factor)
	{
		vector <short> copyOrigSamples; 
		for (short s : origSamples){
			copyOrigSamples.push_back(s);			
		}
		
		// canais aos quais sao adicionais 0 no inicio
		vector <short> newSamples;
		if (orderRsd == 1)		// rn_1 = (x(n) - x(n-1)) >>  resolutionBits
		{
			for (size_t n = 0; n < copyOrigSamples.size(); n++)
			{
				if (n == 0)
				{
					// add a zero at the beginning
					newSamples.push_back(0);	
				}else{
					newSamples.push_back(copyOrigSamples[n-1]);
				}
				
				
				// left/right residual sample com a redução dos bits de resolução da amostra 
				sResidual.push_back((copyOrigSamples[n]-newSamples[n]) >> quantization_factor);  
				
				// atualiza o valor da amostra para realizar o sincronismo do erro
				// (residual sample quantizada + amostra da predição)
				copyOrigSamples[n] = (sResidual[n] << quantization_factor) + newSamples[n];
			}
		}
		else if (orderRsd == 2)	// rn_2 = x(n) - 2*x(n-1) - x(n-2)
		{
			for (size_t n = 0; n < copyOrigSamples.size(); n++)
			{
				if (n == 0 || n == 1)	// add a zero ate the index 0 and 1
				{
					newSamples.push_back(0);
				}
				else
				{ 
					newSamples.push_back(2*copyOrigSamples[n-1] - copyOrigSamples[n-2]);
				}
				sResidual.push_back((copyOrigSamples[n]-newSamples[n]) >> quantization_factor);  // reduzir bits 
				copyOrigSamples[n] = (sResidual[n] << quantization_factor) + newSamples[n];    // sincronismo do erro
			}
		}
		else // orderRsd == 3		// rn_3 = x(n) - 3*x(n-1) -3x(n-2) + x(n-3)
		{
			for (size_t n = 0; n < copyOrigSamples.size(); n++)
			{
				if (n == 0 || n == 1 || n == 2)	// add a zero ate the index 0,1,2
				{
					newSamples.push_back(0);
				}
				else
				{
					newSamples.push_back(3*copyOrigSamples[n-1] - 3*copyOrigSamples[n-2] + copyOrigSamples[n-3]);
				}
				sResidual.push_back((copyOrigSamples[n]-newSamples[n]) >> quantization_factor);  // reduzir bits 
				copyOrigSamples[n] = (sResidual[n] << quantization_factor) + newSamples[n];    // sincronismo do erro
			}
		}
		return sResidual;
	}
	
	/****************************** Reverse predictor -- joined Channels *****************************/
	vector<short> reversePred_joinChannels(vector<short> &residualSamples)
	{		
		vector<short> origAux;
		 
		if (orderRsd == 1)	// xn = r(n) + x(n-1)
		{
			for(size_t n = 0; n < residualSamples.size(); n++) 
			{
				if (n == 0) 
				{
					origAux.push_back(residualSamples[n]);	
					originalSamples.push_back(residualSamples[n]);	
						
				}else{
					origAux.push_back(residualSamples[n] + origAux[n-1]);
				    	originalSamples.push_back(origAux[n]);
				    	
				}
			}
		}else if (orderRsd == 2)	// xn = r(n) + (2*x(n-1) - x(n-2))
		{
			
			for(size_t n = 0; n < residualSamples.size(); n++) 
			{
				if (n == 0 || n == 1)
				{
					origAux.push_back(residualSamples[n]);	
					originalSamples.push_back(residualSamples[n]);	
					
				}else{
					origAux.push_back(residualSamples[n] + (2*origAux[n-1]-origAux[n-2]));
				    	originalSamples.push_back(origAux[n]);
				}
			}
			
		}else		// xn = r(n) + (3*x(n-1) + 3x(n-2) - x(n-3))
		{
			for (size_t n = 0; n < residualSamples.size(); n++)
			{
				if (n == 0 || n == 1 || n == 2)	
				{
					origAux.push_back(residualSamples[n]);	
					originalSamples.push_back(residualSamples[n]);	
				}
				else
				{
					origAux.push_back(residualSamples[n] + (3*origAux[n-1] - 3*origAux[n-2] + origAux[n-3]));
				    	originalSamples.push_back(origAux[n]);
				}
			}
		}
		return originalSamples;
	}
	
	/******************************** LOSSLESS -- mid + side Channels **************************************/
	
	vector<short> predLossless_MidSideChannels(const vector<short> &origSamples)		
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
		
		vector<short> mid, side;
		vector<short> mid_side;
		
		// Calculate MID channel = (L + R)/2 
		for (size_t s = 0; s < channel0.size(); s++)
		{
			mid.push_back((channel0[s]+channel1[s])/2);
		}
		// Calculate SIDE channel = (L - R)/2
		for (size_t s = 0; s < channel0.size(); s++)
		{
			side.push_back((channel0[s]-channel1[s])/2);
		}
		
		// Calculate MID + SIDE
		for (size_t s = 0; s < mid.size(); s++)
		{
			mid_side.push_back(mid[s]+side[s]);
		}
		
		// canais aos quais sao adicionais 0 no inicio
		vector <short> newSamples;
		
		if (orderRsd == 1)		// rn_1 = x(n) - x(n-1) 
		{
			//cout << "Predictor 1" << endl;
			for (size_t n = 0; n < mid_side.size(); n++)
			{
				if (n == 0)
				{
					// add a zero at the beginning
					newSamples.push_back(0);	
				}else{
					newSamples.push_back(mid_side[n-1]);
				}
				sResidual.push_back(mid_side[n]-newSamples[n]);  // left residual sample
			}
		}
		else if (orderRsd == 2)	// rn_2 = x(n) - 2*x(n-1) - x(n-2)
		{
			for (size_t n = 0; n < mid_side.size(); n++)
			{
				if (n == 0 || n == 1)	// add a zero ate the index 0 and 1
				{
					newSamples.push_back(0);	
				}
				else
				{ 
					newSamples.push_back(2*mid_side[n-1] - mid_side[n-2]);
				}
				sResidual.push_back(mid_side[n]-newSamples[n]);
			}
		}
		else // orderRsd == 3		// rn_3 = x(n) - 3*x(n-1) -3x(n-2) + x(n-3)
		{
			for (size_t n = 0; n < mid_side.size(); n++)
			{
				if (n == 0 || n == 1 || n == 2)	// add a zero ate the index 0,1,2
				{
					newSamples.push_back(0);
				}
				else
				{
					newSamples.push_back(3*mid_side[n-1] - 3*mid_side[n-2] + mid_side[n-3]);
				}
				sResidual.push_back(mid_side[n]-newSamples[n]);
			}
		}
		return sResidual;
	}
	
	/************************************** LOSSY -- mid + side Channels *****************************************/
	
	vector<short> predLossy_MidSideChannels(const vector<short> &origSamples, int quantization_factor)
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
		
		vector<short> mid, side;
		vector<short> mid_side;
		
		// Calculate MID channel = (L + R)/2 
		for (size_t s = 0; s < channel0.size(); s++)
		{
			mid.push_back((channel0[s]+channel1[s])/2);
		}
		// Calculate SIDE channel = (L - R)/2
		for (size_t s = 0; s < channel0.size(); s++)
		{
			side.push_back((channel0[s]-channel1[s])/2);
		}
		
		// Calculate MID + SIDE
		for (size_t s = 0; s < mid.size(); s++)
		{
			mid_side.push_back(mid[s]+side[s]);
		}
		
		vector <short> newSamples;
	
		if (orderRsd == 1)		// rn_1 = (x(n) - x(n-1)) >>  resolutionBits
		{
			for (size_t n = 0; n < mid_side.size(); n++)
			{
				if (n == 0)
				{
					// add a zero at the beginning
					newSamples.push_back(0);	
				}else{
					newSamples.push_back(mid_side[n-1]);
				}
				sResidual.push_back((mid_side[n]-newSamples[n]) >> quantization_factor);  
				// atualiza o valor da amostra para realizar o sincronismo do erro
				// (residual sample quantizada + amostra da predição)
            			mid_side[n] = (sResidual[n] << quantization_factor) + newSamples[n];
			}
		}
		else if (orderRsd == 2)	// rn_2 = x(n) - 2*x(n-1) - x(n-2)
		{
			for (size_t n = 0; n < mid_side.size(); n++)
			{
				if (n == 0 || n == 1)	// add a zero ate the index 0 and 1
				{
					newSamples.push_back(0);
				}
				else
				{ 
					newSamples.push_back(2*mid_side[n-1] - mid_side[n-2]);
				}
				sResidual.push_back((mid_side[n]-newSamples[n]) >> quantization_factor);  // reduzir bits  
				mid_side[n] = (sResidual[n] << quantization_factor) + newSamples[n];    // sincronismo do erro
			}
		}
		else // orderRsd == 3		// rn_3 = x(n) - 3*x(n-1) -3x(n-2) + x(n-3)
		{
			for (size_t n = 0; n < mid_side.size(); n++)
			{
				if (n == 0 || n == 1 || n == 2)	// add a zero ate the index 0,1,2
				{
					newSamples.push_back(0);
				}
				else
				{
					newSamples.push_back(3*mid_side[n-1] - 3*mid_side[n-2] + mid_side[n-3]);
				}
				sResidual.push_back((mid_side[n]-newSamples[n]) >> quantization_factor);  // reduzir bits 
				mid_side[n] = (sResidual[n] << quantization_factor) + newSamples[n];    // sincronismo do erro
			}
		}
		return sResidual;
	}
	
	
	/****************************** Reverse predictor -- mid + side Channels *****************************/
	
	vector<short> reversePred_MidSideChannels(vector<short> &residualSamples)
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
		
		vector<short> midR, sideR;
		vector<short> mid_sideR;
		
		// Calculate MID channel = (L + R)/2 
		for (size_t s = 0; s < rChannel0.size(); s++)
		{
			midR.push_back((rChannel0[s]+rChannel1[s])/2);
		}
		// Calculate SIDE channel = (L - R)/2
		for (size_t s = 0; s < rChannel0.size(); s++)
		{
			sideR.push_back((rChannel0[s]-rChannel1[s])/2);
		}
		
		// Calculate MID + SIDE
		for (size_t s = 0; s < midR.size(); s++)
		{
			mid_sideR.push_back(midR[s]+sideR[s]);
		}

		
		vector<short> origAux;
		 
		if (orderRsd == 1)	// xn = r(n) + x(n-1)
		{
			for(size_t n = 0; n < mid_sideR.size(); n++) 
			{
				if (n == 0) 
				{
					origAux.push_back(mid_sideR[n]);	
					originalSamples.push_back(mid_sideR[n]);		
				}else{
					origAux.push_back(mid_sideR[n] + origAux[n-1]);
				    	originalSamples.push_back(origAux[n]);
				}
			}
		}else if (orderRsd == 2)	// xn = r(n) + (2*x(n-1) - x(n-2))
		{
			
			for(size_t n = 0; n < mid_sideR.size(); n++) 
			{
				if (n == 0 || n == 1)
				{
					origAux.push_back(mid_sideR[n]);	
					originalSamples.push_back(mid_sideR[n]);		
				}else{
					origAux.push_back(mid_sideR[n] + (2*origAux[n-1]-origAux[n-2]));
				    	originalSamples.push_back(origAux[n]);
				}
			}
			
		}else		// xn = r(n) + (3*x(n-1) + 3x(n-2) - x(n-3))
		{
			for (size_t n = 0; n < mid_sideR.size(); n++)
			{
				if (n == 0 || n == 1 || n == 2)	
				{
					origAux.push_back(mid_sideR[n]);	
					originalSamples.push_back(mid_sideR[n]);		
				}
				else
				{
					origAux.push_back(mid_sideR[n] + (3*origAux[n-1] - 3*origAux[n-2] + origAux[n-3]));
				    	originalSamples.push_back(origAux[n]);
				}
			}
		}
		return originalSamples;
	}
};

