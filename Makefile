OUTFILE = beaglePod
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Wshadow 
# -Werror

all: beaglePod

beaglePod: beaglePod.c lcd_display.o gpio.o sleep.o
	$(CC_C) $(CFLAGS) beaglePod.c $(OUTDIR)/lcd_display.o $(OUTDIR)/gpio.o $(OUTDIR)/sleep.o -o $(OUTDIR)/$(OUTFILE)

lcd_display.o: lcd_display.c
	$(CC_C) $(CFLAGS) -c lcd_display.c -o $(OUTDIR)/lcd_display.o

gpio.o: gpio.c
	$(CC_C) $(CFLAGS) -c gpio.c -o $(OUTDIR)/gpio.o

sleep.o: sleep.c
	$(CC_C) $(CFLAGS) -c sleep.c -o $(OUTDIR)/sleep.o
	
clean:
	rm $(OUTDIR)/$(OUTFILE) $(OUTDIR)/*.o $(OUTDIR)/beaglePod



######################################### for testing #########################################
lcd_display: lcd_display.c gpio.o sleep.o
	$(CC_C) $(CFLAGS) lcd_display.c $(OUTDIR)/gpio.o $(OUTDIR)/sleep.o -o $(OUTDIR)/lcd_display
