#!/usr/bin/env bash

set -e

c++ -O3 -o card card.cpp; 

./card > aek.ppm; 

pnmtopng aek.ppm > aek.png
