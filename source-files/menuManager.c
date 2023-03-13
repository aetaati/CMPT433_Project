/*
Author: Amirhossein Etaati, Mehdi Esmaeilzadeh
Date: 2023-02-25
Subject: Implementation of the MenuManager module to get the current accelerometer action
*/

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#include "menuManager.h"
#include "../audioMixer.h"
#include "joystick.h"
#include "lcd_display.h"
#include "gpio.h"
#include "bluetooth.h"

// #include "accelerometer.h"
// #include "shutdown.h"
// #include "periodTimer.h"

static void playSingleSound(char *soundPath);

////////////////////////// Accelerometer and Joystick Input
static bool stoppingMenu = false;
static pthread_t menuManagerThreadId;

///////////////////////// Sounds and Songs
static bool stoppingSong = true; // default is true for song thread to check if no playing thread is active - set to false as soon as a player thread is created
#define TEMPO_DEFAULT 120
static int currentTempo = TEMPO_DEFAULT;

static enum eCurrentSong currentSongPlaying = NO_SONG;

// Gets the waiting time for a half beat in milliseconds
// #define GET_BEAT_TIME(BPM) ((120 * 1000) / (BPM))

static pthread_t songPlayerThreadId;
static pthread_mutex_t tempoMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t volumeMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t currentModeMutex = PTHREAD_MUTEX_INITIALIZER;

/********************** Private/Helper functions**********************/

////////////////////////// Accelerometer and Joystick Input

// Sets the timer for action related to idx
// Done after the action gets triggered
// static void setTimers(long long * timers, int idx, int wait_time)
// {
//   if (timers[idx] == 0) {
//     timers[idx] = wait_time;
//   }
// }

// // Reduces wait time for all waiting actions
// // Done at end of each loop
// static void decrementTimers(long long * timers, int size)
// {
//   for (size_t i = 0; i < size; i++) {
//     if (timers[i] > 0) {
//       timers[i] -= INPUT_CHECK_WAIT_TIME;
//     }
//   }
// }

// Checks to see if the action can be triggered or should wait.
// Used for debouncing
static bool isActionTriggered(long long *timers, int idx)
{
  if (timers[idx] == 0)
  {
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

  if (currentJoyStickDirection == JOYSTICK_UP)
  {
    menuManager_UpdateVolume(VOLUME_CHANGE_SIZE, true);
  }
  else if (currentJoyStickDirection == JOYSTICK_DOWN)
  {
    menuManager_UpdateVolume(VOLUME_CHANGE_SIZE, false);
  }
  else if (currentJoyStickDirection == JOYSTICK_LEFT)
  {
    // MenuManager_UpdateTempo(TEMPO_CHANGE_SIZE, false);

    // JOYSTICK IS PRESSED TO The Left
  }
  else if (currentJoyStickDirection == JOYSTICK_RIGHT)
  {
    // MenuManager_UpdateTempo(TEMPO_CHANGE_SIZE, true);
    //  JOYSTICK IS PRESSED TO The RIGHT
  }
  else if (currentJoyStickDirection == JOYSTICK_CENTER)
  {
    if (localCurrentSongPlaying == NO_SONG)
    {
      MenuManager_StopSong();
      MenuManager_StartSong(STANDARD_ROCK_SONG);
    }
    else if (localCurrentSongPlaying == STANDARD_ROCK_SONG)
    {
      MenuManager_StopSong();
      MenuManager_StartSong(MADE_UP_SONG);
    }
    else if (localCurrentSongPlaying == MADE_UP_SONG)
    {
      MenuManager_StopSong();
    }
  }
}

// static void triggerAccelerometerAction(enum eAccelerometerActions accelerometerAction)
// {
//   if (accelerometerAction == ACCELEROMETER_X) {
//     playSingleSound(SNARE_FILE);
//   }
//   if (accelerometerAction == ACCELEROMETER_Y) {
//     playSingleSound(HI_HAT_FILE);
//   }
//   if (accelerometerAction == ACCELEROMETER_Z) {
//     playSingleSound(BASE_DRUM_FILE);
//   }
// }

static void display_menu_content()
{
  printf("Welcome to the BeaglePod Menu !\n");
  printf("1) Song Menu (Press Joystick Center)\n");
  printf("2) Connect to Bluetooth (Move Joystick Right)\n");
  printf("3) Increase Volume (Move Joystickup) \n");
  printf("4) Decrease Volume (Move Joystickdown) \n");
  printf("5) Quit(Move Joystickleft)\n");
}

static void display_songs_in_menu()
{

}

static void *MenuManagerThread(void *arg)
{
  //int timer_size = JOYSTICK_MAX_NUMBER_DIRECTIONS + ACCELEROMETER_MAX_NUMBER_ACTIONS;
  int timer_size = JOYSTICK_MAX_NUMBER_DIRECTIONS;
  // long long action_timers[timer_size];
  // for (size_t i = 0; i < timer_size; i++) {
  //   action_timers[i] = (long long) 0;
  // }

  // 
  display_menu_content();


  while (!stoppingMenu && !Shutdown_isShutdown())
  {
    // Read the Joystick
    enum eJoystickDirections currentJoyStickDirection = Joystick_process_direction();
    //enum eAccelerometerActions accelerometerAction = Accelerometer_GetCurrentAction();

    // Debouncing logic: Joystick
    //int accelerometer_idx = JOYSTICK_MAX_NUMBER_DIRECTIONS + accelerometerAction;
    int accelerometer_idx = JOYSTICK_MAX_NUMBER_DIRECTIONS;
    // Trigger action
    if (isActionTriggered(action_timers, currentJoyStickDirection))
    {
      triggerJoystickAction(currentJoyStickDirection);

      // Update current direction time
      setTimers(action_timers, currentJoyStickDirection, DEBOUNCE_WAIT_TIME);
    }

    // Debouncing logic: Accelerometer
    if (isActionTriggered(action_timers, accelerometer_idx))
    {
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
static void playSingleSound(char *soundPath)
{
  wavedata_t *currentSound = malloc(sizeof(*currentSound));
  AudioMixer_readWaveFileIntoMemory(soundPath, currentSound);
  AudioMixer_queueSound(currentSound);
}

// Thread function to continuously play song passed in as argument
static void *songPlayerThread(void *args)
{
  enum eCurrentSong *mode = (enum eCurrentSong *)args;
  while (!stoppingSong && !Shutdown_isShutdown())
  {
    if (*mode == STANDARD_ROCK_SONG)
    {
      playStandardRockSong(currentRockSoundPlaying);
    }
    else if (*mode == MADE_UP_SONG)
    {
      playMadeUpSong(currentMadeUpSoundPlaying);
    }

    sleepForMs(GET_BEAT_TIME(currentTempo));
  }

  free(args);
  return NULL;
}

/********************** Public/Module functions**********************/

void MenuManager_init(void)
{
  // Joystick
  Joystick_init();

  Bluetooth_init();
  // AudioMixer
  AudioMixer_init();

  LCD_display_Init();
  // Accelerometer
  // Accelerometer_init();
  // Wait for 330ms for the GPIOs to configure before user
  // sleepForMs(330);
}

void MenuManager_HandleInputs(void)
{
  // Launch input manager thread
  pthread_create(&menuManagerThreadId, NULL, MenuManagerThread, NULL);
}

void MenuManager_PlaySingleSound(char *soundPath)
{
  playSingleSound(soundPath);
}

// Starts a new thread to play the song corresponding to mode
void MenuManager_StartSong(enum eCurrentSong mode)
{
  pthread_mutex_lock(&currentModeMutex);
  // only start a new thread when no thread is active
  if (stoppingSong == true)
  {
    stoppingSong = false;
    currentSongPlaying = mode;
    // need to allocate thread argument - to be freed from inside the thread
    enum eCurrentSong *modeArg = malloc(sizeof(enum eCurrentSong));
    *modeArg = mode;
    pthread_create(&songPlayerThreadId, NULL, songPlayerThread, (void *)modeArg);
  }
  pthread_mutex_unlock(&currentModeMutex);
}

// Stops the song plater thread
void MenuManager_StopSong(void)
{
  pthread_mutex_lock(&currentModeMutex);
  stoppingSong = true;
  if (currentSongPlaying != NO_SONG)
  {
    pthread_join(songPlayerThreadId, NULL);
    currentSongPlaying = NO_SONG;
  }
  pthread_mutex_unlock(&currentModeMutex);
}

void MenuManager_cleanup(void)
{
  stoppingMenu = true;
  

  MenuManager_StopSong();
  pthread_join(menuManagerThreadId, NULL);

  // Joystick
  Joystick_cleanup();

  Bluetooth_cleanup();
  // Accelerometer
  // Accelerometer_cleanup();
  // AudioMixer
  AudioMixer_cleanup();

  LCD_display_Cleanup();
}

void MenuManager_UpdateTempo(int changeSize, bool isIncrease)
{
  if (changeSize < 0)
  {
    return;
  }
  pthread_mutex_lock(&tempoMutex);
  if (!isIncrease)
  {
    // decrease tempo
    if ((currentTempo - changeSize) < MIN_TEMPO)
    {
      currentTempo = MIN_TEMPO;
    }
    else
    {
      currentTempo -= changeSize;
    }
  }
  else
  {
    // increase tempo
    if ((currentTempo + changeSize) > MAX_TEMPO)
    {
      currentTempo = MAX_TEMPO;
    }
    else
    {
      currentTempo += changeSize;
    }
  }
  pthread_mutex_unlock(&tempoMutex);
}

void MenuManager_UpdateVolume(int changeSize, bool isIncrease)
{
  if (changeSize < 0)
  {
    return;
  }
  pthread_mutex_lock(&volumeMutex);
  int current_volume = AudioMixer_getVolume();
  int new_volume = current_volume;
  if (!isIncrease)
  {
    // decrease volume
    if ((current_volume - changeSize) < MIN_VOLUME)
    {
      new_volume = MIN_VOLUME;
    }
    else
    {
      new_volume -= changeSize;
    }
  }
  else
  {
    // increase volume
    if ((current_volume + changeSize) > MAX_VOLUME)
    {
      new_volume = MAX_VOLUME;
    }
    else
    {
      new_volume += changeSize;
    }
  }
  AudioMixer_setVolume(new_volume);
  pthread_mutex_unlock(&volumeMutex);
}

int MenuManager_GetCurrentTempo(void)
{
  int tempo = -1;
  pthread_mutex_lock(&tempoMutex);
  tempo = currentTempo;
  pthread_mutex_unlock(&tempoMutex);
  return tempo;
}

int MenuManager_GetCurrentVolume(void)
{
  int volume = -1;
  pthread_mutex_lock(&volumeMutex);
  volume = AudioMixer_getVolume();
  pthread_mutex_unlock(&volumeMutex);
  return volume;
}

int MenuManager_GetCurrentSongPlaying(void)
{
  int currentMode = -1;
  pthread_mutex_lock(&currentModeMutex);
  currentMode = (int)currentSongPlaying;
  pthread_mutex_unlock(&currentModeMutex);

  return currentMode;
}

/////////////////////////////////////////////////////// For Testing ///////////////////////////////////////////////////////
// int main(int argc, char
//   const * argv[])
//   {
//   MenuManager_init();

//   MenuManager_HandleInputs();

//   while (1) {

//   }

//   MenuManager_cleanup();

//   return 0;
// }