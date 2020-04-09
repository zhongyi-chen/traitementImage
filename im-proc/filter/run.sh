#!/bin/sh
# ./heat-equation 10 ../data/barbara.ppm heat-10.ppm
# ./heat-equation 50 ../data/barbara.ppm heat-50.ppm
# ./heat-equation 100 ../data/barbara.ppm heat-100.ppm
./anisotropic-diffusion 100 10 0 ../data/barbara.ppm anisotropic-100-10-0.ppm
./anisotropic-diffusion 100 10 1 ../data/barbara.ppm anisotropic-100-10-1.ppm
./anisotropic-diffusion 100 10 2 ../data/barbara.ppm anisotropic-100-10-2.ppm
pvisu *.ppm