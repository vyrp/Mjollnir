# ------------------------------------------------
# Generic Makefile
#
# Author: yanick.rochon@gmail.com
# Modified: luizmramos@gmail.com
# Date  : 2011-08-10
#
# Changelog :
#   2010-11-05 - first version
#   2011-08-10 - added structure : sources, objects, binaries
#                thanks to http://stackoverflow.com/users/128940/beta
# ------------------------------------------------

# directories
SRCDIR = .
OBJDIR   = ../../obj/$(TARGET)
BINDIR   = ../../bin

# objects
OBJECTS  = $(SOURCES:%.cpp=$(OBJDIR)/%.o)

# project name (generate executable with this name)
BINARY   = $(TARGET)

CC       = g++
# compiling flags here
CFLAGS   = -std=c++0x -Wall -Werror -I/usr/local/include/thrift -I/usr/local/include/gflags

LINKER   = g++ -o
# linking flags here
LFLAGS   = -Wall -L/usr/local/lib
LIBRARIES = -lpthread -lthrift -lgflags

rm       = rm -f

all: $(BINDIR)/$(BINARY)
	
$(BINDIR)/$(BINARY): $(OBJECTS)
	$(LINKER) $@ $(LFLAGS) $(OBJECTS) $(LIBRARIES)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(INCLUDES:%.h=%.cpp): $(SRCDIR)/%.cpp : $(SRCDIR)/%.h
	@touch -c $@

$(INCLUDES):

.PHONY: clean
clean:
	$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	$(rm) $(BINDIR)/$(BINARY)
	@echo "Executable removed!"

