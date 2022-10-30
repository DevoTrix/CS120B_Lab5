int i = 0;
int c = 0;
int k = 0; //tracks the rotation
int ok = 1;
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
int wait = 1; //doesnt fuck with the shitty ass timing.
int JS_BTN = 7;
int pass[4] = {1, 1, 2, 2};
int inp[4] = {0,0,0,0};
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

enum SM1_States { SM1_INIT, S0, Unlock, Unlocked, Locked, SetPass};
int SM1_Tick(int state1) {
   int x = analogRead(JS_X);
   int y = analogRead(JS_Y);
   int btn = digitalRead(JS_BTN);
  switch (state1) { // State transitions
    case SM1_INIT:
      state1 = S0;
      break;
    case S0:
        ok = 1;
        if(pass == inp){
            state1 = Unlock;
        }
        state = S0;
        break;
    case Unlock:
      if(k == 256){
        k = 0;
        i = 0;
        inp = {0, 0, 0, 0};
        state1 = Unlocked;
        break;
      }
      else{
        i++;
        if (i == 8) {
            k++;
            i = 0;
        }
      }
      
      state1 = Unlock;
      break;
    case Unlocked:
        ok = 1;
        if(!btn){
            state1 = SetPass;
            break;
        }
        else{
            if(pass == inp){
                state1 = Lock;
                break;
            }
        }
    case SetPass:
        if(x > 500 && x < 600 && y < 600 && y > 500){
            wait = 1;
        }
        if(count > 3){
            count = 0;
            state1 = Unlocked;
            break;
        }
        if(wait){
            if(x < 500){ // checks if left
                digitalWrite(RIGHT, HIGH);
                pass[count] = 2;
                count++;
            }
            else if(x > 600){ // checks if right
                digitalWrite(LEFT, HIGH);
                pass[count] = 1;
                count++;
            }
            else if(y < 500){ // checks if down
                digitalWrite(UP, HIGH);
                pass[count] = 4;
                count++;
            }
            else if(y > 600){ // checks if up
                digitalWrite(DOWN, HIGH);
                pass[count] = 3;
                count++;
            }
            state1 = SetPass;
        }
        state1 = SetPass;
        break;
    case Lock: 
        ok = 0;
        if(k == 256){
        k = 0;
        i = 7;
        inp = {0, 0, 0, 0};
        state1 = S0;
        break;
      }
      else{
        i--;
        if (i == -1) {
            k++;
            i = 7;
        }
      }
      
      state1 = Lock;
      break;

  }
  switch (state1) { // State Action
    case SM1_INIT:
      break;
    case S0:
        for(int p = 2; p < 6; p++){
            digitalWrite(p, LOW);
        }
        break;
    case Unlock:
      ok = 0;
      for (c; c < 4; c++) {
        if (steps[i][c] == 1) {
          digitalWrite(sig[c], HIGH);

        }
        else {
          digitalWrite(sig[c], LOW);

        }

      }
      break;
    case Unlocked:
        for(int p = 2; p < 6; p++){
            digitalWrite(p, HIGH);
        }
    case Lock:
        for (c; c < 4; c++) {
        if (steps[i][c] == 1) {
          digitalWrite(sig[c], HIGH);

        }
        else {
          digitalWrite(sig[c], LOW);

        }

      }
      break;
    case SetPass:
        for(int p = 2; p < 6; p++){
            digitalWrite(p, LOW);
        }
        break;

  }

  return state1;
}

enum SM2_States { SM2_INIT, T0, T1};
int SM2_Tick(int state2) {
  switch (state2) { // State transitions
    case SM2_INIT:
      //State Transition
      state2 = T0;
      break;
    case T0:
        if(x > 500 && x < 600 && y < 600 && y > 500){
            wait = 1;
        }
        if(count > 3){
            count = 0;
            state2 = T1;
            break;
        }
        if(wait){
            if(x < 500){ // checks if left
                digitalWrite(RIGHT, HIGH);
                inp[count] = 2;
                count++;
            }
            else if(x > 600){ // checks if right
                digitalWrite(LEFT, HIGH);
                inp[count] = 1;
                count++;
            }
            else if(y < 500){ // checks if down
                digitalWrite(UP, HIGH);
                inp[count] = 4;
                count++;
            }
            else if(y > 600){ // checks if up
                digitalWrite(DOWN, HIGH);
                inp[count] = 3;
                count++;
            }
            state2 = T0;
        }
        state2 = T0;
        break;
    case T1:
        if(ok == 1){
            state2 = T0;
        }
        break;

  }
  switch (state2) { // State Action
    case SM2_INIT:
      break;
    case T0:
      for(int p = 02; p < 6; p++){
        digitalWrite(p, LOW);
      }
      break;
    case T1:
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
