#!/bin/sh
./heat-equation 10 ../data/barbara.ppm heat-10.ppm
./heat-equation 50 ../data/barbara.ppm heat-50.ppm
./heat-equation 100 ../data/barbara.ppm heat-100.ppm
pvisu *.ppm