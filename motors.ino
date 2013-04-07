int Drive_Motor(int torque)  {
  
  if(torque < 0 ) {
    int torque2 = map(torque,-1,-255,40,255);
    forward(torque2);
    //Serial.println(torque2);
  }
  
  if(torque > 0) {
    int torque2 = map(torque,1,255,40,255);
    backward(torque2);
    //Serial.println(torque2);
  }
    if((actAngle) = 0) {
    Stop(0);
    //Serial.println(torque2);
  }
  

  }
 
 void forward(int drive)
{
     analogWrite(speedpinA,drive);
     analogWrite(speedpinB,0.9*drive);
     digitalWrite(pinI4,LOW);//right wheel forward
     digitalWrite(pinI3,HIGH);
     digitalWrite(pinI2,LOW);//right wheel forward
     digitalWrite(pinI1,HIGH);
}

void backward(int drive)//
{
     analogWrite(speedpinA,drive);//input a simulation value to set the speed
     analogWrite(speedpinB,0.9*drive);
     digitalWrite(pinI4,HIGH);//right wheel backward
     digitalWrite(pinI3,LOW);
     digitalWrite(pinI2,HIGH);//left wheel backward
     digitalWrite(pinI1,LOW);
} 

void Stop(int torque)
{
     digitalWrite(speedpinA, torque); 
     digitalWrite(speedpinB, torque);
     digitalWrite(pinI4,LOW);//right wheel backward
     digitalWrite(pinI3,LOW);
     digitalWrite(pinI2,LOW);//left wheel backward
     digitalWrite(pinI1,LOW);
    
}    
