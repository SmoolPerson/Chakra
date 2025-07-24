#!/bin/bash
if [ ! -f pyvenv.cfg ]; then
	python3 -m venv .
fi
source bin/activate
python src/python/app.py
