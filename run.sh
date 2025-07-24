#!/bin/bash
if [ ! -f pyvenv.cfg ]; then
	python3 -m venv ./venv
	source ./venv/bin/activate
	pip install pygame numpy
fi
source ./venv/bin/activate
python src/python/app.py
