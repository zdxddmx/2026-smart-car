



extern float l_speed_out ;
extern float r_speed_out ;
extern float place_out;
extern int32 base_speed ;


void place_pid(int16 error);
void l_speed_pid(float aim_speed,float now_speed);
void r_speed_pid(float aim_speed,float now_speed);

float LowPassFilter(float current, float last, float alpha);


