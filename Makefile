all:
	g++ -g -std=c++11 -O3 -o smtsampler smtsampler.cpp megasampler.cpp -L "/home/batchen/z3/build/" -lz3
