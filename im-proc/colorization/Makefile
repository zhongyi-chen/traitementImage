ROOT=../bcl

CPPFLAGS = -I$(ROOT)/include -I. 
CFLAGS   = -Wall -Wextra -Werror -pedantic -std=c99 
LDFLAGS  = -L$(ROOT)/lib 
LDLIBS   = -lbcl -lm -lfftw3 

BIN= colorization run

.PHONY: all
all: $(BIN)


.PHONY: clean cleanall
clean:
	$(RM) *.o *.ppm
cleanall: clean
	$(RM) $(BIN) 