/*
Author: Mehdi Esmaeilzadeh
Date: 2023-03-12
Subject: Implementation of the MenuManager module
*/

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#include "menuManager.h"
#include "songManager.h"
#include "audio_player.h"
#include "joystick.h"
#include "lcd_display.h"
#include "gpio.h"
#include "shutdown.h"
#include "bluetooth.h"
#include "sleep.h"

static bool stoppingMenu = false;

static pthread_t menuManagerThreadId;

static bool show_once_menu = false;

static int current_song_number = 1;
static bool show_display_songs = false;

//////////////////////MIGHT USE
// #define TEMPO_DEFAULT 120
// static int currentTempo = TEMPO_DEFAULT;

wavedata_t *pSound_currentSong = NULL;

// static pthread_mutex_t tempoMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t volumeMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t currentModeMutex = PTHREAD_MUTEX_INITIALIZER;

// Flags Used for switching to the playsong menu
static bool display_mainMenu = true;

/********************** Private/Helper functions**********************/

////////////////////////// Accelerometer and Joystick Input

// Sets the timer for action related to idx
// Done after the action gets triggered
static void setTimers(long long *timers, int idx, int wait_time)
{
  if (timers[idx] == 0)
  {
    timers[idx] = wait_time;
  }
}

// // Reduces wait time for all waiting actions
// // Done at end of each loop
static void decrementTimers(long long *timers, int size)
{
  for (size_t i = 0; i < size; i++)
  {
    if (timers[i] > 0)
    {
      timers[i] -= INPUT_CHECK_WAIT_TIME;
    }
  }
}

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

static void mainMenuJoystickAction(enum eJoystickDirections currentJoyStickDirection)
{
  /***
   * 1) If Joystick is pressed Up Increase the Volume
   * 2) If Joystick is pressed Down Decrease the volume
   * 3) If Joystick is pressed Left QUIT from the main menu
   * 4) If Joystick is pressed Right Connect to the Bluetooth
   * 5) If Joystick is pressed Center Go the songs menu
   */
  if (currentJoyStickDirection == JOYSTICK_UP)
  {
    MenuManager_UpdateVolume(VOLUME_CHANGE_SIZE, true);
  }
  else if (currentJoyStickDirection == JOYSTICK_DOWN)
  {
    MenuManager_UpdateVolume(VOLUME_CHANGE_SIZE, false);
  }
  else if (currentJoyStickDirection == JOYSTICK_LEFT)
  {
    printf("Quiting from the Beaglepod !\n");
    Shutdown_triggerForShutdown();
  }
  else if (currentJoyStickDirection == JOYSTICK_RIGHT)
  {
    //  Connect to the Bluetooth
    int selection;
    inquiry_info *devices;
    char input[15] = {0};
    devices = malloc(BT_MAX_DEV_RSP * sizeof(inquiry_info));
    int num_scanned = Bluetooth_scan(devices, BT_MAX_DEV_RSP);
    Bluetooth_displayDevices(devices, num_scanned);

    printf("Choose a device to connect to\n> ");
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
      fprintf(stderr, "error reading from stdin");
    }
    sscanf(input, "%d", &selection);

    printf("connecting to device...\n");
    if (Bluetooth_connect(&(devices + selection)->bdaddr) != 0)
    {
      printf("error connecting to device\n");
    }
    else
    {
      printf("Connected!\n");
    }
  }
  else if (currentJoyStickDirection == JOYSTICK_CENTER)
  {
    display_mainMenu = false;
    show_display_songs = false;
  }
}

static void songMenuJoystickAction(enum eJoystickDirections currentJoyStickDirection)
{
  /**
   * 1) If Pressed Joystick is up then play song #1
   * 2) If Pressed Joystick is down then play song #2
   * 3) If Pressed Joystick is right then play song #3
   * 4) If Pressed Joystick is left then go back to the main menu
   */

  if (currentJoyStickDirection == JOYSTICK_UP)
  {
    if (current_song_number - 1 >= 1)
    {
      current_song_number--;
    }
  }
  else if (currentJoyStickDirection == JOYSTICK_DOWN)
  {
    if (current_song_number + 1 <= (int)songManager_currentNumberSongs())
    {
      current_song_number++;
    }
  }
  else if (currentJoyStickDirection == JOYSTICK_CENTER)
  {
    songManager_playSong(current_song_number);
  }
  else if (currentJoyStickDirection == JOYSTICK_LEFT)
  {
    current_song_number = 1;
    display_mainMenu = true;
    show_once_menu = false;
  }
}

/**********************************FUNCTION WE MIGHT USE ******************************************************/
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
/**************************************************************************************************************/

static void display_menu_content()
{
  printf("Welcome to the BeaglePod Menu !\n");
  printf("1) Song Menu (Press Joystick Center)\n");
  printf("2) Connect to Bluetooth (Move Joystick Right)\n");
  printf("3) Settings \n");
  printf("4) Quit(Move Joystickleft)\n");
}

static void display_songs_in_menu()
{

  songManager_displayAllSongs();
  printf("Go back to the main menu \n");
}

static void *MenuManagerThread(void *arg)
{
  // int timer_size = JOYSTICK_MAX_NUMBER_DIRECTIONS + ACCELEROMETER_MAX_NUMBER_ACTIONS;

  /********************TIMERS USED FOR DEBOUNCING LOGIC********************/

  int timer_size = JOYSTICK_MAX_NUMBER_DIRECTIONS;
  long long action_timers[timer_size];
  for (size_t i = 0; i < timer_size; i++)
  {
    action_timers[i] = (long long)0;
  }

  /*************************************************************************/

  while (!stoppingMenu && !Shutdown_isShutdown())
  {
    if (display_mainMenu)
    {
      // Display Menu
      if (!show_once_menu)
      {
        show_once_menu = true;
        display_menu_content();
      }
    }
    else
    {
      // Display the songs menu
      if (!show_display_songs)
      {
        show_display_songs = true;
        display_songs_in_menu();
      }
    }
    // Read the Joystick
    enum eJoystickDirections currentJoyStickDirection = Joystick_process_direction();

    // Debouncing logic: Joystick
    // Trigger action
    if (isActionTriggered(action_timers, currentJoyStickDirection))
    {
      if (display_mainMenu)
      {
        mainMenuJoystickAction(currentJoyStickDirection);
      }
      else
      {
        songMenuJoystickAction(currentJoyStickDirection);
      }
      // Update current direction time
      setTimers(action_timers, currentJoyStickDirection, DEBOUNCE_WAIT_TIME);
    }

    // Adjust timers
    decrementTimers(action_timers, timer_size);

    Sleep_ms(INPUT_CHECK_WAIT_TIME);
  }

  return NULL;
}

/********************** Public/Module functions**********************/

void MenuManager_init(void)
{
  // Joystick
  Joystick_init();
  // AudioPlayer
  AudioPlayer_init();

  LCD_display_Init();

  songManager_init();

  // Launch menu manager thread
  pthread_create(&menuManagerThreadId, NULL, MenuManagerThread, NULL);
}

void MenuManager_cleanup(void)
{
  stoppingMenu = true;

  pthread_join(menuManagerThreadId, NULL);

  // Joystick
  Joystick_cleanup();

  // AudioPlayer
  AudioPlayer_cleanup();

  LCD_display_Cleanup();

  songManager_cleanup();
}

void MenuManager_UpdateVolume(int changeSize, bool isIncrease)
{
  if (changeSize < 0)
  {
    return;
  }
  pthread_mutex_lock(&volumeMutex);
  int current_volume = AudioPlayer_getVolume();
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
  AudioPlayer_setVolume(new_volume);
  pthread_mutex_unlock(&volumeMutex);
}

int MenuManager_GetCurrentVolume(void)
{
  int volume = -1;
  pthread_mutex_lock(&volumeMutex);
  volume = AudioPlayer_getVolume();
  pthread_mutex_unlock(&volumeMutex);
  return volume;
}

song_info *MenuManager_GetCurrentSongPlaying(void)
{
  song_info *current_song = NULL;
  pthread_mutex_lock(&currentModeMutex);
  current_song = songManager_getCurrentSongPlaying();
  pthread_mutex_unlock(&currentModeMutex);
  return current_song;
}

/////////////////////////////FUNCTIONS MIGHT USE//////////////////////////////////////////////////
// void MenuManager_UpdateTempo(int changeSize, bool isIncrease)
// {
//   if (changeSize < 0)
//   {
//     return;
//   }
// pthread_mutex_lock(&tempoMutex);
// if (!isIncrease)
// {
//   // decrease tempo
//   if ((currentTempo - changeSize) < MIN_TEMPO)
//   {
//     currentTempo = MIN_TEMPO;
//   }
//   else
//   {
//     currentTempo -= changeSize;
//   }
// }
//   else
//   {
//     // increase tempo
//     if ((currentTempo + changeSize) > MAX_TEMPO)
//     {
//       currentTempo = MAX_TEMPO;
//     }
//     else
//     {
//       currentTempo += changeSize;
//     }
//   }
//   pthread_mutex_unlock(&tempoMutex);
// }

// int MenuManager_GetCurrentTempo(void)
// {
//   int tempo = -1;
//   pthread_mutex_lock(&tempoMutex);
//   tempo = currentTempo;
//   pthread_mutex_unlock(&tempoMutex);
//   return tempo;
// }

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