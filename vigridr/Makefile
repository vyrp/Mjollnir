ALLDIRECTORIES := $(shell find -type d | grep 'src')
DIRECTORIES := $(ALLDIRECTORIES:./src/%=./obj/%)  ./bin ./bin/cpp ./bin/csharp ./bin/py

all: $(DIRECTORIES)
	@$(MAKE) $(MFLAGS) -C src	

clientcpp: $(DIRECTORIES)
	@$(MAKE) clientcpp $(MFLAGS) -C src

clientcsharp: $(DIRECTORIES)
	@$(MAKE) clientcsharp $(MFLAGS) -C src

clientpy: $(DIRECTORIES)
	@$(MAKE) clientpy $(MFLAGS) -C src
	
server: $(DIRECTORIES)
	@$(MAKE) server $(MFLAGS) -C src

$(DIRECTORIES): 
	@mkdir -p $(DIRECTORIES)
	@echo "Created directories "$(DIRECTORIES)

.PHONY: test
test:
	@$(MAKE) $(MFLAGS) test -C src
	
.PHONY: clean
clean:
	@$(MAKE) $(MFLAGS) clean -C src
.PHONY: remove
remove:
	@$(MAKE) $(MFLAGS) remove -C src	
