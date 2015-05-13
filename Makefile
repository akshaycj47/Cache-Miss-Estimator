CXXFLAGS := -g -Wall -std=c++0x -lm
CXX=c++

all: cachesim
#cachesim_prediction.o
cachesim: cachesim.o cachesim_driver.o
	$(CXX) -o cachesim cachesim.o cachesim_driver.o 

clean:
	rm -f cachesim *.o
