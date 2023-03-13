/*
Author: Amirhossein Etaati, Mehdi Esmaeilzadeh
Date: 2023-02-25
Subject: Implementation of the inputManager module to get the current accelerometer action
*/



#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#include "inputManager.h"
#include "audioMixer.h"
#include "joystick.h"
#include "accelerometer.h"
#include "shutdown.h"
#include "periodTimer.h"


static void playSingleSound(char * soundPath);

////////////////////////// Accelerometer and Joystick Input
static bool stoppingInput = false;
static pthread_t inputManagerThreadId;

///////////////////////// Sounds and Songs
static bool stoppingSong = true; // default is true for song thread to check if no playing thread is active - set to false as soon as a player thread is created
#define TEMPO_DEFAULT 120
static int currentTempo = TEMPO_DEFAULT;

static enum eCurrentSong currentSongPlaying = NO_SONG;
// Handle Rock Sounds
static enum eCurrentRockSound currentRockSoundPlaying = HI_HAT_BASE;
// Handle Make Up Sounds
static enum eCurrentMadeUpSound currentMadeUpSoundPlaying = CYN_HARD;
// Gets the waiting time for a half beat in milliseconds
#define GET_BEAT_TIME(BPM) ((120 * 1000) / (BPM))
static pthread_t songPlayerThreadId;
static pthread_mutex_t tempoMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t volumeMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t currentModeMutex = PTHREAD_MUTEX_INITIALIZER;

/********************** Private/Helper functions**********************/

////////////////////////// Common Helpers

// Put thread to sleep for 'delayInMs' milliseconds
// static void sleepForMs(long long delayInMs) 
// {
//   const long long NS_PER_MS = 1000 * 1000;
//   const long long NS_PER_SECOND = 1000000000;
//   long long delayNs = delayInMs * NS_PER_MS;
//   int seconds = delayNs / NS_PER_SECOND;
//   int nanoseconds = delayNs % NS_PER_SECOND;
//   struct timespec reqDelay = {
//     seconds,
//     nanoseconds
//   };
//   nanosleep( & reqDelay, (struct timespec * ) NULL);
// }

////////////////////////// Accelerometer and Joystick Input

// Sets the timer for action related to idx
// Done after the action gets triggered
static void setTimers(long long * timers, int idx, int wait_time)
{
  if (timers[idx] == 0) {
    timers[idx] = wait_time;
  }
}

// Reduces wait time for all waiting actions
// Done at end of each loop
static void decrementTimers(long long * timers, int size) 
{
  for (size_t i = 0; i < size; i++) {
    if (timers[i] > 0) {
      timers[i] -= INPUT_CHECK_WAIT_TIME;
    }
  }
}

// Checks to see if the action can be triggered or should wait.
// Used for debouncing
static bool isActionTriggered(long long * timers, int idx) 
{
  if (timers[idx] == 0) {
    return true;
  }
  return false;
}

static void triggerJoystickAction(enum eJoystickDirections currentJoyStickDirection) 
{
  enum eCurrentSong localCurrentSongPlaying = NO_SONG;
  pthread_mutex_lock(&currentModeMutex);
  localCurrentSongPlaying = currentSongPlaying;
  pthread_mutex_unlock(&currentModeMutex);

  if (currentJoyStickDirection == JOYSTICK_UP) {
    InputManager_UpdateVolume(VOLUME_CHANGE_SIZE, true);

  } else if (currentJoyStickDirection == JOYSTICK_DOWN) {
    InputManager_UpdateVolume(VOLUME_CHANGE_SIZE, false);

  } else if (currentJoyStickDirection == JOYSTICK_LEFT) {
    InputManager_UpdateTempo(TEMPO_CHANGE_SIZE, false);

  } else if (currentJoyStickDirection == JOYSTICK_RIGHT) {
    InputManager_UpdateTempo(TEMPO_CHANGE_SIZE, true);
  } else if (currentJoyStickDirection == JOYSTICK_CENTER) {
    if(localCurrentSongPlaying == NO_SONG) {
      InputManager_StopSong();
      InputManager_StartSong(STANDARD_ROCK_SONG);
    }
    else if(localCurrentSongPlaying == STANDARD_ROCK_SONG) {
      InputManager_StopSong();
      InputManager_StartSong(MADE_UP_SONG);
    }
    else if (localCurrentSongPlaying == MADE_UP_SONG) {
      InputManager_StopSong();
    }
  }
  
}

static void triggerAccelerometerAction(enum eAccelerometerActions accelerometerAction) 
{
  if (accelerometerAction == ACCELEROMETER_X) {
    playSingleSound(SNARE_FILE);
  }
  if (accelerometerAction == ACCELEROMETER_Y) {
    playSingleSound(HI_HAT_FILE);
  }
  if (accelerometerAction == ACCELEROMETER_Z) {
    playSingleSound(BASE_DRUM_FILE);
  }
}

static void * inputManagerThread(void * arg) 
{
  // int timer_size = JOYSTICK_MAX_NUMBER_DIRECTIONS + ACCELEROMETER_MAX_NUMBER_ACTIONS;
  // long long action_timers[timer_size];
  // for (size_t i = 0; i < timer_size; i++) {
  //   action_timers[i] = (long long) 0;
  // }

  while (!stoppingInput && !Shutdown_isShutdown()) {
    // Read the Joystick and Accelerometer
    enum eJoystickDirections currentJoyStickDirection = Joystick_process_direction();
    enum eAccelerometerActions accelerometerAction = Accelerometer_GetCurrentAction();

    // Debouncing logic: Joystick
    int accelerometer_idx = JOYSTICK_MAX_NUMBER_DIRECTIONS + accelerometerAction;
    // Trigger action
    if (isActionTriggered(action_timers, currentJoyStickDirection)) {
      triggerJoystickAction(currentJoyStickDirection);

      // Update current direction time
      setTimers(action_timers, currentJoyStickDirection, DEBOUNCE_WAIT_TIME);
    }

    // Debouncing logic: Accelerometer
    if (isActionTriggered(action_timers, accelerometer_idx)) {
      triggerAccelerometerAction(accelerometerAction);

      // Update current direction time
      setTimers(action_timers, accelerometer_idx, DEBOUNCE_WAIT_TIME);
    }

    // Adjust timers
    decrementTimers(action_timers, timer_size);

    sleepForMs(INPUT_CHECK_WAIT_TIME);
  }

  return NULL;
}


///////////////////////// Sounds and Songs

// Queues a single sound corresponding to soundPath
static void playSingleSound(char * soundPath) 
{
  wavedata_t * currentSound = malloc(sizeof( * currentSound)); 
  AudioMixer_readWaveFileIntoMemory(soundPath, currentSound);
  AudioMixer_queueSound(currentSound);
}

// Plays the next sound for standard rock
static void playStandardRockSong(enum eCurrentRockSound currentSound) 
{
  if(currentSound == HI_HAT_BASE) {
    wavedata_t *hiHatSound = malloc(sizeof(* hiHatSound));
    AudioMixer_readWaveFileIntoMemory(HI_HAT_FILE, hiHatSound);
    AudioMixer_queueSound(hiHatSound);

    wavedata_t *baseSound = malloc(sizeof(* baseSound));
    AudioMixer_readWaveFileIntoMemory(BASE_DRUM_FILE, baseSound);
    AudioMixer_queueSound(baseSound);
    
    currentRockSoundPlaying = HI_HAT_ONE;
  }
  else if(currentSound == HI_HAT_ONE) {
    wavedata_t *hiHatSound = malloc(sizeof(* hiHatSound));
    AudioMixer_readWaveFileIntoMemory(HI_HAT_FILE, hiHatSound);
    AudioMixer_queueSound(hiHatSound);
    currentRockSoundPlaying = HI_HAT_SNARE;
  }
  else if(currentSound == HI_HAT_SNARE) {
    wavedata_t *hiHatSound = malloc(sizeof(* hiHatSound));
    AudioMixer_readWaveFileIntoMemory(HI_HAT_FILE, hiHatSound);
    AudioMixer_queueSound(hiHatSound);

    wavedata_t *snareSound = malloc(sizeof(* snareSound));
    AudioMixer_readWaveFileIntoMemory(SNARE_FILE, snareSound);
    AudioMixer_queueSound(snareSound);
    
    currentRockSoundPlaying = HI_HAT_TWO;
  }
  else if(currentSound == HI_HAT_TWO) {
    wavedata_t *hiHatSound = malloc(sizeof(* hiHatSound));
    AudioMixer_readWaveFileIntoMemory(HI_HAT_FILE, hiHatSound);
    AudioMixer_queueSound(hiHatSound);
    currentRockSoundPlaying = HI_HAT_BASE;
  }
}

// Plays the next sound for make up
static void playMadeUpSong(enum eCurrentMadeUpSound currentSound) 
{
  if(currentSound == CYN_HARD) {
    wavedata_t *cynSound = malloc(sizeof(* cynSound));
    AudioMixer_readWaveFileIntoMemory(CYN_HARD_FILE, cynSound);
    AudioMixer_queueSound(cynSound);
    
    currentMadeUpSoundPlaying = TOM_LO;
  }
  else if(currentSound == TOM_LO) {
    wavedata_t *tomLoSound = malloc(sizeof(* tomLoSound));
    AudioMixer_readWaveFileIntoMemory(TOM_LO_FILE, tomLoSound);
    AudioMixer_queueSound(tomLoSound);
    currentMadeUpSoundPlaying = CO;
  }
  else if (currentSound == CO){
    wavedata_t *coSound = malloc(sizeof(* coSound));
    AudioMixer_readWaveFileIntoMemory(CO_FILE, coSound);
    AudioMixer_queueSound(coSound);
    currentMadeUpSoundPlaying = CYN_HARD;
  }
}

// Thread function to continuously play song passed in as argument
static void *songPlayerThread(void * args) 
{
  enum eCurrentSong *mode = (enum eCurrentSong *) args;
  while(!stoppingSong && !Shutdown_isShutdown()) {
    if(*mode == STANDARD_ROCK_SONG) {
      playStandardRockSong(currentRockSoundPlaying);
    }
    else if(*mode == MADE_UP_SONG) {
      playMadeUpSong(currentMadeUpSoundPlaying);
    }

    sleepForMs(GET_BEAT_TIME(currentTempo));
  }

  free(args);
  return NULL;
}

/********************** Public/Module functions**********************/

void InputManager_init(void) 
{
  // Joystick
  Joystick_init();
  // Accelerometer
  Accelerometer_init();
  // Wait for 330ms for the GPIOs to configure before user
  sleepForMs(330);

  // AudioMixer
  AudioMixer_init();
}

void InputManager_HandleInputs(void) 
{
  // Launch input manager thread
  pthread_create( & inputManagerThreadId, NULL, inputManagerThread, NULL);
}

void InputManager_PlaySingleSound(char *soundPath) 
{
  playSingleSound(soundPath);
}

// Starts a new thread to play the song corresponding to mode
void InputManager_StartSong(enum eCurrentSong mode) 
{
  pthread_mutex_lock(&currentModeMutex);
    // only start a new thread when no thread is active
  if (stoppingSong == true) {
    stoppingSong = false;
    currentSongPlaying = mode;
    // need to allocate thread argument - to be freed from inside the thread
    enum eCurrentSong *modeArg = malloc(sizeof(enum eCurrentSong));
    *modeArg = mode;
    pthread_create(&songPlayerThreadId, NULL, songPlayerThread, (void *) modeArg);
  }
  pthread_mutex_unlock(&currentModeMutex);
}

// Stops the song plater thread
void InputManager_StopSong(void) 
{
  pthread_mutex_lock(&currentModeMutex);
  stoppingSong = true;
  if (currentSongPlaying != NO_SONG) {
    pthread_join(songPlayerThreadId, NULL);
    currentSongPlaying = NO_SONG;
  }
  pthread_mutex_unlock(&currentModeMutex);
}

void InputManager_cleanup(void) 
{
  stoppingInput = true;

  InputManager_StopSong();
  pthread_join(inputManagerThreadId, NULL);

  // Joystick
  Joystick_cleanup();
  // Accelerometer
  Accelerometer_cleanup();
  // AudioMixer
  AudioMixer_cleanup();
}

void InputManager_UpdateTempo(int changeSize, bool isIncrease)
{
  if (changeSize < 0){
    return;
  }
  pthread_mutex_lock(&tempoMutex);
  if(!isIncrease){
    // decrease tempo
    if( (currentTempo - changeSize) < MIN_TEMPO) {
      currentTempo = MIN_TEMPO;
    }
    else {
      currentTempo -= changeSize;
    }
  }
  else {
    // increase tempo
    if( (currentTempo + changeSize) > MAX_TEMPO) {
      currentTempo = MAX_TEMPO;
    }
    else {
      currentTempo += changeSize;
    }
  }
  pthread_mutex_unlock(&tempoMutex);
}

void InputManager_UpdateVolume(int changeSize, bool isIncrease) 
{
  if (changeSize < 0){
    return;
  }
  pthread_mutex_lock(&volumeMutex);
  int current_volume = AudioMixer_getVolume();
  int new_volume = current_volume;
  if(!isIncrease){
    // decrease volume
    if( (current_volume - changeSize) < MIN_VOLUME) {
      new_volume = MIN_VOLUME;
    }
    else {
      new_volume -= changeSize;
    }
  }
  else {
    // increase volume
    if( (current_volume + changeSize) > MAX_VOLUME) {
      new_volume = MAX_VOLUME;
    }
    else {
      new_volume += changeSize;
    }
  }
  AudioMixer_setVolume(new_volume);
  pthread_mutex_unlock(&volumeMutex);
}

int InputManager_GetCurrentTempo(void)
{
  int tempo = -1;
  pthread_mutex_lock(&tempoMutex);
  tempo = currentTempo;
  pthread_mutex_unlock(&tempoMutex);
  return tempo;
}

int InputManager_GetCurrentVolume(void)
{
  int volume = -1;
  pthread_mutex_lock(&volumeMutex);
  volume = AudioMixer_getVolume();
  pthread_mutex_unlock(&volumeMutex);
  return volume;
}

int InputManager_GetCurrentMode(void)
{
  int currentMode = -1;
  pthread_mutex_lock(&currentModeMutex);
  currentMode = (int) currentSongPlaying;
  pthread_mutex_unlock(&currentModeMutex);

  return currentMode;
}

/////////////////////////////////////////////////////// For Testing ///////////////////////////////////////////////////////
// int main(int argc, char
//   const * argv[]) 
//   {
//   InputManager_init();

//   InputManager_HandleInputs();

//   while (1) {

//   }

//   InputManager_cleanup();

//   return 0;
// }