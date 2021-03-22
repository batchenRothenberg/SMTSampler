all:
	g++ -g -std=c++11 -O3 -o smtsampler smtsampler.cpp megasampler.cpp sampler.cpp main.cpp -L "/home/batchen/z3/build" -lz3
#	g++ -Wl,--trace -g -std=c++11 -O3 -o smtsampler smtsampler.cpp megasampler.cpp main.cpp -L "/home/batchen/z3/build" -lz3
