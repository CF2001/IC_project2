
To test exercise 1:

	../src-bin/ex1 <input File> <output File> 
	
	Exemplification: 
		../src-bin/ex1 ../imagesPPM/airplane.ppm outFile.ppm
		
To test exercise 2:

	../src-bin/ex2 <input filename> <output filename> <filter type>
	
	<filter type>: 
		=> 0  (Creates the negative version of an image)
		=> 1  (Creates a mirrored version of an image horizontally)
		=> 2  (Creates a mirrored version of an image vertically)
		=> 3  (Rotates an image by a multiple of 90º)
		=> 4  (Increases (more light) the intensity values of an image)
		=> 5  (Decreases (less light) the intensity values of an image)
	
	Exemplification:
		To test the negative version of an image: 
		../src-bin/ex2 ../imagesPPM/airplane.ppm outputFile.ppm 0
		
To test exercise 3:	

	../src-bin/Golomb <inFileTxt> <outFileTxt> <binary File> <m> 
	
		<inFileTxt> 	=> input text file
		<outFileTxt> 	=> output text file
		<binary file> 	=> .out file 
		<m> 		=> integer Golomb parameter
		
	Exemplification:
		../src-bin/Golomb encodeInt.txt decodeInt.txt binGolomb.out 8
		

To test exercise 4:
	
		../src-bin/audioCodec <wavfileIn> <wavfileOut> <binary file> <typeAudioCodec> <nPredictor>
		
		<wavfileIn> 		=> input wav file
		<wavfileOut> 		=> output wav file
		<binary file>		=> .out file
		<typeAudioCodec>	=> 0 (lossless) | 1 (lossy)
		<nPredictor>		=> 1 | 2 | 3
		
		Exemplification:
			../src-bin/audioCodec ../audioFiles/sample03.wav audioOut.wav compress.out 1 2
