/* 
 *  Megans word clock
 */
 
#include <avr/pgmspace.h>
#include <PololuLedStrip.h>
#include <swRTC.h>
#include "DHT.h"

#define DHTPIN 2     // what digital pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

//Create a new istance of the sw RTC lib
swRTC rtc; 

// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<12> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 240
rgb_color colors[LED_COUNT];

enum ewords { wrd_its = 0, wrd_a , wrd_just, wrd_gone, wrd_near, wrd_nearly , wrd_ten , wrd_quarter , wrd_twenty , wrd_half , wrd_five , 
              wrd_minutes , wrd_to , wrd_past , wrd_hone , wrd_hthree , wrd_htwo , wrd_hfour , wrd_hfive , wrd_hsix , wrd_hseven , 
              wrd_height , wrd_hnine, wrd_hten, wrd_heleven , wrd_htwelve , wrd_oclock ,  wrd_in , wrd_the , wrd_morning , 
              wrd_evening , wrd_midnight , wrd_afternoon , wrd_midday , wrd_today, wrd_is , wrd_wednesday , wrd_monday , wrd_tuesday , wrd_thursday , 
              wrd_friday , wrd_sunday , wrd_saturday , wrd_at, wrd_night};

const PROGMEM int words_st[] {
                    236 , 234 , 229 , 224 , 208 , 208 , 215 , 192 , 218 , 204 , 200 , 
                    176 , 189 , 184 , 173 , 164 , 128 , 156 , 147 , 144 , 132 , 
                    169 , 160 , 113 , 150 , 138 , 117 , 125 , 107 , 73  , 
                    80  , 64  , 97  , 88  , 48  , 54  , 32  , 42  , 57  , 17  , 
                    26  , 10  , 1   , 110 , 64};       
              
const PROGMEM int words_len[] {
                    4   , 1   , 4   , 4   , 4   , 6   , 3   , 7   , 6   , 4   , 4   ,
                    7   , 2   , 4   , 3   , 5   , 3   , 4   , 4   , 3   , 5   , 
                    5   , 4   , 3   , 6   , 6   , 7   , 2   , 3   , 7   ,
                    7   , 8   , 9   , 6   , 5   , 2   , 9   , 6   , 7   , 8   ,
                    6   , 6   , 8   , 2   , 5};       

const char month_01[] PROGMEM = {"January"};
const char month_02[] PROGMEM = {"Febuary"};
const char month_03[] PROGMEM = {"March"};
const char month_04[] PROGMEM = {"April"};
const char month_05[] PROGMEM = {"May"};
const char month_06[] PROGMEM = {"June"};
const char month_07[] PROGMEM = {"July"};
const char month_08[] PROGMEM = {"August"};
const char month_09[] PROGMEM = {"September"};
const char month_10[] PROGMEM = {"October"};
const char month_11[] PROGMEM = {"November"};
const char month_12[] PROGMEM = {"December"};

const char* const month_table[] PROGMEM = {month_01, month_02, month_03, month_04, month_05, month_06,
                                           month_07, month_08, month_09, month_10, month_11, month_12};

const char  day_01[] PROGMEM = {"Sunday"};
const char  day_02[] PROGMEM = {"Monday"};
const char  day_03[] PROGMEM = {"Tuesday"};
const char  day_04[] PROGMEM = {"Wednesday"};
const char  day_05[] PROGMEM = {"Thursday"};
const char  day_06[] PROGMEM = {"Friday"};
const char  day_07[] PROGMEM = {"Saturday"};

const char* const day_table[] PROGMEM ={day_01, day_02, day_03, day_04, day_05, day_06, day_07};

int num_words = 0;
ewords words_ids[20];

const PROGMEM char font5x7[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // 0x20, Space 
   0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04,   // 0x21, ! 
   0x09, 0x09, 0x12, 0x00, 0x00, 0x00, 0x00,   // 0x22, " 
   0x0a, 0x0a, 0x1f, 0x0a, 0x1f, 0x0a, 0x0a,   // 0x23, # 
   0x04, 0x0f, 0x14, 0x0e, 0x05, 0x1e, 0x04,   // 0x24, $ 
   0x19, 0x19, 0x02, 0x04, 0x08, 0x13, 0x13,   // 0x25, % 
   0x04, 0x0a, 0x0a, 0x0a, 0x15, 0x12, 0x0d,   // 0x26, & 
   0x04, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00,   // 0x27, ' 
   0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02,   // 0x28, ( 
   0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08,   // 0x29, ) 
   0x04, 0x15, 0x0e, 0x1f, 0x0e, 0x15, 0x04,   // 0x2a, * 
   0x00, 0x04, 0x04, 0x1f, 0x04, 0x04, 0x00,   // 0x2b, + 
   0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x08,   // 0x2c, , 
   0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00,   // 0x2d, - 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c,   // 0x2e, . 
   0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10,   // 0x2f, / 
   0x0e, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0e,   // 0x30, 0 
   0x04, 0x0c, 0x04, 0x04, 0x04, 0x04, 0x0e,   // 0x31, 1 
   0x0e, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1f,   // 0x32, 2 
   0x0e, 0x11, 0x01, 0x06, 0x01, 0x11, 0x0e,   // 0x33, 3 
   0x02, 0x06, 0x0a, 0x12, 0x1f, 0x02, 0x02,   // 0x34, 4 
   0x1f, 0x10, 0x1e, 0x01, 0x01, 0x11, 0x0e,   // 0x35, 5 
   0x06, 0x08, 0x10, 0x1e, 0x11, 0x11, 0x0e,   // 0x36, 6 
   0x1f, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08,   // 0x37, 7 
   0x0e, 0x11, 0x11, 0x0e, 0x11, 0x11, 0x0e,   // 0x38, 8 
   0x0e, 0x11, 0x11, 0x0f, 0x01, 0x02, 0x0c,   // 0x39, 9 
   0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x0c, 0x00,   // 0x3a, : 
   0x00, 0x0c, 0x0c, 0x00, 0x0c, 0x04, 0x08,   // 0x3b, ; 
   0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02,   // 0x3c, < 
   0x00, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x00,   // 0x3d, = 
   0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08,   // 0x3e, > 
   0x0e, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04,   // 0x3f, ? 
   0x0e, 0x11, 0x17, 0x15, 0x17, 0x10, 0x0f,   // 0x40, @ 
   0x04, 0x0a, 0x11, 0x11, 0x1f, 0x11, 0x11,   // 0x41, A 
   0x1e, 0x11, 0x11, 0x1e, 0x11, 0x11, 0x1e,   // 0x42, B 
   0x0e, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0e,   // 0x43, C 
   0x1e, 0x09, 0x09, 0x09, 0x09, 0x09, 0x1e,   // 0x44, D 
   0x1f, 0x10, 0x10, 0x1c, 0x10, 0x10, 0x1f,   // 0x45, E 
   0x1f, 0x10, 0x10, 0x1f, 0x10, 0x10, 0x10,   // 0x46, F 
   0x0e, 0x11, 0x10, 0x10, 0x13, 0x11, 0x0f,   // 0x37, G 
   0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11,   // 0x48, H 
   0x0e, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0e,   // 0x49, I 
   0x1f, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0c,   // 0x4a, J 
   0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11,   // 0x4b, K 
   0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f,   // 0x4c, L 
   0x11, 0x1b, 0x15, 0x11, 0x11, 0x11, 0x11,   // 0x4d, M 
   0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11,   // 0x4e, N 
   0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e,   // 0x4f, O 
   0x1e, 0x11, 0x11, 0x1e, 0x10, 0x10, 0x10,   // 0x50, P 
   0x0e, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0d,   // 0x51, Q 
   0x1e, 0x11, 0x11, 0x1e, 0x14, 0x12, 0x11,   // 0x52, R 
   0x0e, 0x11, 0x10, 0x0e, 0x01, 0x11, 0x0e,   // 0x53, S 
   0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,   // 0x54, T 
   0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e,   // 0x55, U 
   0x11, 0x11, 0x11, 0x11, 0x11, 0x0a, 0x04,   // 0x56, V 
   0x11, 0x11, 0x11, 0x15, 0x15, 0x1b, 0x11,   // 0x57, W 
   0x11, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x11,   // 0x58, X 
   0x11, 0x11, 0x0a, 0x04, 0x04, 0x04, 0x04,   // 0x59, Y 
   0x1f, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1f,   // 0x5a, Z 
   0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0e,   // 0x5b, [ 
   0x10, 0x10, 0x08, 0x04, 0x02, 0x01, 0x01,   // 0x5c, \ 
   0x0e, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0e,   // 0x5d, ] 
   0x04, 0x0a, 0x11, 0x00, 0x00, 0x00, 0x00,   // 0x5e, ^ 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f,   // 0x5f, _ 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f,   // 0x5f, _ 
   0x04, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00,   // 0x60, ` 
   0x00, 0x0e, 0x01, 0x0d, 0x13, 0x13, 0x0d,   // 0x61, a 
   0x10, 0x10, 0x10, 0x1c, 0x12, 0x12, 0x1c,   // 0x62, b 
   0x00, 0x00, 0x00, 0x0e, 0x10, 0x10, 0x0e,   // 0x63, c 
   0x01, 0x01, 0x01, 0x07, 0x09, 0x09, 0x07,   // 0x64, d 
   0x00, 0x00, 0x0e, 0x11, 0x1f, 0x10, 0x0f,   // 0x65, e 
   0x06, 0x09, 0x08, 0x1c, 0x08, 0x08, 0x08,   // 0x66, f 
   0x0e, 0x11, 0x13, 0x0d, 0x01, 0x01, 0x0e,   // 0x67, g 
   0x10, 0x10, 0x10, 0x16, 0x19, 0x11, 0x11,   // 0x68, h 
   0x00, 0x04, 0x00, 0x0c, 0x04, 0x04, 0x0e,   // 0x69, i 
   0x02, 0x00, 0x06, 0x02, 0x02, 0x12, 0x0c,   // 0x6a, j 
   0x10, 0x10, 0x12, 0x14, 0x18, 0x14, 0x12,   // 0x6b, k 
   0x0c, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,   // 0x6c, l 
   0x00, 0x00, 0x0a, 0x15, 0x15, 0x11, 0x11,   // 0x6d, m 
   0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11,   // 0x6e, n 
   0x00, 0x00, 0x0e, 0x11, 0x11, 0x11, 0x0e,   // 0x6f, o 
   0x00, 0x1c, 0x12, 0x12, 0x1c, 0x10, 0x10,   // 0x70, p 
   0x00, 0x07, 0x09, 0x09, 0x07, 0x01, 0x01,   // 0x71, q 
   0x00, 0x00, 0x16, 0x19, 0x10, 0x10, 0x10,   // 0x72, r 
   0x00, 0x00, 0x0f, 0x10, 0x0e, 0x01, 0x1e,   // 0x73, s 
   0x08, 0x08, 0x1c, 0x08, 0x08, 0x09, 0x06,   // 0x74, t 
   0x00, 0x00, 0x11, 0x11, 0x11, 0x13, 0x0d,   // 0x75, u 
   0x00, 0x00, 0x11, 0x11, 0x11, 0x0a, 0x04,   // 0x76, v 
   0x00, 0x00, 0x11, 0x11, 0x15, 0x15, 0x0a,   // 0x77, w 
   0x00, 0x00, 0x11, 0x0a, 0x04, 0x0a, 0x11,   // 0x78, x 
   0x00, 0x11, 0x11, 0x0f, 0x01, 0x11, 0x0e,   // 0x79, y 
   0x00, 0x00, 0x1f, 0x02, 0x04, 0x08, 0x1f,   // 0x7a, z 
   0x06, 0x08, 0x08, 0x10, 0x08, 0x08, 0x06,   // 0x7b, { 
   0x04, 0x04, 0x04, 0x00, 0x04, 0x04, 0x04,   // 0x7c, | 
   0x0c, 0x02, 0x02, 0x01, 0x02, 0x02, 0x0c,   // 0x7d, } 
   0x08, 0x15, 0x02, 0x00, 0x00, 0x00, 0x00,   // 0x7e, ~ 
   0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,   // 0x7f, DEL 
   };   
   
void read_command_from_serial() 
{
  String command;
  if(Serial.available() > 0) 
  {
     command = Serial.readStringUntil('~');
     //012345678901234567890
     //T12:00:00-01-02-16E
     if (command.length() >= 18)
     {
        // process the commmand
        if ((command[0]!='T'))
        {
          process_message(1,command);
          return;
        }
        int vals[6]={0,0,0,0,0,0};
        for (int i=0; i<6; i++)
        {
          int v = (i*3) + 1; 
          if ((command[v]<'0') || (command[v]>'9') || (command[v+1]<'0') || (command[v+1]>'9'))
          {
            process_message(2,command);
            return;
          }
          vals[i]=command[v]-'0';
          vals[i]*=10; 
          vals[i]+=command[v+1]-'0';
        }
        if (( vals[0]<0) || (vals[0] > 23))
        {
            process_message(3,command);
            return;
        }
        if (( vals[1]<0) || (vals[1] > 59))
        {
            process_message(4,command);
            return;
        }
        if (( vals[2]<0) || (vals[2] > 59))
        {
            process_message(5,command);
            return;
        }
        if (( vals[3]<0) || (vals[3] > 31))
        {
            process_message(6,command);
            return;
        }
        if (( vals[4]<1) || (vals[4] > 12))
        {
            process_message(7,command);
            return;
        }
        if (( vals[5]< 16) || (vals[5] > 99))
        {
            process_message(8,command);
            return;
        }
        Serial.print("Processing date and time ");
        Serial.print(vals[0]); Serial.print(":");
        Serial.print(vals[1]); Serial.print(":");
        Serial.print(vals[2]); Serial.print(" ");
        Serial.print(vals[3]); Serial.print("/");
        Serial.print(vals[4]); Serial.print("/20");
        Serial.println(vals[5]); 
        rtc.stopRTC(); //stop the RTC
        rtc.setTime(vals[0],vals[1],vals[2]); 
        rtc.setDate(vals[3],vals[4],2000+vals[5]); 
        rtc.startRTC(); //start the RTC
     }
     else
     {
       if (command.length() > 0)
       {
            process_message(9,command);
       }
     }
     
  }
}

// Converts a color from HSV to RGB.
// h is hue, as a number between 0 and 360.
// s is the saturation, as a number between 0 and 255.
// v is the value, as a number between 0 and 255.
rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return (rgb_color){r, g, b};
}

rgb_color get_word_color(int w)
{
  uint16_t t = millis() >> 8;
  t += w * 137;
  return hsvToRgb(t % 360, 250, 40);
}

void add_word(ewords w)
{
  words_ids[num_words]=w;
  num_words++;
}

void get_time_in_words(void)
{
  num_words = 0;
  add_word(wrd_its);

  int m = rtc.getMinutes();
  int h = rtc.getHours();
  int md=(m % 5);
  switch (md)
  {
  case 0:
      break;
  case 1:
      add_word(wrd_just);
  case 2:
      add_word(wrd_gone);
      break;
  case 3:
      add_word(wrd_near);
      m=m+(5-(m % 5));
      break;
  case 4:
      add_word(wrd_nearly);
      m=m+(5-(m % 5));
      break;
  }


  switch (m)
  {
  case 0:
      add_word(wrd_oclock);
      break;
  case 1: case 2: case 3: case 4:
      break;
  case 5: case 6: case 7: case 8: case 9:
  case 55: case 56: case 57: case 58: case 59:
      add_word(wrd_five);
      break;
  case 10: case 11: case 12: case 13: case 14:
  case 50: case 51: case 52: case 53: case 54:
      add_word(wrd_ten);
      break;
  case 15: case 45:
        add_word(wrd_a);
  case 16: case 17: case 18: case 19:
  case 46: case 47: case 48: case 49:
      add_word(wrd_quarter);
      break;
  case 20: case 21: case 22: case 23: case 24:
  case 40: case 41: case 42: case 43: case 44:
      add_word(wrd_twenty);
      break;
  case 25: case 26: case 27: case 28:  case 29:
  case 35: case 36: case 37: case 38: case 39:
      add_word(wrd_twenty);
      add_word(wrd_five);
      break;
  case 30: 
        add_word(wrd_a);
  case 31: case 32: case 33: case 34:
      add_word(wrd_half);
      break;
  }

  if ((m >= 5 && m <= 14) || (m >= 20 && m <= 29)) //Minutes Past
  {
      add_word(wrd_minutes);
      add_word(wrd_past);
  }
  else if (m >= 15 && m <= 19) //Past
  {
      add_word(wrd_past);
  }
  else if (m >= 30 && m <= 34) //Half Past
  {
      add_word(wrd_past);
  }
  else if ((m >= 35 && m <= 44) || (m >= 50 && m <= 59)) //Minutes To
  {
      add_word(wrd_minutes);
      add_word(wrd_to);
  }
  else if (m >= 45 && m <= 49) //To
  {
      add_word(wrd_to);
  }

  if (m>34)
      h++;

  switch (h)
  {
  case 0:
      add_word(wrd_htwelve);
      add_word(wrd_midnight);
      break;
  case 1:
      add_word(wrd_hone);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_morning);
      break;
  case 2:
      add_word(wrd_htwo);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_morning);
      break;
  case 3:
      add_word(wrd_hthree);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_morning);
      break;
  case 4:
      add_word(wrd_hfour);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_morning);
      break;
  case 5:
      add_word(wrd_hfive);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_morning);
      break;
  case 6:
      add_word(wrd_hsix);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_morning);
      break;
  case 7:
      add_word(wrd_hseven);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_morning);
      break;
  case 8:
      add_word(wrd_height);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_morning);
      break;
  case 9:
      add_word(wrd_hnine);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_morning);
      break;
  case 10:
      add_word(wrd_hten);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_morning);
      break;
  case 11:
      add_word(wrd_heleven);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_morning);
      break;
  case 12:
      add_word(wrd_htwelve);
      add_word(wrd_midday);
      break;
  case 13:
      add_word(wrd_hone);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_afternoon);
      break;
  case 14:
      add_word(wrd_htwo);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_afternoon);
      break;
  case 15:
      add_word(wrd_hthree);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_afternoon);
      break;
  case 16:
      add_word(wrd_hfour);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_afternoon);
      break;
  case 17:
      add_word(wrd_hfive);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_evening);
      break;
  case 18:
      add_word(wrd_hsix);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_evening);
      break;
  case 19:
      add_word(wrd_hseven);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_evening);
      break;
  case 20:
      add_word(wrd_height);
      add_word(wrd_in); add_word(wrd_the);
      add_word(wrd_evening);
      break;
  case 21:
      add_word(wrd_hnine);
      add_word(wrd_at); 
      add_word(wrd_night);
      break;
  case 22:
      add_word(wrd_hten);
      add_word(wrd_at); 
      add_word(wrd_night);
      break;
  case 23:
      add_word(wrd_heleven);
      add_word(wrd_at); 
      add_word(wrd_night);
      break;
  }
  add_word(wrd_today); add_word(wrd_is);

  switch (rtc.getWeekDay()){
    case 0:
      add_word(wrd_sunday);
      break;
    case 1:
      add_word(wrd_monday);
      break;
    case 2:
      add_word(wrd_tuesday);
      break;
    case 3:
      add_word(wrd_wednesday);
      break;
    case 4:
      add_word(wrd_thursday);
      break;
    case 5:
      add_word(wrd_friday);
      break;
    case 6:
      add_word(wrd_saturday);
      break;
  }
}

void clear_down_all_colors(void)
{
  for(int i = 0; i < LED_COUNT; i++)
  {
    colors[i] = (rgb_color){0,0,0};
  }
}

void clear_down_bottom_colors(void)
{
  for(int i = 0; i < 112; i++)
  {
    colors[i] = (rgb_color){0,0,0};
  }
  colors[125]= (rgb_color){0,0,0};
  colors[126]= (rgb_color){0,0,0};
}


void word_time_to_colour_buffer(void)
{
  clear_down_all_colors();
  for (int w = 0; w < num_words; w++)
  {
    rgb_color col = get_word_color(w);
    ewords wrd = words_ids[w];
    int wl = pgm_read_byte_near(words_len + wrd);
    int ws = pgm_read_byte_near(words_st + wrd);
    for(int j = 0; j < wl; j++)
    {
      colors[j+ws] = col;
    }
  }
}

//int testw=0;
//void test_colour_buffer(void)
//{
//  clear_down_all_colors();
//  rgb_color col = get_word_color(testw);
//  for(int j = 0; j < words_len[testw]; j++)
//  {
//    colors[j+words_st[testw]] = col;
//  }
//   testw ++;
//   if (testw>44)
//   {
//    testw=0;
//   }
//}

void set_pixel(int x, int y, rgb_color col)
{
   if ((x < 0) || ( x > 15))
   {
    return;
   }
   if ((y < 0) || ( y > 14))
   {
    return;
   }
   int rx = 15-x;
   int ry = 14-y;
   int pxidx;
   if (ry % 2 == 0)
   {
     pxidx = rx+(ry*16);
   }
   else
   {
     pxidx = (16*(ry+1))-rx-1;
   }
   colors[pxidx]=col;
}

void add_5x7_char_to_color_buffer(int xp, int yp, uint16_t c, rgb_color col)
{
  if (( c < 0x20) || (c > 0x7f))  { c=0x20;  }
  c = (c -0x20) * 7;
  for (uint16_t y=0;y<7;y++)
  {
    char g = pgm_read_byte_near(font5x7 + (c+y));
    for(int x=0;x<6;x++)
    {
      //g &= (1<<(5-x));
      if (g & (1<<(5-x)))
      {
        set_pixel(x+xp,y+yp,col);
      }
    }
  }
}

void add_5x7_num_to_color_buffer(int xp, int yp, int c, rgb_color col)
{
  add_5x7_char_to_color_buffer(xp,yp,c+0x30,col);
}

void scroll_message_on_display_single_col(String message, rgb_color col)
{
  message = "  " + message + "   ";
  int clen = message.length() -3 ;

  col = (rgb_color){30,30,30};
  for (int ch = 0; ch < clen; ch++)
  {
      for (int x = 4; x >= 0; x--)
      {
        clear_down_bottom_colors();
        add_5x7_char_to_color_buffer(x-6,8,message[ch],col);
        add_5x7_char_to_color_buffer(x,8,message[ch+1],col);
        add_5x7_char_to_color_buffer(x+6,8,message[ch+2],col);
        add_5x7_char_to_color_buffer(x+12,8,message[ch+3],col);
        ledStrip.write(colors, LED_COUNT);  
        delay(25);
      }
  }
}

void process_message(int dbg, String message)
{
  Serial.print("Recieved scrolling message {");
  Serial.print(message);
  Serial.print("}");
  Serial.println(dbg);

  scroll_message_on_display_single_col(message,get_word_color(1));
}

void setup()
{
  rtc.stopRTC(); //stop the RTC
  rtc.setTime(11,06,0); //set the time here
  rtc.setDate(21,2,2017); //set the date here
  rtc.startRTC(); //start the RTC
  
  Serial.begin(19200); //choose the serial speed here
  Serial.setTimeout(500);
  
  Serial.println("Ready to receive time or messages."); 
  Serial.println("Time format is T12:00:00-01-02-2016~"); 
}

float gh=0.0;
float gt=0.0;
bool read_temp = false;
bool disp_temp = false;
bool disp_date = false;


void temperature_to_colour_buffer(void)
{
  char str_buff[20];
  sprintf(str_buff, "Temp%dC RH%d%%",(int)gt,(int)gh);  
  scroll_message_on_display_single_col(str_buff,get_word_color(1));
}

void date_to_colour_buffer(void)
{
 char str_buff[128];
 char str_month[10];
 char str_day[10];
 strcpy_P(str_month, (char*)pgm_read_word(&(month_table[rtc.getMonth()-1])));
 strcpy_P(str_day, (char*)pgm_read_word(&(day_table[rtc.getWeekDay()])));
 int d = rtc.getDay();
 switch (d % 10)
 {
  case 1:
    sprintf(str_buff,"%s %dst %s %d %02d:%02d", str_day, d, str_month, rtc.getYear(), rtc.getHours(), rtc.getMinutes());  
    break;
  case 2:
    sprintf(str_buff,"%s %dnd %s %d %02d:%02d", str_day, d, str_month, rtc.getYear(), rtc.getHours(), rtc.getMinutes());  
    break;
  case 3:
    sprintf(str_buff,"%s %drd %s %d %02d:%02d", str_day, d, str_month, rtc.getYear(), rtc.getHours(), rtc.getMinutes());  
    break;
  default:
    sprintf(str_buff,"%s %dth %s %d %02d:%02d", str_day, d, str_month, rtc.getYear(), rtc.getHours(), rtc.getMinutes());  
    break;
 }
                  
  scroll_message_on_display_single_col(str_buff,get_word_color(1));
}

 //strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i]))); // Necessary casts and dereferencing, just copy.

void random_to_colour_buffer(void)
{
  //clear_down_all_colors();
  for(int i=0;i<10;i++)
  {
      colors[random(240)]=(rgb_color){0,0,0};
  }
  for(int i=0;i<5;i++)
  {
      colors[random(240)]=hsvToRgb(random(360), 250, 40);
  }
  ledStrip.write(colors, LED_COUNT);  
}

void loop()
{
  // If any digit is received, we will go into integer parsing mode
  // until all three calls to parseInt return an interger or time out.
  read_command_from_serial();

  int secs =rtc.getSeconds();
  if (secs == 3)
  {
    read_temp = true;
  }
  if ((secs == 4) && (read_temp==true))
  {
    gh = dht.readHumidity();
    // Read temperature as Celsius (the default)
    gt = dht.readTemperature();
    read_temp = false;
    disp_temp = true;
  }
  if (secs == 34)
  {
    disp_date = true;
  }

  if ((secs == 5) && (disp_temp == true))
  {
    disp_temp=false;
    temperature_to_colour_buffer();
  }
  else if ((secs == 35) && (disp_date == true))
  {
    disp_date = false;
    date_to_colour_buffer();
  }
  else if (secs >= 59)
  {
    random_to_colour_buffer();
  }
  else
  {
    get_time_in_words();
    word_time_to_colour_buffer();
  }
  //test_colour_buffer();
  // Write to the LED strip.
  ledStrip.write(colors, LED_COUNT);  

//  Serial.print(rtc.getHours(), DEC);
//  Serial.print(":");
//  Serial.print(rtc.getMinutes(), DEC);
//  Serial.print(":");
//  Serial.print(rtc.getSeconds(), DEC);
//  Serial.print(" -- ");
//  Serial.print(rtc.getDay(), DEC);
//  Serial.print("/");
//  Serial.print(rtc.getMonth(), DEC);
//  Serial.print("/");
//  Serial.print(rtc.getYear(), DEC);
//  Serial.print("  Day of week: ");
//  Serial.println(rtc.getWeekDay(), DEC);

  delay (100);
}


