build-dynamic:
	g++ -pthread -O0 -march=native dynamic-threads.cpp -o dynamic -lstdc++
run-dynamic:
	./dynamic.exe
clean-dynamic:
	del -f dynamic.exe

build-barrier:
	g++ -pthread -O0 -march=native barrier.cpp -o barrier -lstdc++
run-barrier:
	./barrier.exe
clean-barrier:
	del -f barrier.exe

build-sem:
	g++ -pthread -O0 -march=native semaphore.cpp -o semaphore -lstdc++
run-sem:
	./semaphore.exe
clean-sem:
	del -f semaphore.exe

build-tri:
	g++ -pthread -O0 -march=native tri-loop.cpp -o triloop -lstdc++	
run-tri:
	./triloop.exe
clean-tri:
	del -f triloop.exe

build-all: build-dynamic build-barrier build-sem build-tri
run-all: run-dynamic run-barrier run-sem run-tri
clean-all: clean-dynamic clean-barrier clean-sem clean-tri