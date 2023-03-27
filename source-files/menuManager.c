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
#include "gpio.h"
#include "shutdown.h"
#include "bluetooth.h"
#include "sleep.h"
#include "lcd_4line.h"

#define INPUT_CHECK_WAIT_TIME 5
#define DEBOUNCE_WAIT_TIME 100


/**
 * GLOBAL VARIABLES
*/
static bool stoppingMenu = false;
static pthread_t menuManagerThreadId;
static bool show_once_menu = false;
static int current_song_number = 1;
static bool show_display_songs = false;
static MENU current_menu = MAIN_MENU; // MAIN 
static MAIN_OPTIONS current_main = SONGS_OPT; 
wavedata_t *pSound_currentSong = NULL;

 
static void setArrowAtLine(LCD_LINE_NUM line);

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
  if (currentJoyStickDirection == JOYSTICK_UP)
  {
    printf("current option %d\n", current_main);
    if( current_main == SONGS_OPT){
      current_main = POWEROFF_OPT;
    }
    else{
      current_main--;
    }

    setArrowAtLine(current_main);
    printf("after joystick %d\n", current_main);
  }
  else if (currentJoyStickDirection == JOYSTICK_DOWN)
  {
    // scroll down
    printf("current option %d\n", current_main);
    current_main = (current_main + 1) % NUM_OPTIONS;
    
    setArrowAtLine(current_main);
    printf("after joystick %d\n", current_main);

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


static void display_menu_content()
{
  setArrowAtLine(SONGS_OPT);
}

static void display_songs_in_menu()
{
  songManager_displayAllSongs();
  printf("Go back to the main menu \n");
}

static void setArrowAtLine(LCD_LINE_NUM line){
  switch(line){
      case LCD_LINE1:
      // draw arrow on line 1
        LCD_clear();
        LCD_writeChar(LCD_RIGHT_ARROW);
        LCD_writeString("Select Song");    
        LCD_writeStringAtLine("Bluetooth", LCD_LINE2);
        LCD_writeStringAtLine("Settings", LCD_LINE3);
        LCD_writeStringAtLine("Poweroff", LCD_LINE4);
        
        break;
      case LCD_LINE2:
       // draw arrow line 2
        LCD_clear();
        LCD_writeStringAtLine("                   ", LCD_LINE1);
        LCD_writeStringAtLine("Select Song", LCD_LINE1);
        LCD_writeStringAtLine("", LCD_LINE2);
        LCD_writeChar(LCD_RIGHT_ARROW);
        LCD_writeString("Bluetooth");
        LCD_writeStringAtLine("Settings", LCD_LINE3);
        LCD_writeStringAtLine("Poweroff", LCD_LINE4);
        
        break;
      case LCD_LINE3:
      // draw arrow line 3
        LCD_clear();
        LCD_writeString("Select Song");
        LCD_writeStringAtLine("Bluetooth", LCD_LINE2);
        LCD_writeStringAtLine("", LCD_LINE3); // set cursor 
        LCD_writeChar(LCD_RIGHT_ARROW);
        LCD_writeString("Settings");
        LCD_writeStringAtLine("Poweroff", LCD_LINE4);
       
        break;
      case LCD_LINE4:
      // draw arrow on line 4
        LCD_clear();
        LCD_writeString("Select Song");
        LCD_writeStringAtLine("Bluetooth", LCD_LINE2);
        LCD_writeStringAtLine("Settings", LCD_LINE3); 
        LCD_writeStringAtLine("", LCD_LINE4);
        LCD_writeChar(LCD_RIGHT_ARROW);// set cursor 
      
        LCD_writeString("Poweroff");
        
        break;
      default:
        break;
    }
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

  display_menu_content();

  /*************************************************************************/

  while (!stoppingMenu && !Shutdown_isShutdown())
  {

    // Read the Joystick
    enum eJoystickDirections currentJoyStickDirection = Joystick_process_direction();


    // Debouncing logic: Joystick
    // Trigger action
    if (isActionTriggered(action_timers, currentJoyStickDirection) && currentJoyStickDirection != JOYSTICK_NONE)
    {
      switch(current_menu){
        case MAIN_MENU:
          // Display Menu
          //display_menu_content();
          
          
          mainMenuJoystickAction(currentJoyStickDirection);
          
          break;
        case SONGS_MENU:
          // Display the songs menu
          display_songs_in_menu();
          songMenuJoystickAction(currentJoyStickDirection);

        
          break;
        case BLUETOOTH_MENU:
          break;
        case SETTINGS_MENU:
          break;
        default:
          // invalid option
          break;
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
  songManager_init();

  Joystick_init();

  LCD_init();

  // Launch menu manager thread
  pthread_create(&menuManagerThreadId, NULL, MenuManagerThread, NULL);
}

void MenuManager_cleanup(void)
{
  stoppingMenu = true;

  pthread_join(menuManagerThreadId, NULL);

  LCD_cleanup();

  Joystick_cleanup();

  songManager_cleanup();
}



song_info *MenuManager_GetCurrentSongPlaying(void)
{
  song_info *current_song = NULL;
  pthread_mutex_lock(&currentModeMutex);
  current_song = songManager_getCurrentSongPlaying();
  pthread_mutex_unlock(&currentModeMutex);
  return current_song;
}
