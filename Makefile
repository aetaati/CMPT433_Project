OUTFILE = beaglepod
OUTDIR = $(HOME)/cmpt433/public/myApps
SOURCE = source-files/*.c

PUBDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

LFLAGS = -L$(HOME)/cmpt433/public/pulse-audio_lib_BBB -L$(HOME)/cmpt433/public/asound_lib_BBB 

LD_LIBRARY_PATH=/home/kingsteez/cmpt433/public/pulse-audio_lib_BBB:$(LD_LIBRARY_PATH)



all: 
	$(CC_C) $(CFLAGS) $(SOURCE) -o $(OUTDIR)/$(OUTFILE) $(LFLAGS) -lbluetooth -lasound -pthread
	mkdir -p $(OUTDIR)/songs/
	cp songs/* $(OUTDIR)/songs/
	
clean:
	rm $(OUTDIR)/$(OUTFILE)
	rm -r $(OUTDIR)/songs


