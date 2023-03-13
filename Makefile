OUTFILE = beaglepod
OUTDIR = $(HOME)/cmpt433/public/myApps

PUBDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Wshadow 
# -Werror
LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB

all: beaglepod
#$(OUTDIR)/lcd_display.o $(OUTDIR)/gpio.o
beaglepod: beaglepod.c sleep.o audioMixer.o
	$(CC_C) $(CFLAGS) beaglepod.c $(OUTDIR)/audioMixer.o $(OUTDIR)/sleep.o -o $(OUTDIR)/$(OUTFILE) $(LFLAGS) -lpthread -lasound
	mkdir -p $(PUBDIR)/wave-files/
	cp wave-files/* $(PUBDIR)/wave-files/ 

# lcd_display.o: lcd_display.c
# 	$(CC_C) $(CFLAGS) -c lcd_display.c -o $(OUTDIR)/lcd_display.o

# gpio.o: gpio.c
# 	$(CC_C) $(CFLAGS) -c gpio.c -o $(OUTDIR)/gpio.o

sleep.o: sleep.c
	$(CC_C) $(CFLAGS) -c sleep.c -o $(OUTDIR)/sleep.o

# joystick.o: joystick.c
# 	$(CC_C) $(CFLAGS) -c joystick.c -o $(OUTDIR)/joystick.o

audioMixer.o: audioMixer.c
	$(CC_C) $(CFLAGS) -c audioMixer.c -o $(OUTDIR)/audioMixer.o -lpthread 

	
clean:
	rm $(OUTDIR)/$(OUTFILE) $(OUTDIR)/*.o


