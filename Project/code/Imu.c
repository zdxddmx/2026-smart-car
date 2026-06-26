#include "zf_common_headfile.h"



gyro_param_t  Gyro_Offset;
IMU_param_t   IMU_Data;
Angle_type    YAW;
Angle_type    PITCH;


int Imu_open_flag=0;//IMU积分开启标志位

void Imu_init(void)
{
    imu660ra_init();
//	    imu660rb_init();

}

void Imu_value_get(void)
{
    imu660ra_get_gyro();
//    imu660rb_get_gyro();
	


    if(Imu_open_flag)
    {
        Imu_angle_get();
    }
		
	//	  IMU_Data.gyro_x=((float)imu660rb_gyro_x)/ 14.3 ;//原始值去掉了零飘以后除以转化系数变成角度制物理数据
  IMU_Data.gyro_x=((float)imu660ra_gyro_y)/ 16.4 ;//原始值去掉了零飘以后除以转化系数变成角度制物理数据
		
		Imu_handle_x();//俯仰角一直运行

}

void Imu_angle_get(void)//运动情况下执行
{
//    IMU_Data.gyro_x=((float)imu660ra_gyro_x)/ 14.3 ;//原始值去掉了零飘以后除以转化系数变成角度制物理数据
//    IMU_Data.gyro_y=((float)imu660ra_gyro_y)/ 14.3 ;
    IMU_Data.gyro_z=((float)imu660ra_gyro_z)/ 16.4 ;
	
//	  IMU_Data.gyro_x=((float)imu660rb_gyro_x)/ 14.3 ;//原始值去掉了零飘以后除以转化系数变成角度制物理数据
////    IMU_Data.gyro_y=((float)imu660rb_gyro_y)/ 14.3 ;
//    IMU_Data.gyro_z=((float)imu660rb_gyro_z)/ 14.3 ;

    Imu_handle_0();
//    Imu_handle_180();
}



void Imu_handle_0(void)//绝对角度
{

    if(IMU_Data.gyro_z<0.3&&IMU_Data.gyro_z>-0.3)//滤波
       {
           YAW.Yaw_1-=0;
       }
    else
      {
           YAW.Yaw_1-=(IMU_Data.gyro_z*0.005);//(积分过程)本来是逆时针为正,现在改为顺时针为正
      }
			
}


void Imu_handle_x(void)
{
				if(IMU_Data.gyro_x<0.3&&IMU_Data.gyro_x>-0.3)//滤波
			 {
					 PITCH.Pitch_1+=0;
			 }
    else
      {
           PITCH.Pitch_1+=(IMU_Data.gyro_x*0.005);//(积分过程)本来是逆时针为正,现在改为顺时针为正
      }
}

void Imu_handle_180(void)//[-180,180]
{
  if(IMU_Data.gyro_z<0.01&&IMU_Data.gyro_z>-0.01)//滤波
     {
         YAW.Yaw_2-=0;
     }
  else
    {
         YAW.Yaw_2-=(IMU_Data.gyro_z*0.005);//(积分过程)本来是逆时针为正,现在改为顺时针为正

        if(YAW.Yaw_2>180 && YAW.Yaw_2<=360)
          {
              YAW.Yaw_2-=360;
          }
          else if(YAW.Yaw_2<(-180) && YAW.Yaw_2>=(-360))
          {
              YAW.Yaw_2+=360;
          }
    }
}

void Imu_text(void)
{
    Imu_open_flag=1;//手动置1

    ips114_show_string(8*0,16*0," YAW");           ips114_show_float(8*12,16*0, YAW.Yaw_1,5,6);
	  ips114_show_string(8*0,16*1," PItch");           ips114_show_float(8*12,16*1,  PITCH.Pitch_1,5,6);

//    ips114_show_string(8*0,16*1," YAW.Yaw_2");           ips114_show_float(8*12,16*1, YAW.Yaw_2,5,6);
	
	
//		printf("%d,%d,%d\n",imu660rb_gyro_x, imu660rb_gyro_y, imu660rb_gyro_z);
//		printf("%d,%d,%d\n",imu660ra_gyro_x, imu660ra_gyro_y, imu660ra_gyro_z);

}





