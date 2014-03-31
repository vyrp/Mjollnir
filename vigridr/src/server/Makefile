TARGET = server
SRCDIR = .
OBJDIR   = ../../obj/$(TARGET)
BINDIR   = ../../bin
SOURCES  = $(wildcard *.cpp ./gen-cpp/*.cpp)
INCLUDES = $(wildcard *.h gen-cpp/*.h)
OBJECTS  = $(SOURCES:%.cpp=$(OBJDIR)/%.o)

include ../Makefile.inc

