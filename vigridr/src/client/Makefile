TARGET = client

UTILS = ../utils/Log.cpp
SOURCES  = $(wildcard *.cpp ../thrifts/gen-cpp/*.cpp) $(UTILS)
INCLUDES = $(wildcard *.h ../thrifts/gen-cpp/*.h) $(UTILSINC)
CSBIN = ../../bin/csharp/$(TARGET)
CSSOURCES  = $(wildcard *.cs ../thrifts/gen-csharp/*.cs)
PYBIN = ../../bin/py/$(TARGET)
PYDIR = ../../bin/py
PYTHRIFTS = ../thrifts/gen-py
PYRUNNABLE = python_run_script.sh
PYSOURCES  = $(wildcard *.py)

OTHERBIN = $(CSBIN) $(PYBIN)

all: cpp csharp py

include ../Makefile.inc

CPPBIN = ../../bin/cpp/$(TARGET)
CLIENTOBJDIR = ../../obj/client

.PHONY: csharp
csharp: $(CSBIN)

.PHONY: py
py: $(PYBIN)

$(CSBIN): $(CSSOURCES)
	gmcs -r:../../third-parties/csharp/Thrift.dll $(CSSOURCES) -out:$(CSBIN).exe
	mv $(CSBIN).exe $(CSBIN)
	cp ../../third-parties/csharp/Thrift.dll ../../bin/csharp/

$(PYBIN): $(PYSOURCES) $(PYRUNNABLE) $(PYTHRIFTS)
	cp -r $(PYSOURCES) $(PYDIR)
	cp -r $(PYRUNNABLE) $(PYDIR)/client
	cp -rf $(PYTHRIFTS) $(PYDIR)

.PHONY: delete
delete:
	@echo "Deleting all client binaries"
	rm -f $(OTHERBIN)
	rm -rf $(PYDIR)/*
	rm -f $(CPPBIN)
	rm -rf $(CLIENTOBJDIR)/*
	