ALLDIRECTORIES := $(shell find -type d | grep 'src')
DIRECTORIES := $(ALLDIRECTORIES:./src/%=./obj/%)  ./bin

all: $(DIRECTORIES)
	@$(MAKE) $(MFLAGS) -C src	
	
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
