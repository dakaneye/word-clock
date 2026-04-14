FQBN ?= arduino:avr:mega
PORT ?= $(shell ls /dev/cu.usbmodem* 2>/dev/null | head -n1)
SKETCH := word-clock

.PHONY: build upload set-time test clean help

help:
	@echo "Targets:"
	@echo "  build     - compile the sketch"
	@echo "  upload    - compile and upload to the Arduino"
	@echo "  set-time  - upload and sync the RTC to the current system time"
	@echo "  test      - run native tests"
	@echo "  clean     - remove build artifacts"
	@echo ""
	@echo "Variables (override with VAR=value):"
	@echo "  FQBN=$(FQBN)"
	@echo "  PORT=$(PORT)"

build:
	arduino-cli compile --fqbn $(FQBN) $(SKETCH)

upload: build
	@test -n "$(PORT)" || { echo "ERROR: no Arduino found on /dev/cu.usbmodem*"; exit 1; }
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) $(SKETCH)

set-time: upload
	@test -n "$(PORT)" || { echo "ERROR: no Arduino found"; exit 1; }
	@scripts/set_time.sh "$(PORT)"

test:
	$(MAKE) -C test test

clean:
	rm -rf $(SKETCH)/build
	$(MAKE) -C test clean
