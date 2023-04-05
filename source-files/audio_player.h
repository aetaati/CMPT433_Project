#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

typedef struct {
	int numSamples;
	short *pData;
} wavedata_t;

#define AUDIO_PLAYER_MAX_VOLUME 100
#define AUDIO_PLAYER_MIN_VOLUME 0
// init() must be called before any other functions,
// cleanup() must be called last to stop playback threads and free memory.
void AudioPlayer_init(void);
void AudioPlayer_cleanup(void);

void AudioPlayer_pause(void);

// Read the contents of a wave file into the pSound structure. Note that
// the pData pointer in this structure will be dynamically allocated in
// readWaveFileIntoMemory(), and is freed by calling freeWaveFileData().
void AudioPlayer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound);
void AudioPlayer_freeWaveFileData(wavedata_t *pSound);

// Queue up another sound bite to play as soon as possible.
void AudioPlayer_playWAV(wavedata_t *pSound);

// Get/set the volume.
// setVolume() function posted by StackOverflow user "trenki" at:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
int  AudioPlayer_getVolume(void);
void AudioPlayer_setVolume(double newVolume);


#endif