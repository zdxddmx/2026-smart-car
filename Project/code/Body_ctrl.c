#include "zf_common_headfile.h"

int HD_flag=0;
int HD_TIME=0;
int Island_chamfer=0;//环岛切角


void A_navigation(void)//电磁导航
{
		State_scan();//状态扫描
		State_execution();//状态执行
}







void Normal_run(void)//普通循迹
{
		static float last_gyro_z = 0.0f;
		imu660ra_gyro_z=LowPassFilter(imu660ra_gyro_z, last_gyro_z, 0.1f);
		last_gyro_z=imu660ra_gyro_z;
	
//		static float last_gyro_z = 0.0f;
//		imu660rb_gyro_z=LowPassFilter(imu660rb_gyro_z, last_gyro_z, 0.1f);
//		last_gyro_z=imu660rb_gyro_z;
	
	
		place_pid(Track_error);	//转向环闭环	
	
//		DRV8701_D_motor_ctrl(1500-place_out,1700+place_out);//电机开环,转向闭环
		DRV8701_D_motor_ctrl(1000-place_out,1000+place_out);//电机开环,转向闭环
}
void Ins_run(void)//惯性循迹
{
		DRV8701_D_motor_ctrl(1400-(Island_chamfer-YAW.Yaw_1)*35,1400+(Island_chamfer-YAW.Yaw_1)*35);//这里的系数20决定着入环岛的时机,速度越快,这个值要适当变大
}

void Roundabout_Prepare(void)//待入环岛，此时离最佳入环岛点还有大概25CM
{
		gpio_set_level(Buzzer_pin,1);//响	
		Normal_run();//维持普通循迹
}

void Roundabout_inprogress(void)//执行环岛转向,这个动作是整个环岛中最重要的一步,是强行让车脱离电磁线引导,正常的赛道车是不会自然入环岛的
{
		Ins_run();//惯性导航
}


int Running_status;//状态标志位

void State_scan(void)//状态扫描
{

		
		if(R_ALL>L_ALL&&ALL_DG>=210)//检测到右环岛,但离最佳入环岛点还有点距离
		{
				HD_flag=1;	//开启距离积分
			  Island_chamfer=-20;//左环岛切角
				Running_status=S_R_CIRC_PRE;

		}
		else if(L_ALL>R_ALL&&ALL_DG>=230)//检测到左环岛,但离最佳入环岛点还有点距离
		{
				HD_flag=1;	//开启距离积分
				Island_chamfer=30;//右环岛切角
				Running_status=S_L_CIRC_PRE;
		
		}
		else if(Distance>=15558)//到达最佳环岛点-并开始转向			//		25CM-15558
		{	 
				Imu_open_flag=1;//开启角度积分
				HD_flag=0;//关闭距离积分
				Distance=0;//距离清0
			
				if(Island_chamfer==-20)//根据切角自行判断环岛执行
				{
						Running_status=S_R_CIRC_JIN;	
				}
				else
				{
						Running_status=S_L_CIRC_JIN;	
				}
						
		}
		else if(func_abs(YAW.Yaw_1)>=func_abs(Island_chamfer))//超过目标角度关闭惯性导航并清0对于标志位,这里写成绝对值是因为左环岛和右环岛的切角不一样,这样能巧妙的自适应，反正就是偏出去的角大于目标切角就结束这个状态
		{
				Imu_open_flag=0;//关闭角度积分
				YAW.Yaw_1=0;//角度清0
				Running_status=S_NORMAL;	//正常循迹
		}
		else if(HD_flag==0&&Imu_open_flag==0)
		{
				Running_status=S_NORMAL;	//正常循迹
		}

		
}


void State_execution(void)//状态执行
{
	switch (Running_status)
  {
  	case S_R_CIRC_PRE:  //准备入右环岛
					Roundabout_Prepare();
  		break;
		
  	case S_R_CIRC_JIN:		//到达右环岛最佳点
					Ins_run();
  		break;
		
		case S_L_CIRC_PRE:  //准备入左环岛
					Roundabout_Prepare();
			break;
		
  	case S_L_CIRC_JIN:		//到达左环岛最佳点
					Ins_run();
  		break;
		
//		case S_L_CIRC_INTO:	//执行环岛转向动作
//					Roundabout_inprogress();
//			break;
		
		case S_NORMAL:			//普通循迹
					Normal_run();
			break;
		
  	default:
  		break;
  }



}
