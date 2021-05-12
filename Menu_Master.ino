#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <FastLED.h>
#include "N_D_Flagg.h"
#include "UIA_Scrolling.h"

#define LED_PIN_0  0
#define LED_PIN_1  1
#define LED_PIN_2  2
#define LED_PIN_3  3
#define LED_PIN_4  4
#define LED_PIN_5  5
#define LED_PIN_6  6
#define LED_PIN_7  7
#define LED_PIN_8  8
#define LED_PIN_9  10
// give a name and a pin to each light chain. Which light chain is connected to which pin is not very important

#define COLOR_ORDER RGB
// COLOR_ORDER is created as a global variable to put in for the RGB_ORDER parameter in every instance of "FastLED.addLED"
//RGB_ORDER is the RGB ordering for the LEDS. What order red, green, and blue data is written out in.

#define CHIPSET     WS2811
// CHIPSET is is created as a global variable to be able to add the same chipset to every "FastLED.addLed", for the 
// one of the template parameters. The chipset type is required as a paramater.

#define NUM_STRIPS 10  // global variable for number of strips

#define NUM_LEDS_PER_STRIP 50 // global variable for number of LEDS per strip

#define Safety 5  // global variable for an amount of safety LEDS (this is specifically used for "moving dots")

#define NUM_LEDS NUM_STRIPS * NUM_LEDS_PER_STRIP + Safety // global variable for number of LEDS. Calculated by the program by multiplying number of strip and 
                                                          // number of LEDS per strip and then adding the safety LEDS
                                                          
CRGB leds[NUM_LEDS]; // this is where we create our CRGB struct, using the global variable NUM_LEDS, to tell the struct how much LED data it is storing.
                     //(3 bytes of data per LED, 1 byte each for Red, Green, and Blue)

// All of the global variables were created to make it easier to change the size of the matrix, or change the light type and RGB order of the new light type

LiquidCrystal_I2C lcd(0x20,16,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display

const int stopButton = A3;
const int upButton = A2;
const int downButton = A1;
const int selectButton = A0;
// creating the buttons and assigning pins to each button (the analog pins on our Arduino can also be used as digital pins,
// this is done automatically when using pinMode)

String inString = ""; 
int inChar = 0;
int16_t t = 0;
int pixelmap[NUM_LEDS+21] = {0};
// creating what's needed for reading an array from a serial monitor and showing it in our struct

int period_flag = 5000;
int period_scrolling = 200;
int period_dot = 1;
unsigned long time_now = 0;
// creating different times for the different light shows, because we use millis() and not delay()

int menu = 1; // creating the menu variable and always starting it at 1


void setup() {
  Serial.begin(9600);
 
  lcd.init();  // initialize the lcd
  lcd.backlight(); // turn on the backlight (is not always needed but good thing to have to make sure the screen doesn't randomly go dark)
  
  pinMode(stopButton, INPUT_PULLUP);
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);
  // configuring the specific button and pin to be an 'input' and using the internal pullup resistors
  // all of the buttons are inputs because we are reading data from them, and not sending data to them
  // using the internal pullup resistors is done for convenience, otherwise we would have to connect external resistors to each button, creating more mess and wires

  FastLED.setBrightness(100);
  // 
  
  FastLED.addLeds<CHIPSET, LED_PIN_0, COLOR_ORDER>(leds, 0, NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, LED_PIN_1, COLOR_ORDER>(leds, NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, LED_PIN_2, COLOR_ORDER>(leds, 2*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, LED_PIN_3, COLOR_ORDER>(leds, 3*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, LED_PIN_4, COLOR_ORDER>(leds, 4*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, LED_PIN_5, COLOR_ORDER>(leds, 5*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, LED_PIN_6, COLOR_ORDER>(leds, 6*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, LED_PIN_7, COLOR_ORDER>(leds, 7*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, LED_PIN_8, COLOR_ORDER>(leds, 8*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);
  FastLED.addLeds<CHIPSET, LED_PIN_9, COLOR_ORDER>(leds, 9*NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP).setCorrection(TypicalSMD5050);
  // 

  FastLED.clear();
  FastLED.show();
  // this is to make sure that all of the data on the LEDS is cleared before starting
}

void loop() {
  // the while loop only checks if any of the buttons have been pressed (this only works if we're not within a function)
  
  if (digitalRead(stopButton) == LOW){
    //executeStopAction(); // calls the executeStopAction() function, which stops and clears the LEDS. It's commented out, because we don't stop the light shows 
    // while in the main menu anymore
    
    updateMenu(); // calls the updateMenu() function, which updates the menu
    
    delay(100);
    
    while(digitalRead(stopButton) == LOW){
    } // we stay in here until the stopButton is released
  }

  if (digitalRead(upButton) == LOW){
    menu--; // this subtracts the current menu number by 1, i.e. it "scrolls" the menu up
    updateMenu();
    delay(100);
    while(digitalRead(upButton) == LOW){
    }
  }

  if (digitalRead(downButton) == LOW){
    menu++; // this adds 1 to the current menu number, i.e it "scrolls" the menu down
    updateMenu();
    delay(100);
    while(digitalRead(downButton) == LOW){
    }
  }

  if (digitalRead(selectButton) == LOW){
    executeAction(); // calls the executeAction() function, which does different things depending on which menu item is chosen,
                     // but will always display text on the LCD and execute a light show function
    
    updateMenu();
    delay(100);
    while(digitalRead(selectButton) == LOW){
    }
  }
}

// updateMenu() function: is a switch statement, used to switch between menus. Case 0 and case 6 are used as buffers, so we don't get any errors
// The way we have set it up with the LCD screen and the arrows, '>', is so that it resembles a proper scrolling menu
void updateMenu(){
  switch (menu){
    case 0:
      menu = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print("> Moving Dots");
      lcd.setCursor(0,1);
      lcd.print("  Flags");
      break;
    case 2:
      lcd.clear();
      lcd.print("  Moving Dots");
      lcd.setCursor(0,1);
      lcd.print("> Flags");
      break;
    case 3:
      lcd.clear();
      lcd.print("> Rainbow");
      lcd.setCursor(0,1);
      lcd.print("  Scroll UIA");
      break;
    case 4:
      lcd.clear();
      lcd.print("  Rainbow");
      lcd.setCursor(0,1);
      lcd.print("> Scroll UIA");
      break;
    case 5:
      lcd.clear();
      lcd.print("> Upload image:");
      lcd.setCursor(0,1);
      lcd.print("  Connect to PC");
      break;
    case 6:
      menu = 5;
      break;
  }
}


// executeStopAction() function: not in use at this time, but it was to clear the light shows from the main menu
// we no longer go back to the main menu until the light show is stopped, so it is not used. Can be used to put Arduino into sleep mode, if it support it.
// Stopping the light shows from the menu was changed because of some issues with delay(); this is further explained in the report
void executeStopAction() {
  lcd.clear();
  lcd.print("Shutting down");
  delay(1000);
  
  lcd.clear();
  lcd.print("Just kidding...");
  lcd.setCursor(0,1);
  lcd.print("I can't do that");

  FastLED.clear(); 
  FastLED.show(); 
  delay(1500);
}

// executeAction() function: this function will do different things depending on which menu item (case) was chosen up in updateMenu()
// each case clears all the data from the LEDS and then it calls a specific function that should be run
void executeAction() {
  switch (menu){
    case 1:
      FastLED.clear();
      FastLED.show();
      dot();
      break;
    case 2:
      FastLED.clear();
      FastLED.show();
      flags();
      break;
    case 3:
      FastLED.clear();
      FastLED.show();
      rainbow();
      break;
    case 4: 
      FastLED.clear();
      FastLED.show();
      scrollUIA();
      break;
    case 5:
      FastLED.clear();
      FastLED.show();
      uploadSketch();
      break;
  }
}

// dot() function: shows a "submenu", and calls the function movingDot(), which is the actual light show function
// This function doesn't have anything to detect button presses because the program only stays here long enough to 
// print to the LCD and then it goes directly to the next function
void dot() {
  lcd.clear();
  lcd.print("Executing: Dots");
  lcd.setCursor(0,1);
  lcd.print("Red to exit");
  movingDot();
  //delay(1000);
}

// flags() function: shows a "submenu", and calls the function Flagg(), which is the actual light show function
// This function doesn't have anything to detect button presses because the program only stays here long enough to 
// print to the LCD and then it goes directly to the next function
void flags() {
  lcd.clear();
  lcd.print("Executing: Flags");
  lcd.setCursor(0,1);
  lcd.print("Red to exit");
  Flagg();
  //delay(1000);
}

// scrollUIA() function: shows a "submenu", and calls the function UIA(), which is the actual light show function
// This function doesn't have anything to detect button presses because the program only stays here long enough to 
// print to the LCD and then it goes directly to the next function
void scrollUIA() {
  lcd.clear();
  lcd.print("Executing: Scroll");
  lcd.setCursor(0,1);
  lcd.print("Red to exit");
  UIA();
  //delay(1000);
}

// rainbow() function: shows a "submenu", and calls the function stillRainbow(), which is the actual light show function
// This function doesn't have anything to detect button presses because the program only stays here long enough to 
// print to the LCD and then it goes directly to the next function
void rainbow() {
  lcd.clear();
  lcd.print("Executing: R.Bow");
  lcd.setCursor(0,1);
  lcd.print("Red to exit");
  stillRainbow();
}

// uploadSketch() function: shows a "submenu", and calls the function userUpload(), which only runs if there's a connection to a serial monitor
// This function detects a button press, becuase if there isn't a connection to a serial monitor, the program enters an endless loop.
//So the function needs to be able to check if the stopButton has been pressed, so it can exit the loop we've created
void uploadSketch() {
  lcd.clear();
  lcd.print("Open S. Monitor");
  lcd.setCursor(0,1);
  lcd.print("Red to exit");
  userUpload();
  if (digitalRead(stopButton) == LOW){
    stopButtonPress();
  }
}

// movingDot() function: it starts with 5 reds dots moving sequentaly through the leds struct, "painting" the screen blue 
//(we don't clear the leds after the dots have moved by)
// When the red is done, the green dots start, "painting" the screen red. After that the blue dots start "painting" the screen green
void movingDot () {
   for(int c = 0; c < 30; c++){ // the function runs through the full sequence 30 times before clearing and going back to the main menu, a sort of sleep function
                                // this can be overridden by pressing the stop button, this exits to the main menu right away.
    for(int i = 0; i < NUM_LEDS-5; i++) { // this for loop runs through the entire leds struct doing everything within the loop every time it runs through
                                          // it only stops when it's counted up to NUM_LEDS-5. This is because we have 5 dots, and at one point i + (1, 2, 3, or 4) 
                                          // is larger than NUM_LEDS. This breaks the program, because we are trying to send a color to an LED we don't have.
                                          // This function is also why we have a safety of 5 LEDS, if we didn't the for loop would stop the dots before they got
                                          // to the end of the physical matrix. and we'd end up with 4 LEDS of a different color in the bottom left hand corner.
                                          
      if (digitalRead(stopButton) == LOW){ // checks for a button press
        goto exit; // if the stop button has been pressed, the program flow is sent to the label 'exit' (at the end of movingDot())
      }
      
      leds[i] = CRGB::Red;      // set our current dot to red
      leds[i+1] = CRGB::Red;    // set our current dot + 1 to red
      leds[i+2] = CRGB::Red;    // set our current dot + 2 to red
      leds[i+3] = CRGB::Red;    // set our current dot + 3 to red
      leds[i+4] = CRGB::Red;    // set our current dot + 4 to red
      
      FastLED.show(); // shows the colors we've set the struct to
      
      leds[i] = CRGB::Blue; // leave a trail of blue "painting" the matrix
      
      time_now = millis(); // set millis() to the time_now (0). millis() returns the number of milliseconds since the current program started running
                           // we set this to 0 so that it starts counting from the start of this function instead
                           
      while(millis() < time_now + period_dot){ // this is the same as a delay(), except a delay freezes everything, so you can't exit the function with a button
                                               // if we want the dots to move faster or slower, we change the value for period_dot
        if (digitalRead(stopButton) == LOW){
          goto exit;
        }
      } 
    }
    for(int i = 0; i < NUM_LEDS-5; i++) {
      
      if (digitalRead(stopButton) == LOW){
        goto exit;
      }
      
      leds[i] = CRGB::Green;      // set our current dot to green
      leds[i+1] = CRGB::Green;    // set our current dot + 1 to green
      leds[i+2] = CRGB::Green;    // set our current dot + 2 to green
      leds[i+3] = CRGB::Green;    // set our current dot + 3 to green
      leds[i+4] = CRGB::Green;    // set our current dot + 4 to green
      
      FastLED.show();
      
      leds[i] = CRGB::Red; // leave a trail of red "painting" the matrix
      
      time_now = millis();
      
      while(millis() < time_now + period_dot){
        if (digitalRead(stopButton) == LOW){
          goto exit;
        }
      } 
    }
    for(int i = 0; i < NUM_LEDS-5; i++) {
      
      if (digitalRead(stopButton) == LOW){
        goto exit;
      }
      
      leds[i] = CRGB::Blue;      // set our current dot to blue
      leds[i+1] = CRGB::Blue;    // set our current dot + 1 to blue
      leds[i+2] = CRGB::Blue;    // set our current dot + 2 to blue
      leds[i+3] = CRGB::Blue;    // set our current dot + 3 to blue
      leds[i+4] = CRGB::Blue;    // set our current dot + 4 to blue
      
      FastLED.show();
      
      leds[i] = CRGB::Green;  // leave a trail of green "painting" the matrix
      
      time_now = millis();
      
      while(millis() < time_now + period_dot){
        if (digitalRead(stopButton) == LOW){
          goto exit;
        }
      } 
    }
  }
  exit: // label: 'exit'
    stopButtonPress(); // when 'goto exit' is triggered the program flow is transferred to the stopButtonPress() function, which runs the function
}

// Flagg() function: switches between the Danish and the Norwegian flag, with a specific time between each switch
void Flagg()
{
  for(int c = 0; c < 30; c++){ // the function runs through the full sequence 30 times before clearing and going back to the main menu, a sort of sleep function
                               // this can be overridden by pressing the stop button, this exits to the main menu right away.
                               
    for(int i=0; i<NUM_LEDS ; i++) // this for loop runs through the entire leds struct doing everything within the loop every time it runs through
                                   // it only stops when it's counted up to NUM_LEDS.
    {
      if (digitalRead(stopButton) == LOW){
        goto exit;
      }
      leds[i] = Dansk[i]; // this swaps out every value in leds[] with the corresponding value in Dansk[]
                          // we transpose the Dansk[] array into our leds[] struct
    }  
    FastLED.show(); // shows the new matrix/image
    
    time_now = millis(); // set millis() to the time_now (0). millis() returns the number of milliseconds since the current program started running
                         // we set this to 0 so that it starts counting from the start of this function instead
    
    while(millis() < time_now + period_flag){ // this is the same as a delay(), except a delay freezes everything, so you can't exit the function with a button
                                              // if we want the time between the flags switching to be faster or slower, we change the value for period_flag
      if (digitalRead(stopButton) == LOW){
        goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
        goto exit;
      }
      leds[i] = Norsk[i]; // this swaps out every value in leds[] with the corresponding value in Norsk[]
                          // we transpose the Norsk[] array into our leds[] struct
    }
    
    FastLED.show();
    
    time_now = millis();
    
    while(millis() < time_now + period_flag){
      if (digitalRead(stopButton) == LOW){
        goto exit;
      }
    }
  }
  exit:
    stopButtonPress();
}

// stillRainbow function: shows a static (still) picture of a rainbow flag
// it's supposed to be {Red, Orange, Yellow, Green, Blue, Purple, Violet} but some of the colors don't show up very well, with full brightness
void stillRainbow(){
  while(1){ // an endless loop so we can choose when to stop showing the picture by pressing the stop button. Otherwise the program would show the picture so fast
            // we wouldn't be able to see it
    const int RainbowPic[] PROGMEM = { // this is the rainbow array we created in our Excel file, every second line looks backwards, that's
                                       // because the layout of our light strips is slightly backwards to the way we want the program to show everything.
                                       // the array is saved in flash (program) memory (PROGMEM) so that it doesn't take up too much space on SRAM
                                       // there's usually more space on flash memory than SRAM
    CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::White,CRGB::Black,

    CRGB::Black,CRGB::White,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::White,CRGB::Black,

    CRGB::Black,CRGB::White,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::White,CRGB::Black,

    CRGB::Black,CRGB::White,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::Violet,CRGB::Violet,CRGB::Violet,CRGB::Purple,CRGB::Purple,CRGB::Purple,CRGB::Blue,CRGB::Blue,CRGB::Blue,CRGB::Green,CRGB::Green,CRGB::Green,CRGB::Yellow,CRGB::Yellow,CRGB::Yellow,CRGB::Orange,CRGB::Orange,CRGB::Orange,CRGB::Red,CRGB::Red,CRGB::Red,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::White,CRGB::Black,
    CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,CRGB::Black,
    };
    
    for (int i = 0; i < NUM_LEDS; i++){ // this for loop does the smae thing as the for loops in the Flagg() function
                                        // we loop through all of the LEDS (NUM_LEDS), and for each spot/location, we transpose the value from the RainbowPic[] array 
                                        // into the leds[] struct
      leds[i] = RainbowPic[i];
    }
    
    FastLED.show(); // show the picture
    
    if (digitalRead(stopButton) == LOW){ // check for button press
      goto exit; //the program flow is sent to the label 'exit'
    }
  }
  exit:
    stopButtonPress(); // runs the stopButtonPress() function
}

void UIA()
{
  for(int c = 0; c < 30; c++){
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA37[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA36[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA35[i];
  }
  FastLED.show();
    time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA34[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA33[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA32[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA31[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA30[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA29[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA28[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA27[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA26[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA25[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA24[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA23[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA22[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    };
  for(int i=0; i<NUM_LEDS ; i++)
  {
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    leds[i] = UIA21[i];
  }
  FastLED.show();
  time_now = millis();
  while(millis() < time_now + period_scrolling){
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
  };  
  for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA20[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA19[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA18[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA16[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA15[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA14[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA13[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA12[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA11[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA10[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA9[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA8[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA7[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA6[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA5[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA4[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA3[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA2[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
    for(int i=0; i<NUM_LEDS ; i++)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      leds[i] = UIA1[i]; 
    }  
    FastLED.show();
    time_now = millis();
    while(millis() < time_now + period_scrolling){
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
    }
  }
  exit:
    stopButtonPress();
}

// stopButtonPress() function: when the stop button is pressed during any other function, the program runs this one instead
// it turns on the LCDs backlight, clears the screen and prints the message "exiting", after a delay of 1000ms, it will
// clear all of the data on the LEDS and then run the updateMenu() function, which returns us to the main menu and the
// user can choose another light sequence/show
void stopButtonPress() {
  lcd.backlight();
  lcd.clear();
  lcd.print("Exiting");
  delay(1000);
  FastLED.clear();
  FastLED.show();
  updateMenu();
  //stopLightShow = true;
}

void userUpload(){
  while (!Serial) {;}
  Serial.println();
  Serial.println("Ready to receive pixeldata...");
  Serial.print("Red button to exit");
  Serial.println();
  serialAvailable();
  if (digitalRead(stopButton) == LOW){
          goto exit;
      }
  exit:
    stopButtonPress();
}

void serialAvailable(){
  while(1){
    while (Serial.available() > 0)
    {
      if (digitalRead(stopButton) == LOW){
          goto exit;
      }
      User_Input2();
      FillArray();
      Col_conversion();
      FastLED.show();
    }
    if (digitalRead(stopButton) == LOW){
          goto exit;
      }
  }
  exit:
    stopButtonPress(); 
}

void User_Input2()
{
    inString = Serial.readString();
    Serial.println();
    Serial.print("String: ");
    Serial.println(inString);
}

void FillArray()
{
  Serial.println("Filling Array... ");
  FastLED.clear();
  for (int i = 0; i < 521; i++)
  {
    pixelmap[i] = inString.charAt(i);  
  }
  inString = "";
}

void Col_conversion()
{
  for (int i = 0, n = 0; i < NUM_LEDS+21; i++, n++)
  {
    switch(pixelmap[i]) 
    {
    case 32:
        n--;
        break;
    case 48:
        leds[n] = CRGB::Black; //0x000000;
        break;
    case 49:
        leds[n] = CRGB::White; //0xFFFFFF;
        break;
    case 50:
        leds[n] = CRGB::Red; //0xFF0000;
        break;
    case 51:
        leds[n] = CRGB::Yellow; //0xFFFF00;
        break;
    case 52:
        leds[n] = CRGB::Green; //0x00FF00;
        break;
    case 53:
        leds[n] = CRGB::Blue; //0x0000FF;
        break;
    case 54:
        leds[n] = CRGB::Purple; //0x0000FF; -blue
        break;
    case 55:
        leds[n] = CRGB::Violet; //0x0000FF; -blue
        break;
    case 56:
        leds[n] = CRGB::Orange; //0x0000FF; -blue
        break;
    default:
        Serial.print("");     
    }
  }
  memset (pixelmap, 0, sizeof(pixelmap));
  
  Serial.println("Image Uploaded");
  Serial.println();
  Serial.println("Ready to receive pixeldata...");
  Serial.print("Red button to exit");
  Serial.println();
}
