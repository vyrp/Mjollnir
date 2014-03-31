TARGET = client
SRCDIR = .
OBJDIR   = ../../obj/$(TARGET)
BINDIR   = ../../bin
#UTILS = ../utils/teste.cpp
#UTILSINC = ../utils/teste.h
SOURCES  := $(wildcard *.cpp ../server/gen-cpp/*.cpp) $(UTILS)
INCLUDES := $(wildcard *.h ../server/gen-cpp/*.h) $(UTILSINC)
OBJECTS  := $(SOURCES:%.cpp=$(OBJDIR)/%.o)

include ../Makefile.inc

