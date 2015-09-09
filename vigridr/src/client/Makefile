TARGET = client

UTILS = ../utils/Log.cpp
SOURCES  = $(wildcard *.cpp ../thrifts/gen-cpp/*.cpp) $(UTILS)
INCLUDES = $(wildcard *.h ../thrifts/gen-cpp/*.h) $(UTILSINC)
CSBIN = ../../bin/csharp/$(TARGET)
CSSOURCES  = $(wildcard *.cs ../thrifts/gen-csharp/*.cs)
JAVABIN = ../../bin/java/$(TARGET)
JAVASOURCES  = $(wildcard *.java ../thrifts/gen-java/*.java)
JAVARUNNABLE = java_run_script.sh
PYDIR = ../../bin/py
PYBIN = $(PYDIR)/$(TARGET)
PYTHRIFTS = ../thrifts/gen-py
PYRUNNABLE = python_run_script.sh
PYSOURCES  = $(wildcard *.py)

OTHERBIN = $(CSBIN) $(JAVABIN) $(PYBIN)

all: cpp csharp java py

include ../Makefile.inc

CPPBIN = ../../bin/cpp/$(TARGET)
CLIENTOBJDIR = ../../obj/client

.PHONY: csharp
csharp: $(CSBIN)

.PHONY: java
java: $(JAVABIN)

.PHONY: py
py: $(PYBIN)

$(CSBIN): $(CSSOURCES)
	gmcs -r:../../third-parties/csharp/Thrift.dll $(CSSOURCES) -out:$(CSBIN).exe
	mv $(CSBIN).exe $(CSBIN)
	cp ../../third-parties/csharp/Thrift.dll ../../bin/csharp/

$(JAVABIN): $(JAVASOURCES)
	javac -classpath "../../third-parties/java/*" $(JAVASOURCES) -d $(CLIENTOBJDIR)
	jar cf $(JAVABIN).jar -C $(CLIENTOBJDIR) .
	cp $(JAVARUNNABLE) $(JAVABIN)

$(PYBIN): $(PYSOURCES) $(PYRUNNABLE) $(PYTHRIFTS)
	cp -r $(PYSOURCES) $(PYDIR)
	cp -r $(PYRUNNABLE) $(PYBIN)
	cp -rf $(PYTHRIFTS) $(PYDIR)

.PHONY: delete
delete:
	@echo "Deleting all client binaries"
	rm -f $(OTHERBIN)
	rm -rf $(PYDIR)/*
	rm -f $(CPPBIN)
	rm -rf $(CLIENTOBJDIR)/*
	
