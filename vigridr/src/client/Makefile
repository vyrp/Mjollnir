TARGET = client
SRCDIR = .
OBJDIR   = ../../obj/$(TARGET)
BINDIR   = ../../bin
SOURCES  := $(wildcard $(SRCDIR)/*.cpp ./../server/gen-cpp/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h ./../server/gen-cpp/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

include ../Makefile.inc

