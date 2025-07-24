#!/bin/bash
if [ ! -f pyvenv.cfg ]; then
	python3 -m venv .
	pip install pygame numpy
fi
source bin/activate
python src/python/app.py
