TARGET = server
SRCDIR = .
OBJDIR   = ../../obj/$(TARGET)
BINDIR   = ../../bin
#UTILS = ../utils/teste.cpp
#UTILSINC = ../utils/teste.h
SOURCES  = $(wildcard *.cpp ./gen-cpp/*.cpp) $(UTILS)
INCLUDES = $(wildcard *.h gen-cpp/*.h) $(UTILSINC)
OBJECTS  = $(SOURCES:%.cpp=$(OBJDIR)/%.o)

include ../Makefile.inc

