
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

	../src-bin/Golomb <txt file> <binary file> <m>	
	
		<txt file> =>  .txt file
		<binary file> => .out file 
		<m> => integer Golomb parameter
		
	Exemplification: 
		
