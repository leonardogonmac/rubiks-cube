.PHONY: main pdb corner-copies samples

main:
	mkdir -p bin
	g++ ./*.cpp -o bin/bin

pdb:
	g++ cube.cpp pdb_api.cpp corner.cpp pdbs/pdb_builder.cpp -o pdbs/bin

MEMORY_GB ?= 200

run_sample:
	LIMIT_BYTES=$$(( $(MEMORY_GB) * 1024 * 1024 * 1024 )); \
	if /usr/bin/time -f "Elapsed: %E\nCPU: %P\nMax memory: %M KB" \
		prlimit --as=$$LIMIT_BYTES ./bin/bin $(THREAD_NUM) < $(FILE) > /dev/null ; \
	then \
		echo "OK: $(FILE)"; \
	else \
		echo "Skipped/failed due to memory limit or runtime error: $(FILE)"; \
	fi; \


tests_range:
	mkdir -p outs/t$(THREAD_NUM)/$(DIST)
	for i in $$(seq -w $(FROM) 10); do \
		s=samples/$(DIST)/$$i; \
		out=$${s##*/}; \
		echo "=============================="; \
		echo "Running: $$s"; \
		LIMIT_BYTES=$$(( $(MEMORY_GB) * 1024 * 1024 * 1024 )); \
		if /usr/bin/time -f "Elapsed: %E\nCPU: %P\nMax memory: %M KB" \
			prlimit --as=$$LIMIT_BYTES ./bin/bin $(THREAD_NUM) < $$s > "outs/t$(THREAD_NUM)/$(DIST)/$$out"; \
		then \
			echo "OK: $$s"; \
		else \
			echo "Skipped/failed due to memory limit or runtime error: $$s"; \
		fi; \
	done

tests:
	mkdir -p outs/t$(THREAD_NUM)/$(DIST)
	for s in samples/$(DIST)/*; do \
		out=$${s##*/}; \
		echo "=============================="; \
		echo "Running: $$s"; \
		LIMIT_BYTES=$$(( $(MEMORY_GB) * 1024 * 1024 * 1024 )); \
		if /usr/bin/time -f "Elapsed: %E\nCPU: %P\nMax memory: %M KB" \
			prlimit --as=$$LIMIT_BYTES ./bin/bin $(THREAD_NUM) < $$s > "outs/t$(THREAD_NUM)/$(DIST)/$$out"; \
		then \
			echo "OK: $$s"; \
		else \
			echo "Skipped/failed due to memory limit or runtime error: $$s"; \
		fi; \
	done

samples:
	mkdir -p samples/$(DIST)
	for i in $$(seq -w 1 9); do \
		echo "$(DIST)" | ./bin/bin > samples/$(DIST)/0$$i; \
	done
	echo "$(DIST)" | ./bin/bin > samples/$(DIST)/10

all_tests:
	mkdir -p tests_report/t$(THREAD_NUM)
	mkdir -p outs/t$(THREAD_NUM)
	for i in $$(seq 3 13); do \
		make tests DIST=$$i THREAD_NUM=$(THREAD_NUM) > tests_report/t$(THREAD_NUM)/$$i 2>&1; \
	done

tests_14:
	mkdir -p outs/t$(THREAD_NUM)/14
	for s in 4 7 9; do \
		echo "=============================="; \
		echo "Running: $$s"; \
		LIMIT_BYTES=$$(( $(MEMORY_GB) * 1024 * 1024 * 1024 )); \
		if /usr/bin/time -f "Elapsed: %E\nCPU: %P\nMax memory: %M KB" \
			prlimit --as=$$LIMIT_BYTES ./bin/bin $(THREAD_NUM) < samples/14/0$$s > "outs/t$(THREAD_NUM)/14/0$$s"; \
		then \
			echo "OK: $$s"; \
		else \
			echo "Skipped/failed due to memory limit or runtime error: $$s"; \
		fi; \
	done
