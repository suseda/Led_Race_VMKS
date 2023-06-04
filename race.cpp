#include <Adafruit_NeoPixel.h>
#define MAXLED         115

 
#define PIN_LED        A0    
#define PIN_P1         7   
#define PIN_P2         6    
#define PIN_AUDIO      3   


int NPIXELS=MAXLED; 

#define COLOR1    track.Color(255,0,0)
#define COLOR2    track.Color(0,255,0)

int win_music[] = {
  2637, 2637, 0, 2637, 
  0, 2093, 2637, 0,
  3136    
};
      
byte  gravity_map[MAXLED];     

int TBEEP=3; 

float speed1=0;
float speed2=0;
float dist1=0;
float dist2=0;

byte currLapP1 = 0;
byte currLapP2 = 0;
unsigned long lapStartP1 = 0;
unsigned long lapStartP2 = 0;
bool firstLap = true;

byte loop1=0;
byte loop2=0;

byte leader=0;
byte loop_max=6;


float ACEL=0.2;
float kf=0.015; 
float kg=0.003; 

byte flag_sw1=0;
byte flag_sw2=0;
byte draworder=0;
 
unsigned long timestamp=0;

Adafruit_NeoPixel track = Adafruit_NeoPixel(MAXLED, PIN_LED, NEO_GRB + NEO_KHZ800);

int tdelay = 5; 

void set_ramp(byte H,byte a,byte b,byte c)
{
  for( int i = 0 ; i < ( b - a ) ; i++)
  {
    gravity_map[a+i] = 127-i*((float)H/(b-a));
  };

  gravity_map[b] = 127; 
  
  for( int i = 0 ; i < ( c - b ) ; i++)
  { 
    gravity_map[b+i+1] = 127+H-i*((float)H/(c-b));
  };
}

void set_loop(byte H,byte a,byte b,byte c)
{
  for( int i = 0 ; i < ( b - a ) ; i++)
  {
    gravity_map[a+i] = 127-i*((float)H/(b-a));
  };
  gravity_map[b]=255; 
  for( int i = 0 ; i < ( c - b ) ; i++)
  { 
    gravity_map[b+i+1] = 127+H-i*((float)H/(c-b));
  };
}


void setup() 
{
  for( int i = 0 ; i < NPIXELS ; i++)
  { 
    gravity_map[i] = 127;
  };

  track.begin(); 
  pinMode(PIN_P1,INPUT_PULLUP); 
  pinMode(PIN_P2,INPUT_PULLUP);  

  if ((digitalRead(PIN_P1) == 0)) 
  {
    set_ramp(12,90,100,110);     
    for( int i = 0 ; i < NPIXELS ; i++)
    {
      track.setPixelColor(i, track.Color(0,0,(127-gravity_map[i])/8) );
    };
    track.show();
  };
  Serial.begin(9600);
  start_race();    
}

void start_race()
{
  for( int i = 0 ; i < NPIXELS ; i++)
  { 
    track.setPixelColor(i, track.Color(0,0,0)); 
  };

  track.show();
  delay(2000);
  track.setPixelColor(8, track.Color(255,0,0));
  track.setPixelColor(7, track.Color(255,0,0));
  track.show();
  tone(PIN_AUDIO,400);
  delay(2000);
  noTone(PIN_AUDIO);                  
  track.setPixelColor(8, track.Color(0,0,0));
  track.setPixelColor(7, track.Color(0,0,0));
  track.setPixelColor(10, track.Color(255,255,0));
  track.setPixelColor(9, track.Color(255,255,0));
  track.show();
  tone(PIN_AUDIO,600);
  delay(2000);
  noTone(PIN_AUDIO);                  
  track.setPixelColor(9, track.Color(0,0,0));
  track.setPixelColor(10, track.Color(0,0,0));
  track.setPixelColor(12, track.Color(0,255,0));
  track.setPixelColor(11, track.Color(0,255,0));
  track.show();
  tone(PIN_AUDIO,1200);
  delay(2000);
  noTone(PIN_AUDIO);                               
  timestamp=0;              
};

void winner_fx() 
{
  int msize = sizeof(win_music) / sizeof(int);
  for (int note = 0; note < msize; note++) 
  {
  tone(PIN_AUDIO, win_music[note],200);
  delay(230);
  noTone(PIN_AUDIO);
  }
};


void draw_car1(void)
{
  for( int i = 0 ; i <= loop1 ; i++)
  {
    track.setPixelColor(((word)dist1 % NPIXELS)+i, track.Color(0,0,255-i*20));
  };                   
}

void draw_car2(void)
{ 
  for(int i = 0 ; i <= loop2 ; i++)
  {
    track.setPixelColor(((word)dist2 % NPIXELS)+i, track.Color(255-i*20,0,0));
  };            
}
  
void loop() {
    if(firstLap == true)
    {
      lapStartP1 = millis();
      lapStartP2 = millis();
      firstLap = false;
    } 

    if(loop1 != currLapP1)
    {
        lapStartP1 = millis();
        currLapP1 = loop1;
    }

    if(loop2 != currLapP2)
    {
        lapStartP2 = millis();
        currLapP2 = loop2;
    }

    for( int i = 0 ; i < NPIXELS ; i++)
    {
      track.setPixelColor(i, track.Color(0,0,(127-gravity_map[i])/8) );
    };
    
    if ( (flag_sw1 == 1) && (digitalRead(PIN_P1) == 0) ) 
    {
      flag_sw1=0;
      speed1+=ACEL;
    };

    if ( (flag_sw1 == 0) && (digitalRead(PIN_P1) == 1) ) 
      flag_sw1=1;

    if ((gravity_map[(word)dist1 % NPIXELS]) < 127) 
      speed1-=kg*(127-(gravity_map[(word)dist1 % NPIXELS]));
    
    if ((gravity_map[(word)dist1 % NPIXELS]) > 127) 
      speed1+=kg*((gravity_map[(word)dist1 % NPIXELS])-127);
    
    
    speed1-=speed1*kf; 
    
    if ( (flag_sw2 == 1) && (digitalRead(PIN_P2) == 0) ) 
    {
      flag_sw2=0;
      speed2+=ACEL;
    };

    if ( (flag_sw2 == 0) && (digitalRead(PIN_P2) == 1) ) 
      flag_sw2=1;

    if ((gravity_map[(word)dist2 % NPIXELS]) < 127) 
      speed2-=kg*(127-(gravity_map[(word)dist2 % NPIXELS]));
    
    if ((gravity_map[(word)dist2 % NPIXELS]) > 127) 
      speed2+=kg*((gravity_map[(word)dist2 % NPIXELS])-127);
        
    speed2-=speed2*kf; 
        
    dist1+=speed1;
    dist2+=speed2;

    if (dist1 > dist2) 
      leader=1; 
    else
      leader=2;
      
    if (dist1 > NPIXELS*loop1) 
    {
      loop1++;
      Serial.print("Lap ");
      Serial.print(loop1);
      Serial.print(" P1: ");
      Serial.print(millis() - lapStartP1);
      Serial.println(" ms");
      tone(PIN_AUDIO,600);
      TBEEP=2;
    };

    if (dist2 > NPIXELS*loop2) 
    {
      loop2++;
      Serial.print("Lap ");
      Serial.print(loop2);
      Serial.print(" P2: ");
      Serial.print(millis() - lapStartP2);
      Serial.println(" ms");
      tone(PIN_AUDIO,700);
      TBEEP=2;
    };

    if (loop1 > loop_max) 
    {
      for( int i = 0 ; i < NPIXELS ; i++)
      {
        track.setPixelColor(i, track.Color(0,0,255));
      }; 
      track.show();
      winner_fx();
      loop1=0;
      loop2=0;
      dist1=0;
      dist2=0;
      speed1=0;
      speed2=0;
      timestamp=0;
      start_race();
    }

    if (loop2 > loop_max) 
    {
      for( int i = 0 ; i < NPIXELS ; i++)
      {
        track.setPixelColor(i, track.Color(255,0,0));
      }; 
      track.show();
      winner_fx();
      loop1=0;
      loop2=0;
      dist1=0;
      dist2=0;
      speed1=0;
      speed2=0;
      timestamp=0;
      start_race();
    }
    
    if ((millis() & 512) == (512 * draworder)) 
    {
      if (draworder == 0) 
        draworder=1;
      else 
        draworder=0;   
    }; 

    if (draworder == 0) 
    {
      draw_car1();
      draw_car2();
    }
    else 
    {
      draw_car2();
      draw_car1();
    }   
                 
    track.show(); 
    delay(tdelay);
    
    if (TBEEP > 0) 
    {
      TBEEP-=1; 
      if (TBEEP==0) 
      {
        noTone(PIN_AUDIO);
      }; 
    };   
}
