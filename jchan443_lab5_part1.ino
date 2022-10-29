int j = 0;
int t = 0;
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

enum SM1_States { SM1_INIT, SM1_S0, SM1_S1};
int SM1_Tick(int state1) {
  static  int x = analogRead(JS_X);
  static  int y = analogRead(JS_Y);
  static unsigned int btn = digitalRead(JS_BTN);
  static unsigned int c = 0;
  static unsigned int ccw = 1;
  switch (state1) { // State transitions
    case SM1_INIT:
      state1 = SM1_S0;
      break; 
    case SM1_S0:
      if(x !=0 || y != 0){
        state1 = SM1_S1;
        if(x<0){
             c = 2;
        }
        else if(x>0){
            c = 5;
        }
        if(y <0){
            c = 4;
        }
        else if(y>0){
            c = 3;
        }
      }
      else{
        state1 = SM1_S0;
      }
      break;
    case SM1_S1:
      if(x !=0 || y != 0){
        state1 = SM1_S1;
        if(x<0){
             c = 2;
        }
        else if(x>0){
            c = 5;
        }
        if(y <0){
            c = 4;
        }
        else if(y>0){
            c = 3;
        }
      }
      if(btn){
        state1 = SM1_S0;
      }
      break;
    default:
        break;
  }
  switch (state1) { // State Action
    case SM1_INIT:
      break;
    case SM1_S0:
      for(int i = 2; i < 6;i++){
        digitalWrite(i, LOW);
      }
      break;
    case SM1_S1:
      for(int i = 2; i < 6;i++){
        digitalWrite(i, LOW);
      }
      digitalWrite(c, HIGH);
      break;
    default:
      break;
  }

  return state1;
}

enum SM2_States { SM2_INIT,SM2_S0, SM2_Ccw, SM2_Cw, SM2_rV, SM2_iV, SM2_OFF};
int SM2_Tick(int state2) {
  static int x = analogRead(JS_X);
  static int y = analogRead(JS_Y);
  static unsigned int btn = digitalRead(JS_BTN);
  static int z = 0;
  static int k = 0;
  static int ccw = 0;
  switch (state2) { // State transitions
    case SM2_INIT:
      //State Transition
      state2 = SM2_Ccw;
      break;
    case SM2_Ccw:
      
      if(x<0){
        state2 = SM2_Cw;
      }
      else if(y>0){
        state2 = SM2_iV;
      }
      else if(y < 0){
        state2 = SM2_rV;
      }
      else if(btn){
        state2 = SM2_OFF;
      }
      else if(k >= z){
        if(t==4){
            t==0;
            j++;
        }
        if(j==8){
            j = 0;
        }
        k = 0;
        state2 = SM2_Ccw;
      }
      else{
        k++;
      }
      break;
    case SM2_Cw:
      ccw = 0;
      if(x>0){
        state2 = SM2_Ccw;
      }
      else if(y>0){
        state2 = SM2_iV;
      }
      else if(y < 0){
        state2 = SM2_rV;
      }
      else if(btn){
        state2 = SM2_OFF;
      }
      else if(k >= z){
        if(t==0){
            t= 4;
            j --;
        }
        if(j==0){
            j = 8;
        }
        k = 0;
        state2 = SM2_Cw;
      }
      else{
        k++;
      }
      break;
    case SM2_rV:
    //   state = SM2_S0;
      if(ccw){
        state2 = SM2_Ccw;
      }
      else{
        state2 = SM2_Cw;
      }
      break;
    case SM2_iV:
    //   state = SM2_S0;
    if(ccw){
        state2 = SM2_Ccw;
      }
      else{
        state2 = SM2_Cw;
      }
      break;
    case SM2_OFF:
      if(!btn){
        if(ccw){
            state2 = SM2_Ccw;
        }
        else{
            state2 = SM2_Cw;
        }
      }
      break;
  }

  switch (state2) { // State Action
    case SM2_INIT:
      break;
    case SM2_Ccw:
      ccw = 1;
      for (t; t < 4; t++) {
        if (steps[j][t] == 1) {
          digitalWrite(sig[t], HIGH);

        }
        else {
          digitalWrite(sig[t], LOW);

        }

      }
      break;
    case SM2_Cw:
      ccw = 0;
      for (t; t > 0; t--) {
        if (steps[j][t] == 1) {
          digitalWrite(sig[t], HIGH);

        }
        else {
          digitalWrite(sig[t], LOW);

        }
      }
      break;
    case SM2_iV:
      z--;
      break;
    case SM2_rV:
      z++;
    case SM2_OFF:
      for(int p = 0; p < 4; p++){
        digitalWrite(sig[p], LOW);
      }


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
