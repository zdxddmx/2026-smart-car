#include "zf_common_headfile.h"


int32 base_speed = 0;//1500占空比

float place_error = 0;
float place_last_error = 0;
float place_out = 0;
float place_last_out = 0;
float place_kp = 17;  //-10  -20 10  5
float place_kd =14;   //10  20  25  35  45  60
float place_gyro_kd = 0.2;  //40  -15  -35  -50  -60
//0.3
void place_pid(int16 error)//方向外环计算pid，函数输入为小车循迹的反馈误差
{
    float A= 0.9;//低通滤波系数,一般可以为0.9 or 0.8
    place_last_out = place_out;//更新上一次值
    place_last_error = place_error; //更新上一次值
    place_error = error;
//    place_out = place_kp * place_error + place_kd * (place_error - place_last_error) + place_gyro_kd * imu660rb_gyro_z;//最后加上角速度kd 与 滤波后的Z轴角速度 或者 原始值（不推荐，噪音比较大） 的乘积
	place_out = place_kp * place_error + place_kd * (place_error - place_last_error) + place_gyro_kd * imu660ra_gyro_z;//最后加上角速度kd 与 滤波后的Z轴角速度 或者 原始值（不推荐，噪音比较大） 的乘积

//	    place_out = place_kp * place_error + place_kd * (place_error - place_last_error) ;//最后加上角速度kd 与 滤波后的Z轴角速度 或者 原始值（不推荐，噪音比较大） 的乘积

    place_out = place_out * A + place_last_out * (1-A);
}



//float base_l_speed = 0;
float l_speed_error = 0;
float l_speed_last_error = 0;
float l_speed_out = 0;
float l_speed_last_out = 0;
float l_speed_ki = 2;
float l_speed_kp = 1;
void l_speed_pid(float aim_speed,float now_speed)
{
    float A = 0.9;//低通滤波系数,一般可以为0.9 or 0.8
    aim_speed = base_speed - place_out;//串上方向外环的输出值，实现改变期望速度，根据实际转向调整符号
    l_speed_last_out = l_speed_out;//更新上一次值
    l_speed_last_error = l_speed_error; //更新上一次值
    l_speed_error = aim_speed - now_speed;
    l_speed_out += l_speed_ki * l_speed_error + l_speed_kp * (l_speed_error - l_speed_last_error);
    l_speed_out = l_speed_out * A + l_speed_last_out * (1-A);//最后计算出速度环pwm输出，直接给电机控制函数
}

//float base_r_speed = 0;
float r_speed_error = 0;
float r_speed_last_error = 0;
float r_speed_out = 0;
float r_speed_last_out = 0;
float r_speed_ki = 2;
float r_speed_kp = 1;
void r_speed_pid(float aim_speed,float now_speed)
{
    float A = 0.9;//低通滤波系数,一般可以为0.9 or 0.8
    aim_speed = base_speed + place_out;//串上方向外环的输出值，实现改变期望速度，根据实际转向调整符号
    r_speed_last_out = r_speed_out;//更新上一次值
    r_speed_last_error = r_speed_error;  //更新上一次值
    r_speed_error = aim_speed - now_speed;
    r_speed_out += r_speed_ki * r_speed_error + r_speed_kp * (r_speed_error - r_speed_last_error);
    r_speed_out = r_speed_out * A + r_speed_last_out * (1-A);//最后计算出速度环pwm输出，直接给电机控制函数
}



/**
 * @brief  一阶低通滤波器（增量式，纯函数）
 * @param  current  当前采样值
 * @param  last     上一次滤波输出值
 * @param  alpha    滤波系数 [0, 1]，越小越平滑，越大响应越快
 * @return          本次滤波输出值
 * @note   首次使用时，last 应与 current 相同以完成初始化
 *         示例：filtered = LowPassFilter(raw, filtered, 0.2f);
 */
float LowPassFilter(float current, float last, float alpha)
{
    return last + alpha * (current - last);
}