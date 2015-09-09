ALLDIRECTORIES := $(shell find -type d | grep 'src')
DIRECTORIES := $(ALLDIRECTORIES:./src/%=./obj/%)  ./bin ./bin/cpp ./bin/csharp ./bin/java ./bin/py

all: $(DIRECTORIES)
	@$(MAKE) $(MFLAGS) -C src

clientcpp: $(DIRECTORIES)
	@$(MAKE) clientcpp $(MFLAGS) -C src

clientcsharp: $(DIRECTORIES)
	@$(MAKE) clientcsharp $(MFLAGS) -C src

clientjava: $(DIRECTORIES)
	@$(MAKE) clientjava $(MFLAGS) -C src

clientpy: $(DIRECTORIES)
	@$(MAKE) clientpy $(MFLAGS) -C src
	
server: $(DIRECTORIES)
	@$(MAKE) server $(MFLAGS) -C src

$(DIRECTORIES):
	@echo "Creating directory $@"
	@mkdir -p $@

.PHONY: directories
directories: $(DIRECTORIES)

.PHONY: test
test:
	@$(MAKE) $(MFLAGS) test -C src
	
.PHONY: clean
clean:
	@$(MAKE) $(MFLAGS) clean -C src
.PHONY: remove
remove:
	@$(MAKE) $(MFLAGS) remove -C src

.PHONY: deleteclients
deleteclients:
	@$(MAKE) $(MFLAGS) deleteclients -C src

