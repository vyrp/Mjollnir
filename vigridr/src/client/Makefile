TARGET = client
#UTILS = ../utils/teste.cpp
#UTILSINC = ../utils/teste.h
SOURCES  = $(wildcard *.cpp ../server/gen-cpp/*.cpp) $(UTILS)
INCLUDES = $(wildcard *.h ../server/gen-cpp/*.h) $(UTILSINC)
CSBIN = ../../bin/csharp/$(TARGET).exe
CSSOURCES  = $(wildcard *.cs ../server/gen-csharp/*.cs)
OTHERBIN = $(CSBIN)


all: cpp csharp

include ../Makefile.inc

.PHONY: csharp
csharp: $(CSBIN)

$(CSBIN): $(CSSOURCES)
	gmcs -r:../../third-parties/csharp/Thrift.dll $(CSSOURCES) -out:$(CSBIN)
	cp ../../third-parties/csharp/Thrift.dll ../../bin/csharp/


