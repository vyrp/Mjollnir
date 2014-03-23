TARGET = server
SRCDIR = .
OBJDIR   = ../../obj/$(TARGET)
BINDIR   = ../../bin
SOURCES  = $(wildcard $(SRCDIR)/*.cpp ./gen-cpp/*.cpp)
INCLUDES = $(wildcard $(SRCDIR)/*.h ./gen-cpp/*.h)
OBJECTS  = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

include ../Makefile.inc

