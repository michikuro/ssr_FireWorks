#include <Arduino.h>
#include <PS4Controller.h>
#include <ESP32Servo.h> 
//LOWが正転、HIGHが逆転
Servo sv_object_hold; //sv_object_holdをServoの変数とする
Servo sv_minicar; //sv_minicarをServoの変数とする
Servo sv_animal_hold; //sv_animal_holdをServoの変数とする

int dir_animal_updown = 17;      //モーター回転方向
int pwm_animal_updown = 16;      //モーター速度
int pwmch_animal_updown = 7;    //PWMチャンネル

int dir_block_updown = 19;      //モーター回転方向
int pwm_block_updown = 18;      //モーター速度
int pwmch_block_updown = 8;    //PWMチャンネル

int dir_omu1 = 23;      //モーター回転方向
float pwm_omu1 = 22;      //モーター速度
int pwmch_omu1 = 2;    //PWMチャンネル

int dir_omu2 = 4;      //モーター回転方向
float pwm_omu2 = 2;      //モーター速度
int pwmch_omu2 = 3;    //PWMチャンネル

int dir_omu3 = 33;      //モーター回転方向
float pwm_omu3 = 32;      //モーター速度
int pwmch_omu3 = 4;    //PWMチャンネル

int i_ani = 0;
 int i_obj = 0;
int i_mini = 0;

const int turn = 1500;  //十字路を曲がる秒数





const int r_photo =  35;  // 右のフォトリフレクタ
const int l_photo =  34;  // 左のフォトリフレクタ
const int tl_photo =  27;  // 曲がる用左のフォトリフレクタ
const int tr_photo =  14;  // 曲がる用右のフォトリフレクタ
const int threshold  = 3500;  // センサーの閾値(適宜変更)
 
const int resolution = 256;          //PWMは256段階 

float degree;
 

bool X = false;

bool grasp_or_release = true;


void incre_pro(){                     //前進をだんだん加速する
  
  for(int i = 0; i < 128; ++i ){
        digitalWrite(dir_omu2, LOW);    //正転
        digitalWrite(dir_omu3, HIGH);     //逆転

        ledcWrite(pwmch_omu1, 0);        //停止
        ledcWrite(pwmch_omu2, i);       
        ledcWrite(pwmch_omu3, i);
        delay(10);   
  }

}

void decre_pro(){                     //前進だんだん減速する
  
  for(int i = 128; i > 0; --i ){
        digitalWrite(dir_omu2, HIGH);    //正転
        digitalWrite(dir_omu3, LOW);     //逆転

        ledcWrite(pwmch_omu1, 0);        //停止
        ledcWrite(pwmch_omu2, i);       
        ledcWrite(pwmch_omu3, i);
        delay(10);   
  }

}

/// @brief 
/// @return 
float distance(){      //Rスティックの原点からの距離を出す関数
float distance;
 
  if(PS4.RStickX() < 2 && PS4.RStickX() > -2){
      distance = abs(PS4.RStickY()); 
  }
  else if(PS4.RStickY() < 2 && PS4.RStickY() > -2){
      distance = abs(PS4.RStickX());
  }
  else{
  // tan(degree) = abs * (PS4.RStickY()/PS4.RStickX());
  degree = atan(abs(PS4.RStickY()/PS4.RStickX()));
  distance = abs(PS4.RStickY() / sin(degree));
  }
  return distance;      //距離は0~128
}


void setup() {
  Serial.begin(115200);
  PS4.begin("e0:5a:1b:10:20:2a");
  Serial.println("Ready.");

    pinMode(dir_animal_updown, OUTPUT);
    pinMode(pwm_animal_updown, OUTPUT);

    ledcSetup(pwmch_animal_updown, 20000, 8);
    ledcAttachPin(pwm_animal_updown, pwmch_animal_updown);
    ledcWrite(pwmch_animal_updown,0);


    pinMode(dir_block_updown, OUTPUT);
    pinMode(pwm_block_updown, OUTPUT);

    ledcSetup(pwmch_block_updown, 20000, 8);
    ledcAttachPin(pwm_block_updown , pwmch_block_updown);
    
    

    sv_object_hold.attach(21, 500, 2400) ;   //sv_object_holdの出力をリセットした上で、15番ピンに割り当て、パルス幅を500～2400マイクロ秒とする
    sv_minicar.attach(15, 500, 2400) ;       //sv_minicarの出力をリセットした上で、D6番ピンに割り当て、パルス幅を500～2400マイクロ秒とする
    sv_animal_hold.attach(13, 500, 2400) ;   //sv_animal_holdの出力をリセットした上で、D6番ピンに割り当て、パルス幅を500～2400マイクロ秒とする
    sv_object_hold.write(20);
    sv_minicar.write(20);
    sv_animal_hold.write(20);


    pinMode(dir_omu1, OUTPUT);
    pinMode(pwm_omu1, OUTPUT);

    ledcSetup(pwmch_omu1, 20000, 8);
    ledcAttachPin(pwm_omu1 , pwmch_omu1);
    

    pinMode(dir_omu2, OUTPUT);
    pinMode(pwm_omu2, OUTPUT);

    ledcSetup(pwmch_omu2, 20000, 8);
    ledcAttachPin(pwm_omu2 , pwmch_omu2);


    pinMode(dir_omu3, OUTPUT);
    pinMode(pwm_omu3, OUTPUT);

    ledcSetup(pwmch_omu3, 20000, 8);
    ledcAttachPin(pwm_omu3 , pwmch_omu3);
}

void loop() {
  // // Below has all accessible outputs from the controller
  int line_left = analogRead(34); 
  Serial.printf(" L = %d\n",line_left);
    // // // ライントレース用右のセンサーの値
  int line_right = analogRead(35);
  Serial.printf(" R = %d\n",line_right);
    // // // 曲がる用左のセンサーの値
  int turn_left = analogRead(27);
  Serial.printf(" turn_L = %d\n",turn_left);
    // // // 曲がる用右のセンサーの値
  int turn_right = analogRead(14);  
  Serial.printf(" turn_R = %d\n",turn_right);
  

  if (PS4.isConnected()) {

    if (PS4.Left()) {                           //左回転
      Serial.println("Left Button");
      int pwm = 64;
      digitalWrite(dir_omu1, LOW);    //逆転
      digitalWrite(dir_omu2, LOW);    //逆転
      digitalWrite(dir_omu3, LOW);    //逆転
      ledcWrite(pwmch_omu1, pwm);        
      ledcWrite(pwmch_omu2, pwm);       
      ledcWrite(pwmch_omu3, pwm);


    }
    if (PS4.Right()){                                  //右回転
       Serial.println("Right Button");
      int pwm = 64;
      digitalWrite(dir_omu1, HIGH);    //正転
      digitalWrite(dir_omu2, HIGH);    //正転
      digitalWrite(dir_omu3, HIGH);    //正転
      ledcWrite(pwmch_omu1, pwm);        
      ledcWrite(pwmch_omu2, pwm);       
      ledcWrite(pwmch_omu3, pwm);

    }

    if (PS4.Down()) {                          //建材を下ろす
      Serial.println("Down Button");
      int pwm = 256;
      digitalWrite(dir_block_updown, LOW);     //逆転
      ledcWrite(pwmch_block_updown, pwm); 
    }
    if (PS4.Up()){                              //建材を立てる
       Serial.println("Up Button");
      
      int pwm = 256;
      digitalWrite(dir_block_updown, HIGH);     //正転
      ledcWrite(pwmch_block_updown, pwm); 
      
    }
    if((PS4.Down() == 0 && PS4.Up() == 0)){
      ledcWrite(pwmch_block_updown, 0);        //停止
    }
    
    
    //   //Serial.println("Cross Button"); 
      if(PS4.Cross()){                  //建材を離す
         if(i_obj >= 0){
            sv_object_hold.write(20 + i_obj);
            i_obj--;
            delay(50);
      }   
     }
      if(PS4.Circle()){
     if(i_obj < 140){
        sv_object_hold.write(20 + i_obj) ;
        i_obj++;
        delay(50);
     }           
    }   
    if (PS4.Triangle()){                                  //ミニカー発射
       Serial.println("Triangle Button");
         for(int i = 0; i <= 120; i++){                //iの初期値は600；2000未満の場合は次の{}内を実行する；iに1を足す
            sv_minicar.write(20 + i) ;
            delay(10);
     }
    }
    if (PS4.Square()) {                                    //ミニカー戻す
      Serial.println("Square Button");
       for(int i = 120; i >= 0; i--) {                 //iの初期値は2000；600より大きい場合は次の{}内を実行する；iから1を引く
           sv_minicar.write(20 + i) ;
            delay(10);
    }
  }

    // if (PS4.UpRight()) Serial.println("Up Right");
    // if (PS4.DownRight()) Serial.println("Down Right");
    // if (PS4.UpLeft()) Serial.println("Up Left");
    // if (PS4.DownLeft()) Serial.println("Down Left");

    if (PS4.L1()){                                     //動物離す
       Serial.println("L1 Button");
                       //iの初期値は2000；600より大きい場合は次の{}内を実行する；iから1を引く
            if(i_ani >= 0){
            sv_animal_hold.write(20 + i_ani);
            i_ani--;
            delay(10);
      }   
    }
    if (PS4.R1()){                                    //動物つかむ
      Serial.println("R1 Button");   //iの初期値は2000；600より大きい場合は次の{}内を実行する；iから1を引く
        if(i_ani < 140){
        sv_animal_hold.write(20 + i_ani) ;
        i_ani++;
        delay(10);
        }
       }
    // if (PS4.L3()) Serial.println("L3 Button");
    // if (PS4.R3()) Serial.println("R3 Button");
    //if (PS4.PSButton())Serial.println("PS Button");
    
    // if (PS4.Share()) {          //自律運転終了
    //   Serial.println("Share Button");
    //   X = false;
    // }
    if (PS4.Options()) {//自律運転開始

      Serial.println("Options Button =");
      Serial.print(PS4.Options());
    //   X = true;
      
    //   while(X == true){
        
    //   int i = 0;
    //   if(i == 0){  //まっすぐ
    //   digitalWrite(dir_omu2, LOW);    //正転
    //   digitalWrite(dir_omu3, HIGH);     //逆転
    //   ledcWrite(pwmch_omu1, 0);        //停止
    //   ledcWrite(pwmch_omu2, 128);       
    //   ledcWrite(pwmch_omu3, 128);
    //   delay(1500);
    //   i++;
    //   }
    //   if(i == 1){           //右に90°
    //    int pwm = 64;
    //   digitalWrite(dir_omu1, HIGH);    //正転
    //   digitalWrite(dir_omu2, HIGH);    //正転
    //   digitalWrite(dir_omu3, HIGH);    //正転
    //   ledcWrite(pwmch_omu1, pwm);        
    //   ledcWrite(pwmch_omu2, pwm);       
    //   ledcWrite(pwmch_omu3, pwm);
    //   delay(1400);
    //   i++;
    //   }
    //   if(i == 2){           //まっすぐ

    //   digitalWrite(dir_omu2, LOW);    //正転
    //   digitalWrite(dir_omu3, HIGH);     //逆転
    //   ledcWrite(pwmch_omu1, 0);        //停止
    //   ledcWrite(pwmch_omu2, 128);       
    //   ledcWrite(pwmch_omu3, 128);
    //   delay(1600);
    //   i++;
    //   }
    //   if(i == 3){            //右に90°
    //    int pwm = 64;
    //   digitalWrite(dir_omu1, HIGH);    //正転
    //   digitalWrite(dir_omu2, HIGH);    //正転
    //   digitalWrite(dir_omu3, HIGH);    //正転
    //   ledcWrite(pwmch_omu1, pwm);        
    //   ledcWrite(pwmch_omu2, pwm);       
    //   ledcWrite(pwmch_omu3, pwm);
    //   delay(1400);
    //   i++;
    //   }
    //   if(i == 4){
    //   digitalWrite(dir_omu2, LOW);    //正転
    //   digitalWrite(dir_omu3, HIGH);     //逆転
    //   ledcWrite(pwmch_omu1, 0);        //停止
    //   ledcWrite(pwmch_omu2, 128);       
    //   ledcWrite(pwmch_omu3, 128);
    //   delay(7000);
    //   i++;
    //   }
    //   if(i == 5){
    //   ledcWrite(pwmch_omu1, 0);        //停止
    //   ledcWrite(pwmch_omu2, 0);       
    //   ledcWrite(pwmch_omu3, 0);
    //   X = false;
    //   }
    //   if (PS4.Share()) {          //自律運転終了
    //   Serial.println("Share Button");
    //   X = false;
    // }
    // }



      X = true;
      while(X == true){
       // ライントレース用左のセンサーの値
      int line_left = analogRead(34);  
      // ライントレース用右のセンサーの値
      int line_right = analogRead(35);
      // 曲がる用左のセンサーの値
      int turn_left = analogRead(27);
      // // 曲がる用右のセンサーの値
      int turn_right = analogRead(14); 
      
      int cross_r = 0;     //十字のところを通る回数
      int cross_l = 0;     //十字のところを通る回数

       if (PS4.Share()) {          //自律運転終了
      Serial.println("Share Button");
      X = false;
    }
          
    Serial.printf(" L = %d\n",line_left);
    Serial.printf(" R = %d\n",line_right);
    Serial.printf(" turn_R = %d\n",turn_right);
    Serial.printf(" turn_L = %d\n",turn_left);
    
    // Serial.print("cross = %d\n", cross);
    
    ledcWrite(pwmch_omu1, 0);        //停止
    ledcWrite(pwmch_omu2, 0);        //停止
    ledcWrite(pwmch_omu3, 0);        //停止

    for(int i=0 ; i < 1 ; i++){
       incre_pro();
     }
    
    if(turn_left <= threshold){  //十字に来たら曲がる
      cross_l++;
    }
    if(turn_right <= threshold){
      cross_r++;
    }
      
      if(cross_l == 1 && cross_r ==1){         //時計回りに90°回転
        int pwm = 64;
        digitalWrite(dir_omu1, HIGH);    //正転
        digitalWrite(dir_omu2, HIGH);    //正転
        digitalWrite(dir_omu3, HIGH);    //正転
        ledcWrite(pwmch_omu1, pwm);        
        ledcWrite(pwmch_omu2, pwm);       
        ledcWrite(pwmch_omu3, pwm);
        delay(turn);        //適宜変更
        incre_pro();
        cross_r++;
        cross_l++;
      }

      if(cross_l == 3 && cross_r ==3){         //時計回りに90°回転
      int pwm = 64;
        digitalWrite(dir_omu1, HIGH);    //正転
        digitalWrite(dir_omu2, HIGH);    //正転
        digitalWrite(dir_omu3, HIGH);    //正転
        ledcWrite(pwmch_omu1, pwm);        
        ledcWrite(pwmch_omu2, pwm);       
        ledcWrite(pwmch_omu3, pwm);
        delay(turn);        //適宜変更
        incre_pro();
        cross_r++;
        cross_l++;
      }
      if(cross_l == 5 && cross_r ==5){
        decre_pro();
        ledcWrite(pwmch_omu1, 0);        //停止
        ledcWrite(pwmch_omu2, 0);        //停止
        ledcWrite(pwmch_omu3, 0);        //停止
        X = false;
      }
    
    
    // 直進
    if(line_left > threshold && line_right > threshold){
      digitalWrite(dir_omu2, LOW);    //正転
      digitalWrite(dir_omu3, HIGH);     //逆転
      ledcWrite(pwmch_omu1, 0);        //停止
      ledcWrite(pwmch_omu2, 128);       
      ledcWrite(pwmch_omu3, 128);
     // 直進  
    }else if(line_left <= threshold && line_right <= threshold){
      digitalWrite(dir_omu2, LOW);    //正転
      digitalWrite(dir_omu3, HIGH);     //逆転
      ledcWrite(pwmch_omu1, 0);        //停止
      ledcWrite(pwmch_omu2, 128);       
      ledcWrite(pwmch_omu3, 128);
      
    // 斜め左前へ  
    }else if(line_left <= threshold && line_right > threshold){
      digitalWrite(dir_omu1, HIGH);    //逆転
      digitalWrite(dir_omu2, LOW);    //正転
      digitalWrite(dir_omu3, HIGH);     //正転
      ledcWrite(pwmch_omu1, 64.0);  
      ledcWrite(pwmch_omu2, 142.72);       
      ledcWrite(pwmch_omu3, 78.72);

    // 斜め右前へ  
    }else if(line_right <= threshold && line_left > threshold){
      digitalWrite(dir_omu1, LOW);    //正転
      digitalWrite(dir_omu2, LOW);    //逆転
      digitalWrite(dir_omu3, HIGH);     //逆転
      ledcWrite(pwmch_omu1, 64.0);  
      ledcWrite(pwmch_omu2, 78.72);       
      ledcWrite(pwmch_omu3, 142.72);
      
  
  }else{
    // モーターの完全停止
    ledcWrite(pwmch_omu1, 0);        //停止
    ledcWrite(pwmch_omu2, 0);        //停止
    ledcWrite(pwmch_omu3, 0);        //停止
     X = false;    
  }
 } 
}

//     if (PS4.Touchpad()) {          //自律運転終了
//       Serial.println("Touch Pad Button");
//       X = false;
//     }






    if (PS4.L2()) {                                         //動物下ろす
      Serial.printf("L2 button at %d\n", PS4.L2Value());     
      int pwm = 128;
      digitalWrite(dir_animal_updown, LOW);     //逆転
      ledcWrite(pwmch_animal_updown, pwm); 
    }
    if (PS4.R2()) {                                         //動物持ち上げる
      Serial.printf("R2 button at %d\n", PS4.R2Value());
      int pwm = 128;
      digitalWrite(dir_animal_updown, HIGH);     //正転
      ledcWrite(pwmch_animal_updown, pwm); 
    }
     if ((PS4.L2() == 0 && PS4.R2() == 0)) {
       ledcWrite(pwmch_animal_updown, 0);        //停止
     }
     if(PS4.R3()){
       int pwm = 64;
        digitalWrite(dir_omu1, HIGH);    //正転
        digitalWrite(dir_omu2, HIGH);    //正転
        digitalWrite(dir_omu3, HIGH);    //正転
        ledcWrite(pwmch_omu1, pwm);        
        ledcWrite(pwmch_omu2, pwm);       
        ledcWrite(pwmch_omu3, pwm);
      
     }
     if(PS4.L3()){
      int pwm = 64;
        digitalWrite(dir_omu1, LOW);    //正転
        digitalWrite(dir_omu2, LOW);    //正転
        digitalWrite(dir_omu3, LOW);    //正転
        ledcWrite(pwmch_omu1, pwm);        
        ledcWrite(pwmch_omu2, pwm);       
        ledcWrite(pwmch_omu3, pwm);
     }






//     // if (!(PS4.R2())) {
//     //   ledcWrite(pwmch_animal_updown, 0);        //停止
//     // }
    

    //十字移動（予備）
    if(PS4.LStickX() > 64){                     //右
      pwm_omu1 = PS4.LStickX();
      digitalWrite(dir_omu1, LOW);
      ledcWrite(pwmch_omu1, pwm_omu1);        
      ledcWrite(pwmch_omu2, 1/2 * pwm_omu1);       
      ledcWrite(pwmch_omu3, 1/2 * pwm_omu1);
    }
    if(PS4.LStickX() < -64){                     //左
      pwm_omu1 = abs(PS4.LStickX());
      digitalWrite(dir_omu1, HIGH);
      ledcWrite(pwmch_omu1, pwm_omu1);        
      ledcWrite(pwmch_omu2, 1/2 * pwm_omu1);       
      ledcWrite(pwmch_omu3, 1/2 * pwm_omu1);
    }
    
  if(PS4.LStickY() > 64){                       //前
      pwm_omu2 = PS4.LStickY();
      pwm_omu3 = PS4.LStickY();
      digitalWrite(dir_omu2, LOW);
      digitalWrite(dir_omu3, HIGH);
      ledcWrite(pwmch_omu1, 0);  
      ledcWrite(pwmch_omu2, pwm_omu2);       
      ledcWrite(pwmch_omu3, pwm_omu3); 
  }
  if(PS4.LStickY() < -64){                    //後ろ
      pwm_omu2 = abs(PS4.LStickY());
      pwm_omu3 = abs(PS4.LStickY());
      digitalWrite(dir_omu2, HIGH);
      digitalWrite(dir_omu3, LOW);
      ledcWrite(pwmch_omu1, 0);  
      ledcWrite(pwmch_omu2, pwm_omu2);       
      ledcWrite(pwmch_omu3, pwm_omu3); 
  }


    if(distance() > 64){                    //並進移動
      Serial.printf("distance = %f\n" , distance());
      float Vx = PS4.RStickX();
      float Vy = PS4.RStickY();
      
      Serial.printf("right Stick x at %d\n", PS4.RStickX());
      Serial.printf("right Stick y at %d\n", PS4.RStickY());
      Serial.printf("Vx = %f\n",Vx);
      Serial.printf("Vy = %f\n",Vy);

      pwm_omu1 = Vx;
      pwm_omu2 = 1/2 * Vx + 1.73/2 * Vy;
      pwm_omu3 = -1/2 * Vx - 1.73/2 * Vy;
      

      Serial.printf("pwm_omu1 = %f\n" ,pwm_omu1);
      Serial.printf("pwm_omu2 = %f\n" ,pwm_omu2);
      Serial.printf("pwm_omu3 = %f\n" ,pwm_omu3);
      
      
     
      if(pwm_omu1 > 5){
        digitalWrite(dir_omu1, LOW);    //正転
      }
       else if(pwm_omu1 < -5){
        digitalWrite(dir_omu1, HIGH);    //逆転
      }
      if(pwm_omu2 > 5){
        digitalWrite(dir_omu2, LOW);    //正転
      }
      else if(pwm_omu2 < -5){
        digitalWrite(dir_omu2, HIGH);    //逆転
      }
      if(pwm_omu3 > 5){
        digitalWrite(dir_omu3, LOW);    //正転
      }
      else if(pwm_omu3 < -5){
        digitalWrite(dir_omu3, HIGH);    //逆転
      }
           ledcWrite(pwmch_omu1, abs(pwm_omu1));        
           ledcWrite(pwmch_omu2, abs(pwm_omu2));       
           ledcWrite(pwmch_omu3, abs(pwm_omu3));
    }
    else if(distance() <= 64){
           ledcWrite(pwmch_omu1, 0);        
           ledcWrite(pwmch_omu2, 0);       
           ledcWrite(pwmch_omu3, 0);
    }


//     // if (PS4.Charging()) Serial.println("The controller is charging");
//     // if (PS4.Audio()) Serial.println("The controller has headphones attached");
//     // if (PS4.Mic()) Serial.println("The controller has a mic attached");

//     // Serial.printf("Battery Level : %d\n", PS4.Battery());

//     Serial.println();
//     // This delay is to make the output more human readable
//     // Remove it when you're not trying to see the output
   
  }
  }
  



// put function declarations here:
int myFunction(int, int);

/// @brief 
