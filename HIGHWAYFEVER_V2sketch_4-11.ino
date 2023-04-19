//Move your hand up and down over the LIDAR  to move the your car left and right to avoid the slower cars on the road.

#include <Elegoo_TFTLCD.h> 
#include <Elegoo_GFX.h>
#include <TouchScreen.h>

#include <Wire.h>
#include <VL53L1X.h>

VL53L1X sensor;
 
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0x8410
#define ORANGE  0xFD00

#define TS_MINX 120
#define TS_MINY 80
#define TS_MAXX 920
#define TS_MAXY 905

#define XP 8
#define YP A3
#define XM A2
#define YM 9

Elegoo_TFTLCD tft(A3, A2, A1, A0, A4); // (LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET)
TouchScreen ts = TouchScreen(XP,YP,XM,YM,364);

int soundPin = 49;

int Xc = -20;                                                                  //Obsticle x position
int Y1c = 20+(80*random(3));                                                   //Obsticle 1 y position
int Y2c = 20+(80*random(3));                                                   //Obsticle 2 y position
int Yp = 0;                                                                    //Player y poition
int pastYp = 0;                                                                //Player y poition before current frame
int points = 0;                                                                //Game points
int hiscore = 0;                                                               //High score
int Speed = 0;                                                                 //Obsticle speed
int pastPts = 0;                                                               //Points before current frame
int pointDif = 0;                                                              //Points - past points
int Hex[7][6] = {{0x001F,0x07E0,0x07FF,0xF81F,0xFFE0,0xFD00},   //RED          //Obsticle car colors hexadecimal
                 {0x001F,0xF800,0x07E0,0x07FF,0xF81F,0xFFE0},   //ORANGE
                 {0x001F,0xF800,0x07E0,0x07FF,0xF81F,0xFD00},   //YELLOW
                 {0x001F,0xF800,0x07FF,0xF81F,0xFFE0,0xFD00},   //GREEN
                 {0x001F,0xF800,0x07E0,0xF81F,0xFFE0,0xFD00},   //CYAN
                 {0xF800,0x07E0,0x07FF,0xF81F,0xFFE0,0xFD00},   //BLUE
                 {0x001F,0xF800,0x07E0,0x07FF,0xFFE0,0xFD00}};  //MAGENTA
int xCircle[7] = {110,160,210,85,135,185,235};                                 //Settings circle x position
int yCircle[7] = {160,160,160,210,210,210,210};                                //Settings circle y position
int Color1 = 0;                                                                //Obsticle 1 color
int Color2 = 0;                                                                //Obsticle 2 color
int pColor[7] = {0xF800,0xFD00,0xFFE0,0x07E0,0x07FF,0x001F,0xF81F};            //Player color hexadecimal
String cList[7] = {"RED","ORANGE","YELLOW","GREEN","CYAN","BLUE","MAGENTA"};   //Player color words
int Start = 0;                                                                 //Game mode
int choice = 0;                                                                //Button choice
int Setting = 0;                                                               //Color setting
int Sound = 1;                                                                 //Game sound

void setup() {
  
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

  tft.reset();
  tft.begin(0x9341); // LCD driver identifier
  tft.setRotation(1);
  tft.fillScreen(BLACK);

  sensor.setTimeout(500);
  if (!sensor.init())
  {
    displayText(50, 100, WHITE, 2, "Sensor Not Detected");
    while (1);
  }
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000);
  sensor.startContinuous(50);

  pinMode(soundPin,OUTPUT);

  //tft.fillRoundRect(300,20,80,50,10,BLUE);
  //tft.fillRoundRect(300,100,80,50,10,BLUE);
  //tft.fillRoundRect(300,180,80,50,10,BLUE);
}

void loop() {

  //Start
  while(Start == 0){
    choice = 0;
    tft.fillScreen(BLACK);
    displayText(30, 20, RED, 6, "Highway");
    displayText(80, 75, CYAN, 6, "Fever");
    tft.drawRoundRect(70,130,180,100,20,WHITE);
    displayText(120, 135, WHITE, 2, "Tap to");
    displayText(80, 160, WHITE, 7, "Play");

    //Settings Icon
    tft.drawCircle(280,130,25,WHITE);
    tft.fillCircle(280,130,9,WHITE);
    tft.drawLine(280,118,280,142,WHITE);
    tft.drawLine(268,130,292,130,WHITE);
    tft.drawLine(272,122,288,138,WHITE);
    tft.drawLine(272,138,288,122,WHITE);
    tft.fillCircle(280,130,4,BLACK);

    //Instructions Icon
    displayText(33, 120, WHITE, 3, "?");
    tft.drawCircle(40,130,25,WHITE);

  for(int Time = 0; ++Time;){
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
    
  if (Time%400 == 0){
    displayText(30, 20, CYAN, 6, "Highway");
    displayText(80, 75, RED, 6, "Fever");    
    }
  if (Time%500 == 0){
    tft.fillRoundRect(70,130,180,100,20,WHITE);
    displayText(120, 135, BLACK, 2, "Tap to");
    displayText(80, 160, BLACK, 7, "Play");    
  }
  if (Time%800 == 0){
    displayText(30, 20, RED, 6, "Highway");
    displayText(80, 75, CYAN, 6, "Fever");
    }
  if (Time%1000 == 0){
    tft.fillRect(70,130,180,100,BLACK);
    tft.drawRoundRect(70,130,180,100,20,WHITE);
    displayText(120, 135, WHITE, 2, "Tap to");
    displayText(80, 160, WHITE, 7, "Play");
  }
  
  if (p.z > ts.pressureThreshhold){
    int p_x = map(p.y,TS_MINY,TS_MAXY,0,320);
    int p_y = map(p.x,TS_MINX,TS_MAXX,0,240);

      //Play Button
    if(p_x>65 && p_y>10 && p_x<250 && p_y<100){
      break;
       }
      //Settings Button
    if(p_x>10 && p_y>75 && p_x<60 && p_y<125){
      choice = 1;
      break;
       }
      //Instructions Button
    if(p_x>255 && p_y>75 && p_x<305 && p_y<125){
      choice = 2;
      break;  
       }   
     } 
   }
   if(choice == 1){
     Start = 4;
    }
   else if(choice == 2){
     Start = 5;
    }
   else{  
     startPlay();
    }
  }
   
  //Play
  while(Start == 1){
  sensor.read();
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if(p.z == 0){
    pastPts = points;

       //Levels
      if(points%10 == 0 && points <= 100 && points != 0 && pointDif == 1){
        tft.fillRect(230,217,80,20,BLACK);
        Speed += 3;
        }
        
      //Player Car      
      playerCar();
    
      //Obstacle Cars
      Xc += (24+Speed);
      if(Xc<=360){
         tft.fillRect(300-Xc,Y1c,110+Speed,50,BLACK);
         tft.fillRect(300-Xc,Y2c,110+Speed,50,BLACK);
         tft.fillRoundRect(300-Xc,Y1c,80,50,10,Color1);
         tft.fillRoundRect(300-Xc,Y2c,80,50,10,Color2);  
      }
       
      else{
        tft.fillRect(300-Xc,Y1c,140,50,BLACK);
        tft.fillRect(300-Xc,Y2c,140,50,BLACK);
        tft.fillRect(260,7,60,20,BLACK);
        Xc -= 360;
        Y1c = 20+(80*random(3));
        Y2c = 20+(80*random(3));
        carColors(random(6), random(6));
        points += 1;
        }
     
       //Point Display
      displayText(180, 10, WHITE, 2, "Points:" + String(points));
      displayText(160, 220, WHITE, 2, "Speed:" + String(70 + 10 * (Speed/3)) + " MPH");

       //Detect Collision
      if(Xc >= 190){
        if(abs(Yp-Y1c) <= 50 or abs(Yp-Y2c) <= 50){ 
          Start = 3;
          }
        }
        
      pointDif = points - pastPts;
  }
  else{
    displayText(65, 90, WHITE, 6, "Pause");
    analogWrite(soundPin, 255 * (Sound%2));
    delay(100);
    analogWrite(soundPin, LOW);
    delay(400);
    Start = 2;
    }
  }

  //Pause
  while(Start == 2){
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    
     if (p.z > ts.pressureThreshhold){
       analogWrite(soundPin, 255 * (Sound%2));        
       delay(100);
       analogWrite(soundPin, LOW);
       tft.fillScreen(BLACK);
       displayText(135, 90, WHITE, 9, "3");
        for(int Time = 0; Time <= 30; ++Time){
           sensor.read();
           playerCar();
           if(Time == 10){
             tft.fillRect(130,80,80,80,BLACK);
             displayText(135, 90, WHITE, 9, "2");
            }
           if(Time == 20){
             tft.fillRect(130,80,80,80,BLACK);
             displayText(135, 90, WHITE, 9, "1");  
            }   
          }
       tft.fillRect(130,80,80,80,BLACK);
       Start = 1;
       }
    }

  //Game Over
  while(Start == 3){
    choice = 0;
    Xc -= 330;
    Y1c = 20+(80*random(3));
    Y2c = 20+(80*random(3));
    Speed = 0;
    tft.fillScreen(BLACK);
    displayText(30, 60, RED, 5, "Game Over");
    displayText(65, 120, CYAN, 3, "Play Again?");
    if(points > hiscore){
      hiscore = points;
      displayText(65, 180, YELLOW, 2, "New High Score:" + String(hiscore));
       }
    else{
      displayText(100, 180, WHITE, 2, String(points) + " Points");
      displayText(75, 200, WHITE, 2, "High Score:" + String(hiscore));
       }
    points = 0;
    analogWrite(soundPin, 255 * (Sound%2));
    delay(500);
    analogWrite(soundPin, LOW);
    delay(500);
    analogWrite(soundPin, 255 * (Sound%2));
    delay(500);
    analogWrite(soundPin, LOW);
    tft.fillRoundRect(280,10,31,31,5,WHITE);
    displayText(288, 12, BLACK, 3, "x");
    for(int Time = 0; Time <= 30000; ++Time){
      TSPoint p = ts.getPoint();
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);

      if (Time%500 == 0){
        tft.fillRect(60,110,200,40,BLACK);
        }
      if (Time%1000 == 0){
        displayText(65, 120, CYAN, 3, "Play Again?");
        }
      if (p.z > ts.pressureThreshhold){
        int p_x = map(p.y,TS_MINY,TS_MAXY,0,320);
        int p_y = map(p.x,TS_MINX,TS_MAXX,0,240);
        if(p_x>0 && p_y>0 && p_x<320 && p_y<190){
          choice = 1;
          break;
         }
        if(p_x>4 && p_y>200 && p_x<36 && p_y<232){
          break;
         }              
       }
     }
      if(choice == 1){
        startPlay();
      }
      else{
        Start = 0;        
        }     
   }

   //Settings
   while(Start == 4){
     tft.fillScreen(BLACK);
     soundMode();
     tft.fillRoundRect(280,10,31,31,5,WHITE);
     displayText(288, 12, BLACK, 3, "x");
     tft.fillRoundRect(120,70,80,50,10,pColor[Setting]);
     tft.fillCircle(110,160,20,RED);
     tft.fillCircle(160,160,20,ORANGE);
     tft.fillCircle(210,160,20,YELLOW);
     tft.fillCircle(85,210,20,GREEN);
     tft.fillCircle(135,210,20,CYAN);
     tft.fillCircle(185,210,20,BLUE);
     tft.fillCircle(235,210,20,MAGENTA);     
     
     for(int Time = 0; ++Time;){
       TSPoint p = ts.getPoint();
       pinMode(XM, OUTPUT);
       pinMode(YP, OUTPUT);
       tft.fillRoundRect(120,70,80,50,10,pColor[Setting]);
       if(Time%15 == 0){
         tft.drawCircle(xCircle[Setting],yCircle[Setting],20,WHITE);
       }
       if(Time%30 == 0){
         tft.fillCircle(xCircle[Setting],yCircle[Setting],20,pColor[Setting]);
         }
        if (p.z > ts.pressureThreshhold){
         int p_x = map(p.y,TS_MINY,TS_MAXY,0,320);
         int p_y = map(p.x,TS_MINX,TS_MAXX,0,240);
       //Serial.print(p_x); Serial.print("   "); Serial.println(p_y);
         tft.fillCircle(xCircle[Setting],yCircle[Setting],20,pColor[Setting]);
         if(p_x>190 && p_y>55 && p_x<230 && p_y<95){           
           Setting = 0;
          }
         if(p_x>138 && p_y>55 && p_x<178 && p_y<95){
           Setting = 1;
          }          
         if(p_x>86 && p_y>55 && p_x<126 && p_y<95){
           Setting = 2;
          }
         if(p_x>214 && p_y>3 && p_x<254 && p_y<43){
           Setting = 3;
          }
         if(p_x>162 && p_y>3 && p_x<202 && p_y<43){
           Setting = 4;
          }
         if(p_x>110 && p_y>3 && p_x<150 && p_y<43){
           Setting = 5;
          }
         if(p_x>58 && p_y>3 && p_x<98 && p_y<43){
           Setting = 6;
          }
         if(p_x>75 && p_y>197 && p_x<240 && p_y<222){
           Sound += 1;
           soundMode();
          }
         if(p_x>4 && p_y>200 && p_x<36 && p_y<232){
           break;
          }
        }
      }
     Start = 0; 
    }

   //Instructions
    while(Start == 5){
     tft.fillScreen(BLACK);
     tft.fillRoundRect(280,10,31,31,5,WHITE);
     displayText(288, 12, BLACK, 3, "x");
     
     displayText(60,30,WHITE,3,"How to Play");
     displayText(40,80,WHITE,2,"You are the");
     displayText(185,80,pColor[Setting],2,cList[Setting]);
     displayText(10,100,WHITE,2,"car on the left side of");
     displayText(10,120,WHITE,2,"your screen speeding");
     displayText(10,140,WHITE,2,"through highway traffic.");
     displayText(40,160,WHITE,2,"Move your hand up and");
     displayText(10,180,WHITE,2,"down over the LIDAR");
     displayText(10,200,WHITE,2,"sensor to control your");
     displayText(10,220,WHITE,2,"car. Crash = GG.");
     
     for(int Time = 0; ++Time;){
       TSPoint p = ts.getPoint();
       pinMode(XM, OUTPUT);
       pinMode(YP, OUTPUT);
       if (p.z > ts.pressureThreshhold){
         int p_x = map(p.y,TS_MINY,TS_MAXY,0,320);
         int p_y = map(p.x,TS_MINX,TS_MAXX,0,240);
         if(p_x>4 && p_y>200 && p_x<36 && p_y<232){
           break;
          }
        }
     }
    Start = 0;
   }
}

//Functions
void displayText(int x, int y, int color, int size, String a){
  tft.setCursor(x,y);
  tft.setTextColor(color); tft.setTextSize(size);
  tft.println(a);
   }

void carColors(int m, int n){
    if(Y1c == Y2c){
      n = m;
      }
    Color1 = Hex[Setting][m];
    Color2 = Hex[Setting][n];
  }

void startPlay(){
    analogWrite(soundPin, 255 * (Sound%2));
    delay(100);
    analogWrite(soundPin, LOW);
    tft.fillScreen(BLACK);
    carColors(random(6), random(6));
    displayText(135, 90, WHITE, 9, "3");
    delay(900);
    tft.fillRect(130,80,80,80,BLACK);
    displayText(135, 90, WHITE, 9, "2");
    delay(900);
    tft.fillRect(130,80,80,80,BLACK);
    displayText(135, 90, WHITE, 9, "1");
    delay(900);
    tft.fillRect(130,80,80,80,BLACK);
    Start = 1;  
  }

void playerCar(){
    tft.fillRect(20,pastYp,80,50,BLACK);
    if(sensor.ranging_data.range_mm>100 && sensor.ranging_data.range_mm<290){
      Yp = 290-sensor.ranging_data.range_mm;
      }    
    else if(sensor.ranging_data.range_mm<=100){
      Yp = 190;
      }    
    else{
      Yp = 0;
      }

     if(Start == 1){
       tft.fillRoundRect(20,Yp,80,50,10,pColor[Setting]);
       tft.drawRoundRect(20,Yp,80,50,10,WHITE);
       }
     else{
       tft.drawRoundRect(20,Yp,80,50,10,pColor[Setting]);
       }
     pastYp = Yp;  
     }

void soundMode(){
   if(Sound%2 == 0){
     tft.fillRoundRect(80,20,160,25,10,BLACK);
     tft.drawRoundRect(80,20,160,25,10,WHITE);
     displayText(125,25,WHITE,2,"Unmute");
      }
   else{
     tft.fillRoundRect(80,20,160,25,10,WHITE);
     displayText(140,25,GRAY,2,"Mute");
        }
  }
