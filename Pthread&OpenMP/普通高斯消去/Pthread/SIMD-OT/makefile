build-Neonbarrier:
	clang++ -g -march=armv8-a -pthread Neon-barrier.cpp -o Neonbarrier.o -lstdc++ -O0
run-Neonbarrier:
	./Neonbarrier.o
clean-Neonbarrier:
	rm -f Neonbarrier.o

build-Neondynamic:
	clang++ -g -march=armv8-a -pthread Neon-dynamic.cpp -o Neondynamic.o -lstdc++ -O0
run-Neondynamic:	
	./Neondynamic.o
clean-Neondynamic:
	rm -f Neondynamic.o

build-Neonsemaphore:
	clang++ -g -march=armv8-a -pthread Neon-semaphore.cpp -o Neonsemaphore.o -lstdc++ -O0
run-Neonsemaphore:
	./Neonsemaphore.o
clean-Neonsemaphore:
	rm -f Neonsemaphore.o

build-Neontriloop:
	clang++ -g -march=armv8-a -pthread Neon-tri-loop.cpp -o Neontriloop.o -lstdc++ -O0
run-Neontriloop:
	./Neontriloop.o
clean-Neontriloop:
	rm -f Neontriloop.o


build-allNeon: build-Neonbarrier build-Neondynamic build-Neonsemaphore build-Neontriloop
run-allNeon: run-Neonbarrier run-Neondynamic run-Neonsemaphore run-Neontriloop
clean-allNeon: clean-Neonbarrier clean-Neondynamic clean-Neonsemaphore clean-Neontriloop







build-SSEbarrier:
	g++ -pthread -O0 -march=native SSE-barrier.cpp -o SSEbarrier -lstdc++
run-SSEbarrier:
	./SSEbarrier.exe
clean-SSEbarrier:
	del -f SSEbarrier.exe

build-SSEdynamic:
	g++ -pthread -O0 -march=native SSE-dynamic.cpp -o SSEdynamic -lstdc++
run-SSEdynamic:
	./SSEdynamic.exe
clean-SSEdynamic:
	del -f SSEdynamic.exe

build-SSEsemaphore:
	g++ -pthread -O0 -march=native SSE-semaphore.cpp -o SSEsemaphore -lstdc++
run-SSEsemaphore:
	./SSEsemaphore.exe
clean-SSEsemaphore:
	del -f SSEsemaphore.exe


build-SSEtriloop:
	g++ -pthread -O0 -march=native SSE-tri-loop.cpp -o SSEtriloop -lstdc++
run-SSEtriloop:
	./SSEtriloop.exe
clean-SSEtriloop:
	del -f SSEtriloop.exe


build-allSSE: build-SSEbarrier build-SSEdynamic build-SSEsemaphore build-SSEtriloop
run-allSSE: run-SSEbarrier run-SSEdynamic run-SSEsemaphore run-SSEtriloop
clean-allSSE: clean-SSEbarrier clean-SSEdynamic clean-SSEsemaphore clean-SSEtriloop