build-openmp:
	g++ -fopenmp -O0 -march=native openmp.cpp -o openmp -lstdc++
run-openmp:
	./openmp.exe
clean-openmp:
	rm -f openmp.exe

build-dynamic:
	g++ -fopenmp -O0 -march=native openmp-dynamic.cpp -o dynamic -lstdc++
run-dynamic:
	./dynamic.exe
clean-dynamic:
	rm -f dynamic.exe

build-guided:
	g++ -fopenmp -O0 -march=native openmp-guided.cpp -o guided -lstdc++
run-guided:
	./guided.exe	
clean-guided:
	rm -f guided.exe

build-speed:
	g++ -fopenmp -O0 -march=native openmp-speed.cpp -o speed -lstdc++
run-speed:	
	./speed.exe
clean-speed:
	rm -f speed.exe

build-split:
	g++ -fopenmp -O0 -march=native openmp-split.cpp -o split -lstdc++
run-split:
	./split.exe
clean-split:
	rm -f split.exe

build-datafill:
	g++ -fopenmp -O0 -march=native openmp-datafill.cpp -o datafill -lstdc++
run-datafill:
	./datafill.exe
clean-datafill:
	rm -f datafill.exe

build-all: build-openmp build-dynamic build-guided build-split build-datafill
run-all: run-openmp run-dynamic run-guided run-split run-datafill
clean-all: clean-openmp clean-dynamic clean-guided clean-speed clean-split clean-datafill