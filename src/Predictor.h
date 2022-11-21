#include <iostream>
#include <vector>
#include <sndfile.hh>

class Predictor
{

	private: 
		int orderRsd;			// residual order 
		vector<short> sResidual;	// samples residuals
		vector<short> xnSamples;	// originals samples (final) 	
		
	
	public:
	
	Predictor(int order)
	{
		orderRsd = order;
	}
	
	void compressLossless(){
		
	}
	
	
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
	
	void predictorLossless_allChannels(const vector<short> &origSamples)
	{	
		vector<short> newSamples; 
		
		for (size_t n = 0; n < origSamples.size(); n++)
		{
			if (orderRsd == 0)		// rn_0 = x(n)
			{
				sResidual.push_back(origSamples[n]);	
			}	
			else if (orderRsd == 1)	// rn_1 = x(n) - x(n-1) (problem: n=0)
			{
				if (n == 0)	// add a zero at the beginning
					newSamples.push_back(0);	
				else
					newSamples.push_back(origSamples[n-1]);
				
				sResidual.push_back(origSamples[n]-newSamples[n]);
			}
			else if (orderRsd == 2)	// rn_2 = x(n) - 2*x(n-1) - x(n-2)
			{
				if (n == 0 || n == 1)	// add a zero ate the index 0 and 1
					newSamples.push_back(0);	
				else 
					newSamples.push_back(2*origSamples[n-1] - origSamples[n-2]);
				sResidual.push_back(origSamples[n]-newSamples[n]);
			}
			else // orderRsd == 3
			{
				if (n == 0 || n == 1 || n == 2)	// add a zero ate the index 0,1,2
					newSamples.push_back(0);
				else
					newSamples.push_back(3*origSamples[n-1] - 3*origSamples[n-2] + origSamples[n-3]);
				sResidual.push_back(origSamples[n]-newSamples[n]);
			}
				
		}	
		
		/*
		for (size_t i = 0; i < newSamples.size(); i++)
		{
			cout << newSamples[i] << endl;
		}*/
	}
	
	
	void predictorLossless_separateChannels(const vector<short> &origSamples)
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
		
		if (orderRsd == 0)		// rn_0 = x(n)
		{
			for (size_t n = 0; n < channel0.size(); n++)
			{
				sResidual.push_back(channel0[n]);	
				sResidual.push_back(channel1[n]);
			}
		}	
		else if (orderRsd == 1)		// rn_1 = x(n) - x(n-1) (problem: n=0)
		{
			for (size_t n = 0; n < channel0.size(); n++)
			{
				if (n == 0)	// add a zero at the beginning
				{
					newchannel0.push_back(0);	
					newchannel1.push_back(0);	
				}
				else
				{
					newchannel0.push_back(channel0[n-1]);
					newchannel1.push_back(channel1[n-1]);
				}
				sResidual.push_back(channel0[n]-newchannel0[n]);
				sResidual.push_back(channel1[n]-newchannel1[n]);
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
		else // orderRsd == 3				// rn_3 = x(n) - 3*x(n-1) -3x(n-2) + x(n-3)
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
	}
	
	/******************************************************************************/
	
		/***** Reverse predictors ******/
		
	
	void reversePredLosseless_allChannels(const vector<short> &origSamples)
	{
		vector<short> newSamples; 	// orignal samples with 0's at the beginning
		
		if (orderRsd == 0)		// x(n) = rn_0
		{
			for (size_t n = 0; n < origSamples.size(); n++)
				xnSamples.push_back(sResidual[n]);
		}	
		else if (orderRsd == 1)	// x(n)  = rn_1 + x(n-1)
		{
		
			for (size_t n = 0; n < origSamples.size(); n++)
			{
				if (n == 0)	// add a zero at the beginning
					newSamples.push_back(0);
				else
					newSamples.push_back(origSamples[n-1]);
				xnSamples.push_back(sResidual[n]+newSamples[n]);
			}
		}
		else if (orderRsd == 2)	// x(n) = rn_2 + 2*x(n-1) + x(n-2)
		{
			for (size_t n = 0; n < origSamples.size(); n++)
			{
				if (n == 0 || n == 1)	// add a zero ate the index 0 and 1
					newSamples.push_back(0);	
				else 
					newSamples.push_back(2*origSamples[n-1] - origSamples[n-2]);
				xnSamples.push_back(sResidual[n]-newSamples[n]);
			}
		}
		else // orderRsd == 3		// x(n) = rn_3 + 3*x(n-1) + 3x(n-2) - x(n-3)
		{
			/*
			for (size_t n = 0; n < origSamples.size(); n++)
			{
				if (n == 0 || n == 1 || n == 2)	// add a zero ate the index 0,1,2
					newSamples.push_back(0);
				else
					newSamples.push_back(3*origSamples[n-1] + 3*origSamples[n-2] - origSamples[n-3]);
				xnSamples.push_back(sResidual[n]+newSamples[n]);
			}*/
		}
		
		
		for (size_t i = 0; i < xnSamples.size(); i++)
		{
			cout << xnSamples[i] << endl;
		}
	}
	
	
	vector<short> reversePredLosseless_separateChannels(const vector<short> &origSamples)
	{	
		
		// Separate Samples
		vector <short> channel0;
		vector <short> channel1; 
		size_t count { };
		for (short s : origSamples){
			if (count%2 == 0){	// Channel 0
				channel0.push_back(s);			
			}else{	// Channel 1
				channel1.push_back(s);
			}
			count++;
		}
		
		// canais aos quais sao adicionais 0 no inicio
		vector <short> newchannel0;
		vector <short> newchannel1; 
		
		// Separate residual Samples
		vector <short> channel0R;
		vector <short> channel1R; 
		size_t n { };
		for (short s : sResidual){
			if (n%2 == 0){	// Channel 0
				channel0R.push_back(s);			
			}else{	// Channel 1
				channel1R.push_back(s);
			}
			n++;
		}
		
		if (orderRsd == 0)		// x(n) = rn_0
		{
			for (size_t n = 0; n < channel0.size(); n++)
			{
				xnSamples.push_back(channel0R[n]);	
				xnSamples.push_back(channel1R[n]);
			}
		}	
		else if (orderRsd == 1)		// rn_1 = x(n) - x(n-1) 
		{
			for (size_t n = 0; n < channel0.size(); n++)
			{
				if (n == 0)	// add a zero at the beginning
				{
					newchannel0.push_back(0);	
					newchannel1.push_back(0);	
				}
				else
				{
					newchannel0.push_back(channel0[n-1]);
					newchannel1.push_back(channel1[n-1]);
				}
				xnSamples.push_back(channel0R[n]+newchannel0[n]);
				xnSamples.push_back(channel1R[n]+newchannel1[n]);
			}
		}
		else if (orderRsd == 2)	// x(n) = rn_2 + 2*x(n-1) + x(n-2)
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
					newchannel0.push_back(2*channel0[n-1] - channel1[n-2]);
					newchannel1.push_back(2*channel0[n-1] - channel1[n-2]);
				}
				xnSamples.push_back(channel0R[n]-newchannel0[n]);
				xnSamples.push_back(channel1R[n]-newchannel1[n]);
			}
		}
		else // orderRsd == 3			// x(n) = rn_3 + 3*x(n-1) + 3x(n-2) - x(n-3)
		{
			/*
			for (size_t n = 0; n < channel0R.size(); n++)
			{
				if (n == 0 || n == 1 || n == 2)	// add a zero ate the index 0,1,2
				{
					newchannel0R.push_back(0);
					newchannel1R.push_back(0);
				}
				else
				{
					newchannel0R.push_back(3*channel0R[n-1] - 3*channel0R[n-2] + channel0R[n-3]);
					newchannel1R.push_back(3*channel1R[n-1] - 3*channel1R[n-2] + channel1R[n-3]);
				}
				xnSamples.push_back(channel0R[n]+newchannel0R[n]);
				xnSamples.push_back(channel1R[n]+newchannel1R[n]);
			}*/
		}
		/*
		for (size_t i = 0; i < xnSamples.size(); i++)
		{
			cout << xnSamples[i] << endl;
		}*/
		return xnSamples;
	}
	
	
};


