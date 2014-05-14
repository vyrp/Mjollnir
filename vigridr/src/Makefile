THRIFTDIR = thrifts
THRIFTS =  $(shell find $(THRIFTDIR) -type f -name *.thrift)
THRIFTSGENCPP = $(shell echo $(THRIFTS) | sed s,\\s,\\n,g | sed s,/[^/]*\.thrift,/gen-cpp,g | sort -u)

all: $(THRIFTSGENCPP) clientcpp clientcsharp server buildtests

$(THRIFTSGENCPP): ../obj/thrift
	rm -rf $(THRIFTSGENCPP)
	$(foreach file, $(THRIFTS), thrift --gen cpp:pure_enums -o $(shell dirname $(file)) $(file);)
	$(foreach file, $(THRIFTS), thrift --gen py -o $(shell dirname $(file)) $(file);)
	$(foreach file, $(THRIFTS), thrift --gen csharp -o $(shell dirname $(file)) $(file);)
	$(foreach folder, $(THRIFTSGENCPP), rm -f $(folder)/*.skeleton.cpp; mkdir -p ../obj/$(folder);)

../obj/thrift: $(THRIFTS)
	touch ../obj/thrift

.PHONY: clientcpp
clientcpp:
	@$(MAKE) cpp $(MFLAGS) -C client

.PHONY: clientcsharp
clientcsharp:
	@$(MAKE) csharp $(MFLAGS) -C client
	
.PHONY: server
server:
	@$(MAKE) cpp $(MFLAGS) -C server

.PHONY: test
test:
	@$(MAKE) $(MFLAGS) test -C test

.PHONY: buildtests
buildtests:
	@$(MAKE) $(MFLAGS) -C test

.PHONY: clean
clean:
	rm ../obj/thrift
	$(MAKE) $(MFLAGS) clean -C client
	$(MAKE) $(MFLAGS) clean -C server
	$(MAKE) $(MFLAGS) clean -C test

.PHONY: remove	
remove:
	$(MAKE) $(MFLAGS) remove -C client
	$(MAKE) $(MFLAGS) remove -C server
	$(MAKE) $(MFLAGS) remove -C test
	rm -rf $(THRIFTDIR)/gen-*

