void stopMoving();
void moveBigNose(int * prev, int next);
void moveSmallNose(int * prev, int next);
void moveXMotors();

int set_PWM_freq(long fre);
void PWM1_Start();
void PWM1_Stop();
void runPWM1(unsigned int duty, long freq);
void set_PWM1_duty(unsigned int duty, int Max_Duty);
void PWM2_Start();
void PWM2_Stop();
void runPWM2(unsigned int duty, long freq);
void set_PWM2_duty(unsigned int duty, int Max_Duty);

void step(int wait);
