build-openmp:
	g++ -fopenmp -march=native openmp.cpp -o openmp -lstdc++ -std=c++17
run-openmp:
	./openmp.exe
clean-openmp:
	del -f openmp.exe

build-openmpdynamic:
	g++ -fopenmp -march=native openmp-dynamic.cpp -o openmpdynamic -lstdc++ -std=c++17
run-openmpdynamic:
	./openmpdynamic.exe
clean-openmpdynamic:
	del -f openmpdynamic.exe

build-openmpbitcast:
	g++ -fopenmp -march=native openmp-bitcast.cpp -o openmpbitcast -lstdc++ -std=c++17
run-openmpbitcast:
	./openmpbitcast.exe
clean-openmpbitcast:
	del -f openmpbitcast.exe

build-openmpbitmagic:
	g++ -fopenmp -march=native openmp-bitmagic.cpp -o openmpbitmagic -lstdc++ -std=c++17
run-openmpbitmagic:
	./openmpbitmagic.exe
clean-openmpbitmagic:
	del -f openmpbitmagic.exe

build-all: build-openmp build-openmpdynamic build-openmpbitcast build-openmpbitmagic
run-all: run-openmp run-openmpdynamic run-openmpbitcast run-openmpbitmagic
clean-all: clean-openmp clean-openmpdynamic clean-openmpbitcast clean-openmpbitmagic
