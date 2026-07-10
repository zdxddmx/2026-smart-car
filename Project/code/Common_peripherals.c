#include "zf_common_headfile.h"


//----------------------------------------------------------------蜂鸣器

void Buzzer_init(void)
{
	gpio_init(Buzzer_pin, GPO, 0, GPO_PUSH_PULL);
}

void Buzzer_check(int TIME)
{
	gpio_set_level(Buzzer_pin,1);//响
	system_delay_ms(TIME);
	gpio_set_level(Buzzer_pin,0);//不响
}



//----------------------------------------------------------------按键

uint8 key1_state=1;					//当前按键状态
uint8 key2_state=1;
uint8 key3_state=1;
uint8 key4_state=1;

uint8 key1_last_state=0;		//上一次按键状态
uint8 key2_last_state=0;
uint8 key3_last_state=0;
uint8 key4_last_state=0;

uint8 key1_flag;						//触发标志位
uint8 key2_flag;
uint8 key3_flag;
uint8 key4_flag;

void Key_init(void)
{
	gpio_init(KEY1, GPI, 1, GPI_PULL_UP);
	gpio_init(KEY2, GPI, 1, GPI_PULL_UP);
	gpio_init(KEY3, GPI, 1, GPI_PULL_UP);
	gpio_init(KEY4, GPI, 1, GPI_PULL_UP);
	
	gpio_init(Switch1, GPI, 1, GPI_FLOATING_IN);
	gpio_init(Switch2, GPI, 1, GPI_FLOATING_IN);

}

void Key_scan(void)
{
	  //保存按键状态
    key1_last_state=key1_state;
    key2_last_state=key2_state;
    key3_last_state=key3_state;
    key4_last_state=key4_state;
	
	  //检测当前按键状态
    key1_state=gpio_get_level(KEY1);
    key2_state=gpio_get_level(KEY2);
    key3_state=gpio_get_level(KEY3);
    key4_state=gpio_get_level(KEY4);
	
		//比较前后两次按键状态
	  if(key1_state && !key1_last_state )   {key1_flag=1;}
    if(key2_state && !key2_last_state )   {key2_flag=1;}
    if(key3_state && !key3_last_state )   {key3_flag=1;}
    if(key4_state && !key4_last_state )   {key4_flag=1;}
	
}


//----------------------------------------------------------------轮驱DRV8701

void DRV8701_init(void)
{
		gpio_init(L_DIR, GPO, 1, GPO_PUSH_PULL);//L
		pwm_init(L_PWM,17000,0);
	
		gpio_init(R_DIR, GPO, 1, GPO_PUSH_PULL);//R
		pwm_init(R_PWM,17000,0);
}

void DRV8701_D_motor_ctrl(int32 L_SPEED,int32 R_SPEED)//开环驱动函数
{
	L_SPEED=L_SPEED>M_MAX?M_MAX:(L_SPEED<M_MIN)?M_MIN:L_SPEED;//左限幅
	R_SPEED=R_SPEED>M_MAX?M_MAX:(R_SPEED<M_MIN)?M_MIN:R_SPEED;//右限幅

	if(L_SPEED>0)//正转
	{
		gpio_set_level(L_DIR,1);
		pwm_set_duty(L_PWM,L_SPEED);
	}
	else//反转
	{
		gpio_set_level(L_DIR,0);
		pwm_set_duty(L_PWM,-L_SPEED);
	}

	if(R_SPEED>0)//正转
	{
		gpio_set_level(R_DIR,1);
		pwm_set_duty(R_PWM,R_SPEED);
	}
	else//反转
	{
		gpio_set_level(R_DIR,0);
		pwm_set_duty(R_PWM,-R_SPEED);
	}

}

//----------------------------------------------------------------负压电机
//void FY_init(void)
//{
//		pwm_init(FY_pin,17000,0);
//}

//void FY_S_motor_ctrl(int SPEED)
//{
//		pwm_set_duty(FY_pin,SPEED);
//}

//void NEG_motor_ctrl(int32 SPEED, int32 step)
//{


//    static int32 cur = 0;

//    if (SPEED > cur)
//    {
//        cur += step;
//        if (cur > SPEED) cur = SPEED;
//    } else if (SPEED < cur)
//    {
//        cur -= step;
//        if (cur < SPEED) cur = SPEED;
//    }

//    SPEED=SPEED>MAX_DRIVE_DUTY?MAX_DRIVE_DUTY:SPEED<MIN_DRIVE_DUTY?MIN_DRIVE_DUTY:SPEED;

//    pwm_set_duty(FY_pin, cur);
//}



//----------------------------------------------------------------无刷负压电机

void FY_init(void)
{
	pwm_init(FY_pin,50,0);

	
}





// @brief   设置无刷电机转速（百分比）
// @param   percent   油门 0~100（0=停转，100=满速），超出自动钳到100
//-------------------------------------------------------------------------------------------------------------------
void bldc_set_speed(uint8 percent)
{
    // 计算无刷电调转速   （1ms - 2ms）/20ms * 10000（10000是PWM的满占空比时候的值）
    // 在50Hz的控制频率下，无刷电调转速 0%   为 500
    // 在50Hz的控制频率下，无刷电调转速 20%  为 600
    // 在50Hz的控制频率下，无刷电调转速 40%  为 700
    // 在50Hz的控制频率下，无刷电调转速 60%  为 800
    // 在50Hz的控制频率下，无刷电调转速 80%  为 900
    // 在50Hz的控制频率下，无刷电调转速 100% 为 1000

    if (percent > 100)
    {
        percent = 100;                                 // 简单防呆
    }
    pwm_set_duty(FY_pin, BLDC_DUTY_MIN + (uint16)percent * 5);   // 500 + percent*5
}







//----------------------------------------------------------------





//----------------------------------------------------------------带方向编码器-蓝色

int16 LA_SPEED=0;
int16 RA_SPEED=0;
int16 A_SPEED=0;
int16 Distance=0;

void Dir_encoder_init(void)
{
    encoder_dir_init(ENCODER_DIR_1, ENCODER_DIR_DIR_1, ENCODER_DIR_PULSE_1);   	// 初始化编码器模块与引脚 带方向增量编码器模式
    encoder_dir_init(ENCODER_DIR_2, ENCODER_DIR_DIR_2, ENCODER_DIR_PULSE_2);    // 初始化编码器模块与引脚 带方向增量编码器模式
}

void Dir_encoder_pulse_get(void)
{
    LA_SPEED = -encoder_get_count(ENCODER_DIR_1);                  // 获取编码器计数
    RA_SPEED = encoder_get_count(ENCODER_DIR_2);              	// 获取编码器计数
	
	  A_SPEED=(LA_SPEED+RA_SPEED)/2;
	
		if(HD_flag==1)
		{
			 Distance+=A_SPEED;
		}

    encoder_clear_count(ENCODER_DIR_1);                                		// 清空编码器计数
    encoder_clear_count(ENCODER_DIR_2);                             		// 清空编码器计数

}

void Encoder_text(void)//电机测试
{
				
       static int16 L_SPEED=0,R_SPEED=0;
	
		HD_flag=1;

       Key_scan();

          if(key1_flag)//左电机+500
             {
                 key1_flag=0;
                 L_SPEED+=500;
             }
          if(key2_flag)//左电机-500
             {
                 key2_flag=0;
                 L_SPEED-=500;
             }
          if(key3_flag)//右电机+500
             {
                 key3_flag=0;
                 R_SPEED+=500;
             }
          if(key4_flag)//右电机-500
             {
                 key4_flag=0;
                 R_SPEED-=500;
             }


          ips114_show_string(8*0,16*1, "Encoder_text");

          ips114_show_string(8*0,16*3, "L:");          ips114_show_int16(8*7,  16*3,L_SPEED);
          ips114_show_string(8*0,16*4, "L_A:");        ips114_show_int16(8*7,  16*4,LA_SPEED);

          ips114_show_string(8*0,16*5, "R:");          ips114_show_int16(8*7,  16*5,R_SPEED);
          ips114_show_string(8*0,16*6, "R_A:");        ips114_show_int16(8*7,  16*6,RA_SPEED);
						 
	        ips114_show_string(8*0,16*7, "DIS:");   		 ips114_show_int16(8*7,  16*7,Distance);					 
						 
//					printf("encoder_data_dir_1 counter %d .\r\n", LA_SPEED);     // 输出编码器计数信息
//					printf("encoder_data_dir_2 counter %d .\r\n", RA_SPEED);     // 输出编码器计数信息

          DRV8701_D_motor_ctrl(L_SPEED,R_SPEED);
}

//----------------------------------------------------------------遥控
void Ctrl_init(void)
{
	gpio_init(IO_P77, GPI, 0, GPI_PULL_UP);
	gpio_init(IO_P11, GPI, 0, GPI_PULL_UP);

}


