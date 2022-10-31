all:
	@(if [ ! -d build ]; then mkdir build; fi; cd build;\
	  if [ ! -d src-bin ]; then mkdir src-bin; fi; cmake ..; make)

clean:
	rm -rf build ../src-bin
