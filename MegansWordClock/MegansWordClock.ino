/* 
 *  Megans word clock
 */
 
#include <PololuLedStrip.h>
#include <swRTC.h>

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

int words_st[]  = { 236 , 234 , 229 , 224 , 208 , 208 , 215 , 192 , 218 , 204 , 200 , 
                    176 , 189 , 184 , 173 , 164 , 128 , 156 , 147 , 144 , 132 , 
                    169 , 160 , 113 , 150 , 138 , 117 , 125 , 107 , 73  , 
                    80  , 64  , 97  , 88  , 48  , 54  , 32  , 42  , 57  , 17  , 
                    26  , 10  , 1   , 110 , 64};       
              
int words_len[] = { 4   , 1   , 4   , 4   , 4   , 6   , 3   , 7   , 6   , 4   , 4   ,
                    7   , 2   , 4   , 3   , 5   , 3   , 4   , 4   , 3   , 5   , 
                    5   , 4   , 3   , 6   , 6   , 7   , 2   , 3   , 7   ,
                    7   , 8   , 9   , 6   , 5   , 2   , 9   , 6   , 7   , 8   ,
                    6   , 6   , 8   , 2   , 5};       

int num_words = 0;
ewords words_ids[20];

void process_message(int dbg, String message)
{
  Serial.print("Recieved scrolling message {");
  Serial.print(message);
  Serial.print("}");
  Serial.println(dbg);
}

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
  t += w * 155;
  return hsvToRgb(t % 360, 255, 80);
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
  case 15: case 16: case 17: case 18: case 19:
  case 45: case 46: case 47: case 48: case 49:
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
  case 30: case 31: case 32: case 33: case 34:
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

void update_colour_buffer(void)
{
  for(int i = 0; i < LED_COUNT; i++)
  {
    colors[i] = (rgb_color){0,0,0};
    
  }
  for (int w = 0; w < num_words; w++)
  {
    rgb_color col = get_word_color(w);
    ewords wrd = words_ids[w];
    for(int j = 0; j < words_len[wrd]; j++)
    {
      colors[j+words_st[wrd]] = col;
    }
  }
}

int testw=0;
void test_colour_buffer(void)
{
  for(int i = 0; i < LED_COUNT; i++)
  {
    colors[i] = (rgb_color){0,0,0};
    
  }
  rgb_color col = get_word_color(testw);
  for(int j = 0; j < words_len[testw]; j++)
  {
    colors[j+words_st[testw]] = col;
  }
   testw ++;
   if (testw>44)
   {
    testw=0;
   }
}

void loop()
{
  // If any digit is received, we will go into integer parsing mode
  // until all three calls to parseInt return an interger or time out.
  read_command_from_serial();

  get_time_in_words();
  update_colour_buffer();
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

void setup()
{
  rtc.stopRTC(); //stop the RTC
  rtc.setTime(13,10,0); //set the time here
  rtc.setDate(20,2,2017); //set the date here
  rtc.startRTC(); //start the RTC
  
  Serial.begin(19200); //choose the serial speed here
  Serial.setTimeout(250);
  
  Serial.println("Ready to receive time or messages."); 
  Serial.println("Time format is T12:00:00-01-02-2016~"); 
}
