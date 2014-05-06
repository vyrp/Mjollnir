THRIFTS =  $(shell find . -type f -name *.thrift)
THRIFTSGENCPP = $(shell echo $(THRIFTS) | sed s,/[^/]*\.thrift,/gen-cpp,g | sed s,\\s,\\n,g | sort -u)

all: $(THRIFTSGENCPP) client server buildtests

$(THRIFTSGENCPP): ../obj/thrift
	rm -rf $(THRIFTSGENCPP)
	$(foreach file, $(THRIFTS), thrift --gen cpp:pure_enums -o $(shell dirname $(file)) $(file);)
	$(foreach file, $(THRIFTS), thrift --gen py -o $(shell dirname $(file)) $(file);)
	$(foreach folder, $(THRIFTSGENCPP), rm -f $(folder)/*.skeleton.cpp; mkdir -p ../obj/$(folder);)

../obj/thrift: $(THRIFTS)
	touch ../obj/thrift

.PHONY: client
client:
	@$(MAKE) $(MFLAGS) -C client
	
.PHONY: server
server:
	@$(MAKE) $(MFLAGS) -C server

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

.PHONY: remove	
remove:
	$(MAKE) $(MFLAGS) remove -C client
	$(MAKE) $(MFLAGS) remove -C server

