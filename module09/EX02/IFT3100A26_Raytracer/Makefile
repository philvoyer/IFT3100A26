TARGET = raytracer

CC = g++               # gcc c++ compiler
CV = -std=c++11        # c++ version
CD = -g                # generate debug information
CW = -Wall -Wextra     # compiler extra warnings
CP = -pedantic         # ISO C/C++ compliance
CN = -Wc++11-narrowing # warnings about narrowing
CO = -O2               # level of optimization
CT = -pthread          # threads (std::thread)
RF = rm -f             # force remove
RR = rm -rf            # force remove recursive

SOURCE = raytracer.cpp

default: release

release:
	$(CC) $(CV) $(CO) $(CT) -o $(TARGET) $(SOURCE)

debug:
	$(CC) $(CV) $(CD) $(CN) $(CW) $(CP) $(CT) -o $(TARGET) $(SOURCE)

clean:
	$(RF) $(TARGET)
	$(RR) $(TARGET).dSYM
