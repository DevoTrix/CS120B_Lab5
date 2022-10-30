int i = 0;
int c = 0;
int IN1 = 10;
int IN2 = 11;
int IN3 = 12;
int IN4 = 13;
int UP = 3;
int DOWN = 4;
int LEFT = 2;
int RIGHT = 5;
int JS_X = A0;
int JS_Y = A1;
int JS_BTN = 7;
int sig[4] = {IN1, IN2, IN3, IN4};
int steps[8][4] { //codes for the motor to turn properly
  {0, 0, 0, 1},
  {0, 0, 1, 1},
  {0, 0, 1, 0},
  {0, 1, 1, 0},
  {0, 1, 0, 0},
  {1, 1, 0, 0},
  {1, 0, 0, 0},
  {1, 0, 0, 1},
};

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);

} task;

int delay_gcd;
const unsigned short tasksNum = 2;
task tasks[tasksNum];

enum SM1_States { SM1_INIT, SM1_Locked, SM1_UnLocked};
int SM1_Tick(int state1) {
  switch (state1) { // State transitions
    case SM1_INIT:
      state1 = SM1_S0;
      break;
    case SM1_Locked:
      if(k == 256){

      }
      break;

  }
  switch (state1) { // State Action
    case SM1_INIT:
      break;
    case SM1_S0:
      for (c; c < 4; c++) {
        if (steps[i][c] == 1) {
          digitalWrite(sig[c], HIGH);

        }
        else {
          digitalWrite(sig[c], LOW);

        }

      }
      break;

  }

  return state1;
}

enum SM2_States { SM2_INIT, SM2_S0};
int SM2_Tick(int state2) {
  switch (state2) { // State transitions
    case SM2_INIT:
      //State Transition
      state2 = SM2_S0;
      break;
    case SM2_S0:
      state2 = SM2_S0;
      break;

  }
  switch (state2) { // State Action
    case SM2_INIT:
      break;
    case SM2_S0:
      digitalWrite(UP, HIGH);
      digitalWrite(DOWN, LOW);
      digitalWrite(LEFT, LOW);
      digitalWrite(RIGHT, HIGH);

      int j_x = analogRead(JS_X);
      int j_y = analogRead(JS_Y);
      int j_btn = digitalRead(JS_BTN);

      Serial.print("X: "); Serial.print(j_x);
      Serial.print(" Y: " ); Serial.print(j_y);
      Serial.print(" BTN: "); Serial.println(j_btn);
      
      break;

  }

  return state2;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(UP, OUTPUT);
  pinMode(DOWN, OUTPUT);
  pinMode(LEFT, OUTPUT);
  pinMode(RIGHT, OUTPUT);
  pinMode(JS_BTN, INPUT_PULLUP);
  Serial.begin(9600);


  unsigned char i = 0;
  tasks[i].state = SM1_INIT;
  tasks[i].period = 1;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM1_Tick;
  i++;
  tasks[i].state = SM2_INIT;
  tasks[i].period = 50;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM2_Tick;

  delay_gcd = 1; // GCD
}

void loop() {
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) {
    if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
      tasks[i].state = tasks[i].TickFct(tasks[i].state);
      tasks[i].elapsedTime = millis(); // Last time this task was ran
    }
  }
}
