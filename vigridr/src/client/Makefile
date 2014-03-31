TARGET = client
SRCDIR = .
OBJDIR   = ../../obj/$(TARGET)
BINDIR   = ../../bin
SOURCES  := $(wildcard *.cpp ../server/gen-cpp/*.cpp)
INCLUDES := $(wildcard *.h ../server/gen-cpp/*.h)
OBJECTS  := $(SOURCES:%.cpp=$(OBJDIR)/%.o)

include ../Makefile.inc

