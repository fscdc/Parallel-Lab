all:test1,test2,testunroll,clean
test1:
	g++ -O0 1-combine-common-cache.cpp -o 1
	./1.exe
test2:
	g++ -O0 2-conbine-common-recursion.cpp -o 2
	./2.exe
testunroll:
	g++ -O0 unroll-1.cpp -o unroll
	./unroll.exe
clean:
	rm -rf 1.exe 2.exe unroll.exe