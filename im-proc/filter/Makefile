ROOT=../bcl

CPPFLAGS = -I. -I$(ROOT)/include 
CFLAGS   = -Wall -Wextra  -pedantic -std=c99 
LDFLAGS  = -L$(ROOT)/lib
LDLIBS   = -lm -lfftw3 -lbcl

BIN=heat-equation\
	anisotropic-diffusion\
	median\
	bilateral\
	nlmeans\
	butterworth

.PHONY=all
all: $(BIN)

butterworth: fft.o

.PHONY: clean cleanall
clean:
	$(RM) *.o *.ppm
cleanall: clean
	$(RM) $(BIN)


