#include <AccelStepper.h>
#include <MultiStepper.h>
#include <string.h>
#include <math.h>
#include <Servo.h>


int servoPin =9 ;
int PUL3_PIN = 4;
int DIR3_PIN = 7;
int EN1_PIN = 8;
int PUL1_PIN = 2;
int DIR1_PIN = 5;
int Switch1 = 3, Switch2 = 6;


MultiStepper steppers;
AccelStepper stepper1(AccelStepper::DRIVER, PUL1_PIN, DIR1_PIN);
AccelStepper stepper3(AccelStepper::DRIVER, PUL3_PIN, DIR3_PIN);

Servo servo ; 

bool lock = true;
int curSpeed = 800;
int idx = 0;
bool newchar=false;
int i=0, j=-1.5;
bool en=false;
char str[10];
String data,data1,data2;
bool circle = true;
bool cross = false ; 
bool isFirst=true;
int first[2],final[2];
bool finish=false;

long pos[2];
float l01 = 12, l02 = 12;
float l2 = 29, l3 = 20;
float L = 30;


float n = 3;


float r = 2;
                                                                                                                                

void PengoDown(){
  for(int i=0;i<=180;i++){
    servo.write(i);
    delay(5);
  }  
}

void PengoUp(){
  for(int i=180;i>=0;i--){
    servo.write(i);
    delay(5);
  } 
}

void enableAll() {
  digitalWrite(EN1_PIN, LOW);
  en = true;
}

void disableAll() {
  digitalWrite(EN1_PIN, HIGH);
  en = false;
}


void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  String rc1;
  char rc[10];
  while (Serial.available() > 0 && newchar == false) {

    rc1 = Serial.readString();
    rc1.toCharArray(rc,4);
    
    str[0]=rc[0];
    str[1]=rc[1];
    str[2]=rc[2];    

    newchar = true;
    enableAll();
    
  }
}


void getData() {

  char a1[5], a2[5];

  if (newchar == false) {
    return;
  }
  newchar = false;

  if(str[0]=='X'){
    
    cross=true;
    circle=false;
    Serial.println("Đánh dấu X");
    disableAll();
  }
  else if(str[0]=='O'){
    cross=false;
    circle=true;
    Serial.println("Đánh dấu O");
    disableAll();
  }
  
  else if(str[0]!='X'&&str[0]!='O'&&str[0]!='a'){
    
    Serial.print("Di chuyển đến điểm : ");
    Serial.println(str);
    for (int s = 0; s < strlen(str); s++) {
      if (str[s] == ' ') {

        idx = s;
        strncpy(a1, str + 0, s);
        strncpy(a2, str + s + 1, strlen(str) - s - 1);
        break;
      }
    }
    
    String string1 = String(a1);
    String string2 = String(a2);

    i = string1.toInt();
    j = string2.toInt();
    
    if(isFirst==true){
      first[0]=i;first[1]=j;
      isFirst=false;
    }
    final[0]=i;final[1]=j;

    // Serial.print(final[0]);
    // Serial.println(final[1]);
    // Serial.print(first[0]);
    // Serial.println(first[1]);

  }

  else if (str[0]=='a'){
    finish=true;
    // Serial.println("ddddddd");
  }

}



void ControlAngelXandZ() {
  long position[2];

  position[0] = -i *1120.0/36.0 ;
  position[1] = -j * 800.0/36.0;



  stepper1.setMaxSpeed(curSpeed);
  stepper3.setMaxSpeed(curSpeed);
  steppers.moveTo(position);
  steppers.runSpeedToPosition();
}


void CaculatePoint() {

  long position[2];
  float Xij , Yij , X , Y , A , B ;
  float c2 , s2  , c1 , s1 ;
  double t1 , t2 ;

    Xij=L*(j+0.5)/n;
    Yij=L*(i+0.5)/n;

    X = Xij ;
    Y = Yij ;

    A = X+l01; 
    B = Y-l02;

    c2 = ( sq(A) + sq(B) - sq(l2) - sq(l3) )/(2*l2*l3);
    s2 = sqrt(abs(1-sq(c2)));
    t2 = atan2(s2,c2);

    c1 = +l3*sin(t2)*(Y-l02)+(X+l01)*(l2+l3*c2);
    s1 =  -A*l3*sin(t2)+(l2+l3*cos(t2))*B;
    t1 = atan2(s1,c1);

    t1=180*t1/PI;
    t2=180*t2/PI;

    // Px = l1 * cos(t1) + l2 * cos(t1 + t2);
    // Py = l1 * sin(t1) + l2 * sin(t1 + t2);

    position[0] = -t1 * 1120 / 36;
    position[1] = -t2 * 800 / 36;
    

    if(en){
      stepper1.setMaxSpeed(curSpeed);
      stepper3.setMaxSpeed(curSpeed);
      steppers.moveTo(position);
      steppers.runSpeedToPosition();
    }
}


void drawcircle(){

  long position[2];
  float Xij , Yij , X , Y , A , B ;
  float c2 , s2  , c1 , s1 ;
  double t1 , t2 ;

  if(i>=0&&j>=0){

    for(float t=0;t<2*PI;t=t+0.2){

      Xij=L*(j+0.5)/n;
      Yij=L*(i+0.5)/n;

      X = Xij + r*cos(t) ;
      Y = Yij + r*sin(t);


      A = X+l01; 
      B = Y-l02;

      c2 = ( sq(A) + sq(B) - sq(l2) - sq(l3) )/(2*l2*l3);
      s2 = sqrt(abs(1-sq(c2)));
      t2 = atan2(s2,c2);

      c1 = +l3*sin(t2)*(Y-l02)+(X+l01)*(l2+l3*c2);
      s1 =  -A*l3*sin(t2)+(l2+l3*cos(t2))*B;
      t1 = atan2(s1,c1);

      t1=180*t1/PI;
      t2=180*t2/PI;

      // Px = l1 * cos(t1) + l2 * cos(t1 + t2);
      // Py = l1 * sin(t1) + l2 * sin(t1 + t2);

      position[0] = -t1 * 1120 / 36;
      position[1] = -t2 * 800 / 36;
      

      if(en){
        stepper1.setMaxSpeed(curSpeed);
        stepper3.setMaxSpeed(curSpeed);
        steppers.moveTo(position);
        steppers.runSpeedToPosition();
      }
    }
  }

  else {

  }

}

void drawcross(){

  long position[2];
  float Xij , Yij , X , Y , A , B ;
  float c2 , s2  , c1 , s1 ;
  double t1 , t2 ;

  if(i>=0&&j>=0){
    PengoDown();
    for(float t=0;t<1;t=t+0.2){

      Xij=L*(j+0.5)/n;
      Yij=L*(i+0.5)/n;

      X=(Xij-r)+(2*r)*t;
      Y=(Yij-r)+(2*r)*t;

      A = X+l01; 
      B = Y-l02;

      c2 = ( sq(A) + sq(B) - sq(l2) - sq(l3) )/(2*l2*l3);
      s2 = sqrt(abs(1-sq(c2)));
      t2 = atan2(s2,c2);

      c1 = +l3*sin(t2)*(Y-l02)+(X+l01)*(l2+l3*c2);
      s1 =  -A*l3*sin(t2)+(l2+l3*cos(t2))*B;
      t1 = atan2(s1,c1);

      t1=180*t1/PI;
      t2=180*t2/PI;

      // Px = l1 * cos(t1) + l2 * cos(t1 + t2);
      // Py = l1 * sin(t1) + l2 * sin(t1 + t2);

      position[0] = -t1 * 1120 / 36;
      position[1] = -t2 * 800 / 36;
      

      if(en){
        stepper1.setMaxSpeed(curSpeed);
        stepper3.setMaxSpeed(curSpeed);
        steppers.moveTo(position);
        steppers.runSpeedToPosition();
      }
    }
//////////////////////////////////////////////////////////////
    PengoUp();
    for(float t=0;t<1;t=t+0.2){

      Xij=L*(j+0.5)/n;
      Yij=L*(i+0.5)/n;

      X=Xij+r;
      Y=(Yij+r)-(2*r)*t;

      A = X+l01; 
      B = Y-l02;

      c2 = ( sq(A) + sq(B) - sq(l2) - sq(l3) )/(2*l2*l3);
      s2 = sqrt(abs(1-sq(c2)));
      t2 = atan2(s2,c2);

      c1 = +l3*sin(t2)*(Y-l02)+(X+l01)*(l2+l3*c2);
      s1 =  -A*l3*sin(t2)+(l2+l3*cos(t2))*B;
      t1 = atan2(s1,c1);

      t1=180*t1/PI;
      t2=180*t2/PI;

      // Px = l1 * cos(t1) + l2 * cos(t1 + t2);
      // Py = l1 * sin(t1) + l2 * sin(t1 + t2);

      position[0] = -t1 * 1120 / 36;
      position[1] = -t2 * 800 / 36;
      

      if(en){
        stepper1.setMaxSpeed(curSpeed);
        stepper3.setMaxSpeed(curSpeed);
        steppers.moveTo(position);
        steppers.runSpeedToPosition();
      }

    }

//////////////////////////////////////////////////////////
    PengoDown();
    for(float t=0;t<1;t=t+0.2){

      Xij=L*(j+0.5)/n;
      Yij=L*(i+0.5)/n;

      X=(Xij+r)-(2*r)*t;
      Y=(Yij-r)+(2*r)*t;

      A = X+l01; 
      B = Y-l02;

      c2 = ( sq(A) + sq(B) - sq(l2) - sq(l3) )/(2*l2*l3);
      s2 = sqrt(abs(1-sq(c2)));
      t2 = atan2(s2,c2);

      c1 = +l3*sin(t2)*(Y-l02)+(X+l01)*(l2+l3*c2);
      s1 =  -A*l3*sin(t2)+(l2+l3*cos(t2))*B;
      t1 = atan2(s1,c1);

      t1=180*t1/PI;
      t2=180*t2/PI;

      // Px = l1 * cos(t1) + l2 * cos(t1 + t2);
      // Py = l1 * sin(t1) + l2 * sin(t1 + t2);

      position[0] = -t1 * 1120 / 36;
      position[1] = -t2 * 800 / 36;
      
      if(en){
        stepper1.setMaxSpeed(curSpeed);
        stepper3.setMaxSpeed(curSpeed);
        steppers.moveTo(position);
        steppers.runSpeedToPosition();
      }
    }
    PengoUp();
    
  }

  else {

  }
}

void backHome(){

  double t1 , t2 ;
  long pos[2];

  t1=-1.5225;
  t2=2.7214;

  t1=180*t1/PI;
  t2=180*t2/PI;

  pos[0] = -t1 * 1120 / 36;
  pos[1] = -t2 * 800 / 36;

  if(en){
    stepper1.setMaxSpeed(curSpeed);
    stepper3.setMaxSpeed(curSpeed);
    steppers.moveTo(pos);
    steppers.runSpeedToPosition();
    
  }
  disableAll();
}

void isFinish(){
  
  long position[2];
  float Xij , Yij , X , Y , A , B ;
  float c2 , s2  , c1 , s1 ;
  double t1 , t2 ;
  for(float t=0;t<1;t=t+0.2){


    X = final[0]+(first[0]-final[0])*t ;
    Y = final[1]+(first[1]-final[1])*t ;
    X=10*X;
    Y=10*Y;
    A = X+l01; 
    B = Y-l02;

    c2 = ( sq(A) + sq(B) - sq(l2) - sq(l3) )/(2*l2*l3);
    s2 = sqrt(abs(1-sq(c2)));
    t2 = atan2(s2,c2);

    c1 = +l3*sin(t2)*(Y-l02)+(X+l01)*(l2+l3*c2);
    s1 =  -A*l3*sin(t2)+(l2+l3*cos(t2))*B;
    t1 = atan2(s1,c1);

    t1=180*t1/PI;
    t2=180*t2/PI;

    // Px = l1 * cos(t1) + l2 * cos(t1 + t2);
    // Py = l1 * sin(t1) + l2 * sin(t1 + t2);

    position[0] = -t1 * 1120 / 36;
    position[1] = -t2 * 800 / 36;
    

    if(en){
      stepper1.setMaxSpeed(curSpeed);
      stepper3.setMaxSpeed(curSpeed);
      steppers.moveTo(position);
      steppers.runSpeedToPosition();
    }
  }
}



void setup() {

  Serial.begin(9600);

  Serial.write('1');

  servo.attach(servoPin);
  pinMode(EN1_PIN, OUTPUT);
  pinMode(PUL3_PIN, OUTPUT);
  pinMode(DIR3_PIN, OUTPUT);
  pinMode(PUL1_PIN, OUTPUT);
  pinMode(DIR1_PIN, OUTPUT);
  pinMode(Switch1, OUTPUT);
  pinMode(Switch2, OUTPUT);

  digitalWrite(EN1_PIN, LOW);

  
  steppers.addStepper(stepper1);
  steppers.addStepper(stepper3);

  stepper1.setMaxSpeed(curSpeed);
  stepper3.setMaxSpeed(curSpeed);

  stepper1.setSpeed(curSpeed);
  stepper3.setSpeed(curSpeed);

  stepper1.setAcceleration(200);
  stepper3.setAcceleration(200);

  stepper1.setCurrentPosition(0);
  stepper3.setCurrentPosition(0);
}


void loop() {
   


  recvWithEndMarker();
  getData();  

  CaculatePoint();

  if(circle==true&&cross==false&&en){
    PengoDown();    
    drawcircle();
    PengoUp();
  }

  else if (circle==false&&cross==true&&en)
    drawcross();
  backHome();



}
