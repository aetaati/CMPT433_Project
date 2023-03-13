OUTFILE = beaglepod
OUTDIR = $(HOME)/cmpt433/public/myApps
SOURCE = source-files/*.c

PUBDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB


all: 
	$(CC_C) $(CFLAGS) $(SOURCE) -o $(OUTDIR)/$(OUTFILE) $(LFLAGS) -lbluetooth -lasound -pthread
	
clean:
	rm $(OUTDIR)/$(OUTFILE)


