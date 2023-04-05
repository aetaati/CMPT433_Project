OUTFILE = beaglepod
OUTDIR = $(HOME)/cmpt433/public/myApps
SOURCE = source-files/*.c

PUBDIR = $(HOME)/cmpt433/public/myApps
CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

LFLAGS = -L$(HOME)/cmpt433/public/pulse-audio_lib_BBB -L$(HOME)/cmpt433/public/asound_lib_BBB 

LD_LIBRARY_PATH=/home/kingsteez/cmpt433/public/pulse-audio_lib_BBB:$(LD_LIBRARY_PATH)


# Deploy this Node.js project to the public folder
PROJECT_NAME=beaglepod-server
DEPLOY_PATH= $(HOME)/cmpt433/public/myApps/$(PROJECT_NAME)-copy

# Deploy web interface
node : deploy node_install react_install all

all: 
	$(CC_C) $(CFLAGS) $(SOURCE) -o $(OUTDIR)/$(OUTFILE) $(LFLAGS) -lbluetooth -lasound -pthread
	mkdir -p $(OUTDIR)/songs/
	cp songs/* $(OUTDIR)/songs/
	
deploy:
	@echo 'COPYING THE NODE.JS FILES TO $(DEPLOY_PATH)'
	@echo ''
	mkdir -p $(DEPLOY_PATH)
	chmod a+rwx $(DEPLOY_PATH)
	cp -r web-server/* $(DEPLOY_PATH)
	@echo 'Do not edit any files in this folder; they are copied!' > $(DEPLOY_PATH)/DO_NOT_EDIT-FILES_COPIED.txt
	@echo ''
	@echo 'NOTE: On the host, in $(DEPLOY_PATH), it is best to run: npm install'
	@echo '      Or, just run the node_install target in this makefile.'

node_install:
	@echo ''
	@echo ''
	@echo 'INSTALLING REQUIRED NODE PACKAGES'
	@echo '(This may take some time)'
	@echo ''
	cd $(DEPLOY_PATH) && npm install

react_install:
	@echo ''
	@echo ''
	@echo 'INSTALLING REQUIRED NODE PACKAGES'
	@echo '(This may take some time)'
	@echo ''
	cd $(DEPLOY_PATH)/client && npm install

	




clean:
	rm $(OUTDIR)/$(OUTFILE)
	rm -r $(OUTDIR)/songs


