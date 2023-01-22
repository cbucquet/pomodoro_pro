#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define VRX A0
#define VRY A1
#define SW 6
#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3
#define A 4
#define B 5

// Music Notes
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

#define REST 0
#define speaker 9

const int tempo = 35;
const int cntDeathNotes = 11;

int deathNotes[cntDeathNotes] = {NOTE_B5, NOTE_F6, NOTE_F6, NOTE_F6, NOTE_E6, NOTE_D6, NOTE_C6, NOTE_E5, NOTE_G4, NOTE_E5, NOTE_C6};
int deathDelay[2*cntDeathNotes] = {4, 1, 2, 5, 2, 2, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2};

const int cntCoinNotes = 2;
int coinNotes[cntCoinNotes] = {NOTE_B5, NOTE_F6};
int coinDelay[2*cntCoinNotes] = {2, 1, 2, 0};

const int cntSwitchNotes = 1;
int switchNotes[cntCoinNotes] = {NOTE_FS6};
int switchDelay[2*cntCoinNotes] = {2, 0};

const int cntStartNotes = 7;
int startNotes[cntStartNotes] = {NOTE_E6, NOTE_E6, NOTE_E6, NOTE_C6, NOTE_E6, NOTE_G6, NOTE_G5};
int startDelay[2*cntStartNotes] = {2, 2, 2, 6, 2, 6, 2, 2, 2, 6, 8, 8, 8, 8};
bool startFlags[2*cntStartNotes] = {false, false, false, false, true, false, false, false, false, true, false, true, false, true};

void playSound(int n, int notes[], int rest[]) {
    for (int i = 0; i < n; i++)
    {
      tone(speaker, notes[i]);
      delay(tempo*rest[2*i]);
      noTone(speaker);
      
      delay(tempo*rest[2*i+1]);
    }
}

// Global Variables
int gameState = 0;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int backButton = 3;
int currentButtonState = 0;

int leaderboard[3] = { 69, 42, 21 };
String leaderboardName[3] = { "CPU", "CPU", "CPU" };

const String TIME_OPTIONS[9] = {" 30 ", "1:00", "1:30", "2:00", "2:30", "3:00", "3:30", "4:00", "DEMO"};
int sessions = 3;
bool isDemo = false;

int startTime = millis();
const int STUDY_TIME = 1500;
const int GAME_TIME = 300;

int start_game_time;

int snake_score = 0;

// Back Button
void checkBackButton() {
  if(digitalRead(backButton) == HIGH) {
    gameState = 0;
    playSound(cntSwitchNotes, switchNotes, switchDelay);
  }
}

// Home Screen
void drawHomeScreen() {
  // Draw Buttons
  display.setTextSize(2);
  display.setTextColor(WHITE);
  // Study Button
  display.setCursor(36, 5);
  display.print("Study");
  // display.drawRoundRect(30, 5, 70, 25, 5, WHITE);
  // Scoreboard Button
  display.setCursor(30, 40);
  display.print("Scores");
  // display.drawRoundRect(30, 5, 70, 25, 5, WHITE);

  // Draw Dot
  int yDotPosition = (currentButtonState == 0) ? 12 : 47;
  display.fillCircle(5, yDotPosition, 3, WHITE);
  display.display();
}
void checkInputHomeScreen() {
  //check if button has been pressed
  int input = getJoyStickInput();
  if(input == DOWN && currentButtonState == 0) {
    currentButtonState = 1;
  }
  else if(input == UP && currentButtonState == 1){
    currentButtonState = 0;
  }
  else if(input == A) {
    gameState = currentButtonState == 0 ? 1 : 6;
    playSound(cntSwitchNotes, switchNotes, switchDelay);
  }
}

// Selecting time Screen
void drawSelectingTimeScreen() {
  // Draw Buttons
  display.setTextSize(1);
  display.setTextColor(WHITE);
  // Study Button
  display.setCursor(20, 10);
  display.print("time:");
  // display.drawRoundRect(30, 5, 70, 25, 5, WHITE);
  // Scoreboard Button
  display.setTextSize(2);
  display.setCursor(20, 25);
  display.print("< "+ TIME_OPTIONS[sessions] + " >");
  display.display();
}
void checkInputSelectingTimeScreen() {

  int input = getJoyStickInput();
  if(input == LEFT && sessions > 0) {
    sessions--;
  }
  else if(input == RIGHT && sessions < 8){
    sessions++;
  }
  else if(input == A) {
    gameState = 2;
    playSound(cntSwitchNotes, switchNotes, switchDelay);
    if(TIME_OPTIONS[sessions] == "DEMO") {
      isDemo = true;
      sessions = 1;
    }
    else {
      isDemo = false;
    }
  }
}


// Study time flash screen
void drawStudyTime() {
  // Draw Text
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(36, 10);
  display.print("STUDY");
  display.setCursor(40, 35);
  display.print("TIME");
  display.display();

  bool inverted = true;
  for(int i = 0; i < 4; i++) {
    display.invertDisplay(inverted);
    delay(500);
    inverted = !inverted;
  }

  startTime = millis();
  gameState = 3;
}


// Study Timer Screen
void drawTimer() {
    display.invertDisplay(false);
    int seconds = (isDemo ? 10 : STUDY_TIME) - ((millis() - startTime) / 1000);
    String secondsString = String(seconds%60);
    String minutesString = String(seconds/60);
    
    display.setTextSize(3.5);
    display.setTextColor(WHITE);
    display.setCursor(20, 20);
    display.print(((seconds/60<10) ? "0" : "")+ minutesString+":"+((seconds%60<10) ? "0" : "")+secondsString);

    display.setCursor(10, 55);
    display.setTextSize(0.9);
    String sessionsLeft = String(sessions + 1);    
    display.print("sessions left: "+ sessionsLeft);
    display.display();

    if(seconds <= 0){
      gameState = 4;
    }
}


// Game Time
void drawGameTime() {
  // Draw Text
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(40, 10);
  display.print("GAME");
  display.setCursor(40, 35);
  display.print("TIME");
  display.display();

  bool inverted = true;
  for (int i = 0; i < cntStartNotes; i++)
  {
    tone(speaker, startNotes[i]);
    delay(tempo*startDelay[2*i]);
    if(startFlags[2*i]) {
      display.invertDisplay(inverted);
      inverted = !inverted;
    }
    noTone(speaker);
    
    delay(tempo*startDelay[2*i+1]);
    if(startFlags[2*i+1]) {
      display.invertDisplay(inverted);
      inverted = !inverted;
    }
  }

  startTime = millis();
  gameState = 5;
  start_game_time = millis();
  snakeSetup();
}


// 128x64
// 4x4 snake head
// text 10 pixels tall


bool pressed = false;

int getJoyStickInput() {
  int vrxValue = analogRead(VRX);
  int vryValue = analogRead(VRY);
  int swValue = digitalRead(SW);
  if (vrxValue <= 100) {
    // UP
    if(pressed)
      return -1;
    pressed = true;
    return 0;
  }
  if (vrxValue >= 923) {
    // DOWN
    if(pressed)
      return -1;
    pressed = true;
    return 1;
  }
  if (vryValue <= 100) {
    // Right
    if(pressed)
      return -1;
    pressed = true;
    return 2;
  }
  if (vryValue >= 923) {
    // Left
    if(pressed)
      return -1;
    pressed = true;
    return 3;
  }
  if (swValue == LOW) {
    // Button Pressed
    if(pressed)
      return -1;
    pressed = true;
    return 4;
  }

  // No joystick action
  pressed = false;
  return -1;
}

void leaderBoardLoop() {
  // Draw Buttons
  display.setTextSize(1.9);
  display.setTextColor(WHITE);
  display.setCursor(10,5);
  display.print("Leaderboard");
  // Study Button
  display.setTextSize(1.8);
  for(int i = 0; i < 3; i++) {
    display.setCursor(10, 15*i+20);
    display.print(String(i+1) + ".  " + String(leaderboard[i]) + "   --   " + leaderboardName[i]);
  }
  display.display();
}

void updateLeaderboard(int score) {
  for(int i = 0; i < 3; i++) {
    if(score >= leaderboard[i]) {
      for(int j = 2; j > i; j--) {
        leaderboard[j] = leaderboard[j-1];
        leaderboardName[j] = leaderboardName[j];
      }
      leaderboard[i] = score;
      leaderboardName[i] = "User";
    }
  }
}
void checkLeaderBoardInput() {
  int input = getJoyStickInput();
  if(input == A) {
    gameState = 0;    
  }
}

// *** Snake Code ***

const int X_MAX = 31; // Boarder (not inclusive) for x coord
const int Y_MAX = 16; // Boarder (not inclusive) for y coord
const int MAX_CNT = X_MAX*Y_MAX; // Max # of snake components
const int START_LENGTH = 5; // Default starting snake length
const int EXTRA_LENGTH = 3; // Length increase after food consumption
const int SIDE = 4; // Side length of each square (snake parts, food) 
const int MOVE_GAP = 75; // Time, in milleseconds, between each snake move

int dir = -1; // Direction of movement, -1 = nothing
int xPos[MAX_CNT]; // xPos[i] = x coord of snake component i
int yPos[MAX_CNT]; // yPos[i] = y coord of snake component i
int found[X_MAX][Y_MAX]; // found[i][j] = # of snake blocks at position (i, j)
int cnt = 2; // Number of components inside snake. We manually create 2
int xFood, yFood; // x and y coordinates of the food
int lastTime; // Last time that the snake moved positions

// Directions determined by move
const int xDir[4] = {0, 0, 1, -1};
const int yDir[4] = {-1, 1, 0, 0};


// Record the count of each snake position
void fillFound()
{
    // Start by resetting the array
    for (int i = 0; i < X_MAX; i++)
        for (int j = 0; j < Y_MAX; j++)
            found[i][j] = 0;
    
    for (int i = 0; i < cnt; i++)
        found[xPos[i]][yPos[i]]++;
}


// Return true if the snake is in bounds and
// there are no intersections, false otherwise
// minBlocks = min value of found[x][y] to trigger death
// CRITICAL: THIS FUNCTION ASSUMES FOUND IS PREPROCESSED
bool check(int x, int y, int minBlocks=2)
{
    // Horizontal out-of-bounds
    if (x < 0 || x >= X_MAX)
        return false;
    
    // Vertical out-of-bounds
    else if (y < 0 || y >= Y_MAX)
        return false;
    
    // Intersection
    else if (found[x][y] >= minBlocks)
        return false;
    
    else
        return true;
}


// Extend snake by "extra" blocks
// We don't allow negative positions or intersections
void extendSnake(int extra)
{
    fillFound();
    
    // Difference in last two blocks
    int xDif = xPos[cnt-1]-xPos[cnt-2];
    int yDif = yPos[cnt-1]-yPos[cnt-2];
    int curDir = 0;
    
    for (int i = 0; i < 4; i++)
        if (xDif == xDir[i] && yDif == yDir[i])
            curDir = i;
    
    // Position of the next block to be added
    int xNxt = xPos[cnt-1];
    int yNxt = yPos[cnt-1];
    
    // MAX_CNT-1 to guarantee one space open
    int finCnt = min(cnt+extra, MAX_CNT-1);

    // cnt should end at finCnt
    for (; cnt < finCnt; cnt++)
    {
        xNxt += xDir[curDir];
        yNxt += yDir[curDir];
    
        // Invalid extension --> break
        if (!check(xNxt, yNxt, 1))
            break;
        
        // Valid extension --> record
        else
        {
            xPos[cnt] = xNxt;
            yPos[cnt] = yNxt;
            found[xNxt][yNxt]++;
        }
    }
}


// Spawn a piece of food by repeatedly randomizing xFood, yFood
// Ensure that food is spawned in an unoccupied position with found
void spawnFood()
{
    fillFound();
    xFood = random(X_MAX);
    yFood = random(Y_MAX);
    
    while (found[xFood][yFood])
    {
        xFood = random(X_MAX);
        yFood = random(Y_MAX);
    }
}


// drawRect but with out specifications
void drawRectSpecial(int x, int y)
{
    display.drawRect(x*SIDE, y*SIDE, SIDE, SIDE, WHITE);
}


// Display the snake and food
void displaySnake()
{
    display.clearDisplay();
  
    for (int i = 0; i < cnt; i++)
      drawRectSpecial(xPos[i], yPos[i]);
  
    drawRectSpecial(xFood, yFood);
    display.display();
}

// The snake is dead.
// Delay so the player can see their mistake.
// Return score (snake length).
int death()
{
    playSound(cntDeathNotes, deathNotes, deathDelay);

    for(int i = 0; i < X_MAX; i++) {
      for(int j = 0; j < Y_MAX; j++) {
        found[X_MAX][Y_MAX] = 0;
      }
    }
    
    int copy = cnt;
    cnt = 2;
    dir = -1;    

    return copy;
}

void snakeSetup() {
    randomSeed(analogRead(A2));
    snake_score = 0;
    // Establish initial position
    // x starts at >= START_LENGTH to
    // ensure that the entire snake can spawn
    xPos[0] = random(START_LENGTH, X_MAX);
    yPos[0] = random(Y_MAX);
    
    // Set manually to provide
    // "extendSnake" with a default direction
    xPos[1] = xPos[0]-1;
    yPos[1] = yPos[0];

    extendSnake(START_LENGTH-cnt);
    spawnFood();

    // Let the player see the snake then delay
    displaySnake();
    delay(500);

    lastTime = millis();
}


void snakeLoop() {

  // while the game time is less than 5 min...
  while(true) {
    int move = getJoyStickInput();
    
    if (move >= 0 && move < 4)
    {
        int xNxt = xPos[0] + xDir[move];
        int yNxt = yPos[0] + yDir[move];
        
        if (xNxt != xPos[1] || yNxt != yPos[1])
            dir = move;
    }
    
    // Move the snake forward
    if (dir != -1 && millis() - lastTime > MOVE_GAP)
    {
        lastTime = millis();
      
        // Each block moves to the position of the block in front of it
        for (int i = cnt-1; i > 0; i--)
        {
            xPos[i] = xPos[i-1];
            yPos[i] = yPos[i-1];
        }
        
        // The front block moves in a potentially new direction
        xPos[0] += xDir[dir];
        yPos[0] += yDir[dir];
    }
    
    displaySnake();
    
    // Snake intersection --> death
    if (!check(xPos[0], yPos[0]))
    {
      Serial.print("Score: ");

      break;
    }

    fillFound();
    
    // Snake has consumed food -->
    // extend and spawn new food
    if (found[xFood][yFood])
    {
        snake_score++;
        extendSnake(EXTRA_LENGTH);
        spawnFood();
        playSound(cntCoinNotes, coinNotes, coinDelay);
    }
  }

  death();

  for (int a = 0; a < 4; a++) {
    display.clearDisplay();
    display.setCursor(25, 15);
    display.print("Score:" + String(snake_score));
    display.setCursor(40, 40);
    int seconds = (isDemo ? 15 : GAME_TIME) - ((millis() - start_game_time) / 1000);
    if(seconds < 0) {
      seconds = 0;
    }
    String secondsString = String(seconds%60);
    String minutesString = String(seconds/60);
    display.print(((seconds/60<10) ? "0" : "")+ minutesString+":"+((seconds%60<10) ? "0" : "")+secondsString);
    updateLeaderboard(snake_score);
    display.display();

    delay(500);
  }
  

  if (millis() - start_game_time >= (isDemo ? 15 : GAME_TIME)*1000) {
    // subtract sessions by one and change to study time
    sessions--;
    if (sessions < 0) {
      gameState = 0;
      sessions = 3;  
    }
    else{
      gameState = 2;      
    }
  }
  else {
    snakeSetup();
    snakeLoop();    
  }
    
  
}

// *** End Snake Code ***

void setup() {
  Serial.begin(9600);

  // Joystick Setup
  pinMode(VRX, INPUT);
  pinMode(VRY, INPUT);
  pinMode(SW, INPUT_PULLUP);


  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  // display.display();
  // delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  display.display();

}

// void loop() {
//   int vrxValue = analogRead(VRX);
//   int vryValue = analogRead(VRY);

//   Serial.println("vrx: " + String(vrxValue) + "     " + "vry: " + String(vryValue));
// }



void loop() {
  // put your main code here, to run repeatedly:
  display.clearDisplay();
  
  switch (gameState) {
    case 0:
      checkInputHomeScreen();
      drawHomeScreen();
      // delay(100);
      break;
    case 1:
      checkInputSelectingTimeScreen();
      // checkBackButton();
      drawSelectingTimeScreen();
      // delay(100);
      break;
    case 2:
      drawStudyTime();
      break;
    case 3:
      drawTimer();
      break;
    case 4:
      drawGameTime();
      break;
    
    case 5:
      snakeLoop();
      break;
    case 6:
      // checkBackButton();
      checkLeaderBoardInput();
      leaderBoardLoop();
      break;

    case 7:
      break;


    default:
      break;
  }

  // Serial.println(getJoyStickInput());
  delay(100);
}
