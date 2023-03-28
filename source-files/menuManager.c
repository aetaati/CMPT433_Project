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
static MENU current_menu = MAIN_MENU;
static MAIN_OPTIONS current_main = SONGS_OPT; 
wavedata_t *pSound_currentSong = NULL;

static char* main_menu_option_strings[NUM_OPTIONS] = {
  "Select Song",
  "Bluetooth",
  "Settings",
  "Poweroff"
};

static char** bt_scan_option_strings;

 
static void Main_setArrowAtLine(LCD_LINE_NUM line);


static void displayMainMenu()
{
  current_menu = MAIN_MENU;
  current_main = SONGS_OPT;
  Main_setArrowAtLine(SONGS_OPT);
}




static void displayBluetoothMenu(){
  current_menu = BLUETOOTH_MENU;
  LCD_clear();
 

  // scan
  LCD_writeStringAtLine("Scanning for Devices", LCD_LINE1);
  bluetooth_scan_t* scanner = malloc(sizeof(bluetooth_scan_t));
  Bluetooth_scan(scanner);
  char* names[scanner->num_devices];
  Bluetooth_getHumanReadableNames(scanner, names);
  printf("num scanned: %d\n", scanner->num_devices);
  bt_scan_option_strings = names;
  LCD_clear();
  LCD_writeStringAtLine("    Select Device", LCD_LINE1);
  LCD_writeStringAtLine("", LCD_LINE2);
  LCD_writeChar(LCD_RIGHT_ARROW);

  // display up to the first 3 devices
  int index = 1;
  LCD_writeString(bt_scan_option_strings[0]);
  while(index < 3 && index < scanner->num_devices){
    LCD_writeStringAtLine(bt_scan_option_strings[index], index + 1);
    printf("%s %d\n", bt_scan_option_strings[index], index + 1);
    index++;
  }
}

static void displaySettingsMenu(){
  LCD_clear();
  LCD_writeStringAtLine("Settings Menu", LCD_LINE1);
}

static void displaySongMenu()
{
  LCD_clear();
  LCD_writeStringAtLine("Songs Menu", LCD_LINE1);
}



static void mainMenuSwitch(MAIN_OPTIONS option){
  switch(option){
    case SONGS_OPT:
      displaySongMenu();
      break;
    case BLUETOOTH_OPT:
      displayBluetoothMenu();
      break;
    case SETTINGS_OPT:
      displaySettingsMenu();
      break;
    case POWEROFF_OPT:
      Shutdown_triggerForShutdown();
      break;
    default:
      // invalid option
      break;
  }
}


static pthread_mutex_t currentModeMutex = PTHREAD_MUTEX_INITIALIZER;

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

static void bluetoothMenuJoystickAction(enum eJoystickDirections currentJoyStickDirection){
  switch(currentJoyStickDirection){
    case JOYSTICK_UP:
      // scroll up

      break;

    case JOYSTICK_DOWN:
      // scroll down
      
      break;

    case JOYSTICK_CENTER:
      // connect to selected device

      break;
    case JOYSTICK_LEFT:
      displayMainMenu();
      
    default:
      // unsupported direction
      break;
  }
  


}


static void mainMenuJoystickAction(enum eJoystickDirections currentJoyStickDirection)
{


  switch(currentJoyStickDirection){
    case JOYSTICK_UP:
      // scroll up
      if( current_main == SONGS_OPT){
        current_main = POWEROFF_OPT;
      }
      else{
        current_main--;
      }
      Main_setArrowAtLine(current_main);
      break;

    case JOYSTICK_DOWN:
      // scroll down
      current_main = (current_main + 1) % NUM_OPTIONS;
      Main_setArrowAtLine(current_main);
      break;

    case JOYSTICK_CENTER:
      // drill into sub menu
      mainMenuSwitch(current_main);
      break;
      
    default:
      // unsupported direction
      break;
  }
}


static void songMenuJoystickAction(enum eJoystickDirections currentJoyStickDirection)
{
  switch(currentJoyStickDirection){
    case JOYSTICK_UP:
      break;
    case JOYSTICK_DOWN:
      break;
    case JOYSTICK_LEFT:
      break;
    case JOYSTICK_RIGHT:
      break;
    default:
      // unsupported direction
      break;
  }
}





static void Main_setArrowAtLine(LCD_LINE_NUM line){
  switch(line){
      case LCD_LINE1:
      // draw arrow on line 1
        LCD_clear();
        LCD_writeChar(LCD_RIGHT_ARROW);
        LCD_writeString(main_menu_option_strings[0]);    
        LCD_writeStringAtLine(main_menu_option_strings[1], LCD_LINE2);
        LCD_writeStringAtLine(main_menu_option_strings[2], LCD_LINE3);
        LCD_writeStringAtLine(main_menu_option_strings[3],  LCD_LINE4);
        
        break;
      case LCD_LINE2:
       // draw arrow line 2
        LCD_clear();
        LCD_writeStringAtLine(main_menu_option_strings[0], LCD_LINE1);
        LCD_writeStringAtLine("", LCD_LINE2);
        LCD_writeChar(LCD_RIGHT_ARROW);
        LCD_writeString(main_menu_option_strings[1]);
        LCD_writeStringAtLine(main_menu_option_strings[2], LCD_LINE3);
        LCD_writeStringAtLine(main_menu_option_strings[3], LCD_LINE4);
        
        break;
      case LCD_LINE3:
      // draw arrow line 3
        LCD_clear();
        LCD_writeString(main_menu_option_strings[0]);
        LCD_writeStringAtLine(main_menu_option_strings[1], LCD_LINE2);
        LCD_writeStringAtLine("", LCD_LINE3); // set cursor 
        LCD_writeChar(LCD_RIGHT_ARROW);
        LCD_writeString(main_menu_option_strings[2]);
        LCD_writeStringAtLine(main_menu_option_strings[3], LCD_LINE4);
       
        break;
      case LCD_LINE4:
      // draw arrow on line 4
        LCD_clear();
        LCD_writeString(main_menu_option_strings[0]);
        LCD_writeStringAtLine(main_menu_option_strings[1], LCD_LINE2);
        LCD_writeStringAtLine(main_menu_option_strings[2], LCD_LINE3); 
        LCD_writeStringAtLine("", LCD_LINE4);
        LCD_writeChar(LCD_RIGHT_ARROW);// set cursor 
      
        LCD_writeString(main_menu_option_strings[3]);
        
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

  displayMainMenu();

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
          mainMenuJoystickAction(currentJoyStickDirection);
          break;
        case SONGS_MENU:
          songMenuJoystickAction(currentJoyStickDirection);
          break;
        case BLUETOOTH_MENU:
          bluetoothMenuJoystickAction(currentJoyStickDirection);
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
  LCD_init();

  songManager_init();

  Joystick_init();

  pthread_create(&menuManagerThreadId, NULL, MenuManagerThread, NULL);
}

void MenuManager_cleanup(void)
{
  stoppingMenu = true;

  pthread_join(menuManagerThreadId, NULL);

  Joystick_cleanup();

  songManager_cleanup();

  LCD_cleanup();
}



song_info *MenuManager_GetCurrentSongPlaying(void)
{
  song_info *current_song = NULL;
  pthread_mutex_lock(&currentModeMutex);
  current_song = songManager_getCurrentSongPlaying();
  pthread_mutex_unlock(&currentModeMutex);
  return current_song;
}
