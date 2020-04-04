ROOT=../bcl

CPPFLAGS = -I$(ROOT)/include -I.
CFLAGS   = -Wall -Wextra -Werror -pedantic -std=c99 
LDFLAGS  = -L$(ROOT)/lib 
LDLIBS   = -lbcl -lm  

VIEWER = pvisu
DATA   = ../data

BIN=\
	make-se\
	labeling\
	labeling-color\
	dilation\
	erosion\
	opening\
	closing\
	add\
	subtract\
	binarization\
	extract-gear\
	run

.PHONY: all
all: $(OBJ) $(BIN)

morphology.o: se.o
make-se: se.o
dilation: morphology.o se.o
erosion: morphology.o se.o
opening: morphology.o se.o
closing: morphology.o se.o

.PHONY: extract-gear
extract-gear:
	./opening 2 10 $(DATA)/gear.ppm white_circle.ppm
	./subtract $(DATA)/gear.ppm white_circle.ppm gear.ppm
	./binarization 200 255 gear.ppm gear-final.ppm
	$(VIEWER) gear-final.ppm

.PHONY: morphology-mrg morphology-lex morphology-bit
morphology-mrg: se.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o morphology.o $@.c
	make test-color
morphology-lex: se.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o morphology.o $@.c
	make test-color
morphology-bit: se.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o morphology.o $@.c
	make test-color

.PHONY: test-color
test-color: all
	./dilation 2 4 $(DATA)/mm-color.ppm dilation-color.ppm; #$(VIEWER) dilation-color.ppm
	./erosion 2 4 $(DATA)/mm-color.ppm erosion-color.ppm; #$(VIEWER) erosion-color.ppm

.PHONY: clean cleanall
clean:
	$(RM) *.o *.ppm
cleanall: clean
	$(RM) $(BIN)

