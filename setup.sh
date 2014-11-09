#!/bin/sh
rm -rf nutshell
virtualenv nutshell
source env.sh
pip install numpy codepy
cd copperhead
python setup.py build
python setup.py install
cd ..
