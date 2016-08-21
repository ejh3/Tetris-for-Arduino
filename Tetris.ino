
/*
===SETUP===
Potentiometer on analog pin A0 to rotate pieces
Potentiometer on analog pin A5 to shift pieces left and right
The rest of the wiring simply follows the diagram given in the tutorial at
https://www.arduino.cc/en/Tutorial/HelloWorld

It is recommended to have two breadboards for ease of control while playing the game
*/

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int rot=0;
int rotPrevious=0;
int timer=0;
int timerPrevious=0;
int xOff=0;
int xOffPrevious=0;
int xOffOff=0;
int tOff=0;
int tetroType=0;
bool newTetro=1;
int tetroCase=0;
int score=0;
int difficulty=500; //smaller number=more difficult


byte screen[] = {
  B00000,B00000,B00000,B00000,B00000,B00000,B00000,B00000,
  B11111,B11111,B00000,B00000,B00000,B00000,B00000,B00000, 
  B00000,B00000,B00000,B11111,B11111,B00000,B00000,B00000, 
  B00000,B00000,B00000,B00000,B00000,B00000,B11111,B11111, 
  B11111,B11111,B00000,B11111,B11111,B00000,B00000,B00000, 
  B11111,B11111,B00000,B00000,B00000,B00000,B11111,B11111, 
  B00000,B00000,B00000,B11111,B11111,B00000,B11111,B11111,
  B11111,B11111,B00000,B11111,B11111,B00000,B11111,B11111
};
bool blocks[6][16]= {
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
//tetro[type][rotation][coordinate number][x and y] makes an array or x and y value for each tetromino
int tetro[7][4][4][2] = {
  {
  {{0,0},{1,0},{0,1},{1,1}}, //tetro[0]: O
  {{0,0},{1,0},{0,1},{1,1}},
  {{0,0},{1,0},{0,1},{1,1}},
  {{0,0},{1,0},{0,1},{1,1}}
  },
  {
  {{0,2},{1,2},{2,2},{3,2}}, //tetro[1]: I
  {{2,3},{2,2},{2,1},{2,0}},
  {{0,1},{1,1},{2,1},{3,1}},
  {{1,3},{1,2},{1,1},{1,0}}
  },
  {
  {{0,1},{1,1},{2,1},{1,2}}, //tetro[2]: T
  {{1,0},{1,1},{2,1},{1,2}},
  {{1,0},{1,1},{2,1},{0,1}},
  {{1,0},{1,1},{1,2},{0,1}}
  },
  {
  {{0,1},{1,1},{1,2},{2,2}}, //tetro[3]: S
  {{1,2},{1,1},{2,1},{2,0}},
  {{0,0},{1,0},{1,1},{2,1}},
  {{0,2},{0,1},{1,1},{1,0}}
  },
  {
  {{0,2},{1,1},{1,2},{2,1}}, //tetro[4]: Z
  {{1,0},{1,1},{2,1},{2,2}},
  {{0,1},{1,0},{1,1},{2,0}},
  {{0,0},{0,1},{1,1},{1,2}}
  },
  {
  {{0,2},{0,1},{1,1},{2,1}}, //tetro[5]: J
  {{1,0},{1,1},{1,2},{2,2}},
  {{2,0},{0,1},{1,1},{2,1}},
  {{1,0},{1,1},{1,2},{0,0}}
  },
  {
  {{2,2},{0,1},{1,1},{2,1}}, //tetro[6]: L
  {{1,0},{1,1},{1,2},{2,0}},
  {{0,0},{0,1},{1,1},{2,1}},
  {{1,0},{1,1},{1,2},{0,2}}
  }
};
bool putBlock(int x, int y, int mode) { //mode says whether to place a block (1) or remove a block (0)
  if(x<0 || x>5) {
    Serial.println("Bad X");
    return true;
  }
  if(y>15) {
    Serial.println("Bad Y");
    return true;
  }
  if(y<0) {
    return false;
  }
  if(mode<0 || mode>2) 
    Serial.println("Bad Mode");
    if(mode==2) //collision detection
      {
        if(blocks[x][y]==1) {
          return true; Serial.println("Collided");}
        if(blocks[x][y]==0) {
          return false; Serial.println("Safe to move");}
      }
    else
      {blocks[x][y]=mode;}
  int piece=0;
  for(int i=0; i<=2; i++) { //scans blocks array to determine correct character
    if(blocks[3*int(x/3)+i][y]==1) {
      if(piece<=3 && piece>0)
        piece++;
      piece=piece+1+i;
    }   
  }
  lcd.setCursor(y, int(x/3));
  lcd.write(byte(piece));
  return false;
}
//either erases or places a tetromino
bool putTetro(int shape, int rotation, int x, int y, int mode) //returns true if a collision is detected
{
  if(mode==1) {
    for(int i=0; i<=3; i++) {
      if(putBlock((tetro[shape][rotation][i][0]+x)%6,tetro[shape][rotation][i][1]+y, 2)==true) //if it finds a collision
        return true;
    }
  }
  for(int i=0; i<=3; i++) {
    putBlock((tetro[shape][rotation][i][0]+x)%6,tetro[shape][rotation][i][1]+y, mode);
  }
  return false;
}
void checkTetro(int shape, int rotation, int x, int y) 
{
  for(int k=0; k<=3; k++) { //checks 
    for(int i=0; i<=3; i++) { //iterates throuogh all four blocks of the tetromino
      for(int j=0; j<=5; j++) { //iterates through row on specified block's y coordinate
        if(putBlock((tetro[shape][rotation][i][0]+x+j)%6,tetro[shape][rotation][i][1]+y, 2)==true) { //iterates across the row checking for blocks
          //Serial.print("Found Block "); Serial.print((tetro[shape][rotation][i][0]+x+j)%6); Serial.print(tetro[shape][rotation][i][1]+y); Serial.print("  |  ");
          if(j==5) {
            //Serial.println("Clear Row");
            overlap(tetro[shape][rotation][i][1]+y);
          }
        }
        else { 
          if(k==3 && tetro[shape][rotation][i][1]+y==0) {
            lcd.clear();
            lcd.setCursor(1, 0);
            lcd.write("PLAY AGAIN:");
            Serial.println("Pre wait");
            lcd.setCursor(4, 1);
            lcd.write("SCORE:");
            lcd.setCursor(10, 1);
            lcd.print(score);
            for(int z=15; z>=0; z--) {
              Serial.println("waitng");
              if(z<10) {
                lcd.setCursor(12, 0);
                lcd.write("0");
                lcd.setCursor(13, 0);
                lcd.print(z);
              }
              else {
                lcd.setCursor(12, 0);
                lcd.print(z);
              }
              Serial.println(z);
              delay(1000);
            }
            Serial.println("After wait");
            tetroCase=0;
            score=0;
            lcd.clear();
            for (int i=0; i<=5; i++) {
              for (int j=0; j<=15; j++) {
                blocks[i][j]= 0;
              }
            }
            return;
          }
          //Serial.println("No Block");
          break;
        }
      }
    }
  }
}

void overlap(int row) {
  score=score+6;
  for(int j=0; j<=row; j++) {
    for(int i=0; i<=5; i++) {
      if(row-j!=0) {
        //blocks[i][row-j]=blocks[i][row-j-1];
        putBlock(i,row-j, 0);
        putBlock(i,row-j, blocks[i][row-j-1]);
      }
      else {
        blocks[i][row-j]=0;
      }
    }  
  }
}
void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  
  for (int i = 0; i <= 7; ++i) { //makes chars from array
    lcd.createChar(i, &screen[i * 8]);
  }
  /*putTetro(1, 0, 0, 13, 1);
  putTetro(1, 0, 0, 12, 1);
  putTetro(1, 0, 0, 11, 1);
  putTetro(1, 0, 0, 10, 1);
  putTetro(1, 0, 0, 9, 1);
  putTetro(1, 0, 0, 7, 1);
  putTetro(1, 1, 2, 12, 1);*/
  randomSeed(analogRead(A0)); //SSHHHH Don't tell anyone but the random number is seeded from the pot on A0
  tetroType=int(random(0, 7));
  Serial.println(tetroType);//random number between 0 and 6 inclusive 
  xOff=(map(analogRead(A5), 0, 1023, 0, 10))-xOffPrevious;
}

void loop() {
  start:
  if(newTetro==1) {
    if(tetroCase!=0) {
      switch (tetroCase) {
        case 1:
          Serial.println("Tetro Case 1");
          checkTetro(tetroType, rot, xOffOff, -1+timer);
          break;
        case 2:
          Serial.println("Tetro Case 2");
          checkTetro(tetroType, rotPrevious, xOff, -1+timer);
          break;
        case 3:
          Serial.println("Tetro Case 3");
          checkTetro(tetroType, rot, xOff, -1+timerPrevious);
          break;
      }
    }
    if(tetroCase==0) {
      Serial.println("Tetro Case 0. New Game Beginning");
    }
    Serial.println("Extra testing info");
    tOff=int(millis()/difficulty); //essentially resetting millis()
    timer=int(millis()/difficulty)-tOff;
    timerPrevious=timer;
    tetroType=int(random(0, 7));
    //tetroType=1; //testing
    newTetro=0;
    xOffPrevious=xOff;
  }
  xOffOff=xOff;
  xOff=(map(analogRead(A5), 0, 1023, 0, 10))-xOffPrevious;
  while(xOff<0)
    {xOff=xOff+6;}
  if(xOff!=xOffOff) {
    putTetro(tetroType, rot, xOffOff, -1+timer, 0);
    Serial.print("xOff: ");
    Serial.println(xOff);
    if(putTetro(tetroType, rot, xOff, -1+timer, 1)==true) { //if there is a block in intended spot
      putTetro(tetroType, rot, xOffOff, -1+timer, 1);
      Serial.println("X Collision has been Detected");
      newTetro=1;
      tetroCase=1;
      goto start;
    }
  }
  rotPrevious=rot;
  rot=(map(analogRead(A0), 0, 1023, 0, 4))%4;
  if(rot!=rotPrevious) {
    putTetro(tetroType, rotPrevious, xOff, -1+timer, 0);
    if(putTetro(tetroType, rot, xOff, -1+timer, 1)==true) {
      putTetro(tetroType, rotPrevious, xOff, -1+timer, 1);
      Serial.println("Collision has been Detected");
      newTetro=1;
      tetroCase=2;
      goto start;
    }
  }
  timerPrevious=timer;
  timer=int(millis()/difficulty)-tOff;
  if(timer!=timerPrevious) {
    putTetro(tetroType, rot, xOff, -1+timerPrevious, 0);
    if(putTetro(tetroType, rot, xOff, -1+timer, 1)==true) {
      putTetro(tetroType, rot, xOff, -1+timerPrevious, 1);
      Serial.println("Collision Detected");
      newTetro=1;
      tetroCase=3;
      goto start;
    }
  }
  
  delay(50);
}
