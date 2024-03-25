# 指定源文件列表
SOURCES := $(wildcard *.cpp)

# 从源文件列表中派生出目标可执行文件列表
EXECUTABLES := $(SOURCES:.cpp=.exe)

# 编译所有.cpp文件到.exe
all: $(EXECUTABLES)

# 运行所有.exe文件
run: 
	./avx-512.exe
	./avx.exe
	./sse-alighed.exe
	./sse.exe
	./serial-cache.exe
	./serial.exe

# 通用规则来从每个.cpp文件创建一个可执行文件
%.exe: %.cpp
	g++ -O0 -march=native $< -o $@

# 清除所有编译的可执行文件和输出文件
clean:
	del /Q $(EXECUTABLES)

.PHONY: all clean run
