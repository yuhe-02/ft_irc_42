#!/bin/bash

VENV_DIR=".venv"
PORT=6667


if ! command -v python &>/dev/null; then
    echo "Error: Python is not installed." >&2
    exit 1
fi

if [ ! -d "$VENV_DIR" ]; then
    python -m venv "$VENV_DIR"
fi

source "$VENV_DIR/bin/activate"
pip install -r requirements.txt
python test.py -p $PORT
deactivate

