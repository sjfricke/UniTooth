.PHONY: clean install wipe

PROJECT=unitooth

venv:
	@virtualenv venv
	+@echo "Run '. ./venv/bin/activate' first."
	@exit 1

install: venv
	@pip install -e .

clean:
	@find -name '*.pyc' -delete

wipe: clean
	@rm -rf $(PROJECT).egg-info
	@rm -rf venv
