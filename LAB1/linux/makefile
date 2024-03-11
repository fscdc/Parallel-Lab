all:test1,test2,testunroll,clean
test1:
	g++ -O0 1-combine-common-cache-linux.cpp -o 1
	./1
test2:
	g++ -O0 2-conbine-common-recursion-linux.cpp -o 2
	./2
testunroll:
	g++ -O0 unroll-1-linux.cpp -o unroll
	./unroll
clean:
	rm -rf 1 2 unroll