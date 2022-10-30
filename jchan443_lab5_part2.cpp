int i = 0;
int c = 0;
int locked = 1;
int correct = 0;
int done = 1;
int count = 0;
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
int pass[4] = {1, 1, 2, 3};
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

enum SM1_States { SM1_INIT, SM1_S0, SM1_Lock, SM1_Unlock};
int SM1_Tick(int state1) {
  switch (state1) { // State transitions
    case SM1_INIT:
      state1 = SM1_S0;
      break;
    case SM1_S0:
      if(correct){
        correct = 0;
        if(locked){
            state1 = SM1_Unlock;
        }
        else{
            state1 = SM1_Lock;
        }
      }
      break;
    case SM1_Lock:
      if(k == 256){
        state1 = SM1_S0;
        locked = 1;
        k = 0;
        i = 7;
      }
      else{
        i++;
        if(i==8){
            k++;
            i=0;
        }
        state1 = SM1_Lock;
      }
      break;
    case SM1_Unlock:
        if(k==256){
            done = 1;
            state1 = SM1_S0;
            locked = 0;
            k = 0;
            i = 0;
        }
        else{
            i--;
            if(i == -1){
                k++;
                i = 7;
            }
            state1 = SM1_Unlock;
        }
        break;
  }
  switch (state1) { // State Action
    case SM1_INIT:
      break;
    case SM1_S0:
        break;
    case SM1_Unlock:
      for (c; c < 4; c++) {
        if (steps[i][c] == 1) {
          digitalWrite(sig[c], HIGH);

        }
        else {
          digitalWrite(sig[c], LOW);

        }

      }
      break;
    case SM1_Unlock:
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
// let Right = 1, Left = 2, Up = 3, Down = 4
enum SM2_States { SM2_INIT, SM2_S0, SM2_S1, SM2_Incorrect, SM2_Correct, SM2_SetPass};
int SM2_Tick(int state2) {
  switch (state2) { // State transitions
    case SM2_INIT:
      //State Transition
      state2 = SM2_S0;
      break;
    case SM2_S0:
      if(!locked && digitalRead(JS_BTN)){
        count = 0;
        state2 = SM2_SetPass;
      }
      else{
        count = 0;
        state2 = SM2_S1;
      }
      break;
    case SM2_S1:
        if(x >600){
            digitalWrite(RIGHT, HIGH);
            if(pass[count] != 1){
                count = 0;
                state2 = SM2_Incorrect;
                break;
            }
            else{
                count++;
                break;
            }
        }
        else if(x < 500){
            digitalWrite(LEFT, HIGH);
            if(pass[count] != 2){
                count = 0;
                state2 = SM2_Incorrect;
                break;
            }
            else{
                count++;
                break;
            }
        }
        else if(y < 500){
            digitalWrite(DOWN, HIGH);
            if(pass[count] != 4){
                count = 0;
                state2 = SM2_Incorrect;
                break;
            }
            else{
                count++;
                break;
            }
        }
        else if(y > 600){
            digitalWrite(UP, HIGH);
            if(pass[count] != 3){
                count = 0;
                state2 = SM2_Incorrect;
                break;
            }
            else{
                count++;
                break;
            }
        }
        else{
            state2 = SM1_S1;
        }
    case SM2_Correct:
        correct = 1;
        state2 = SM2_S0;
        break;
    case Sm2_Incorrect:
        if(count == 3){
            count = 0;
            state2 = SM2_S0;
            break;
        }
        else{
            for(int p = 2; p < 6; p++){
                digitalWrite(p, HIGH);
            }
            count++;
        }
        break;
    case SM2_SetPass:
        if(count < 4){
            if(x >600){
                digitalWrite(RIGHT,HIGH);
                count++;
                pass[count] = 1;    
            }
            else if(x < 500){
                digitalWrite(LEFT,HIGH);
                count++;
                pass[count] = 2;
            }
            else if(y < 500){
                digitalWrite(DOWN, LOW);
                count++;
                pass[count] = 4;
            }
            else if(y > 600){
                digitalWrite(UP, HIGH);
                count++;
                pass[count] = 3;
            }
        }
        state2 = SM2_S0;
        break;
        
  }
  switch (state2) { // State Action
    case SM2_INIT:
      break;
    case SM2_S0:
      if(locked){
        for(int p = 2; p < 6; p++){
            digitalWrite(p, LOW);
        }
      }
      break;
    case SM2_S1:
        for(int p = 2; p < 6; p++){
            digitalWrite(p, LOW);
        }
        break;
    case SM2_Incorrect:
        for(int p = 2; p < 6; p++){
            digitalWrite(p,LOW);
        }
        break;
    case SM2_Correct:
        for(int p = 2; p < 6; p++){
            digitalWrite(p, HIGH);
        }
        break;
    case SM2_SetPass:
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
