#include "zf_common_headfile.h"

int16   adc_date[4];                                //储存模拟量采集值
int16 	L_H=0,L_S=0,R_S=0,R_H=0;										//定义传感器单位名称
int16   ALL_DG=0;                                   //电感总值
int16 	L_ALL=0;
int16 	R_ALL=0;
int16   adc_min[4]={40,80,80,35};              		 //最小值  (给定一个需要)
int16   adc_max[4]={580, 580, 550, 570};       			//最大值

int16   Track_error=0;															//最终的赛道相对偏差

void adv_init(void)
{
	 adc_init(ADC_CH8_P00, ADC_10BIT);
	 adc_init(ADC_CH9_P01, ADC_10BIT);
	 adc_init(ADC_CH13_P05, ADC_10BIT);
	 adc_init(ADC_CH14_P06, ADC_10BIT);
}

void AD_Fitier(void)//滤波
{
	int16 i;

  int16 filter_buf_LH [FILTER_N];  
  int16 filter_buf_LS [FILTER_N];  

  int16 filter_buf_RS [FILTER_N];  
  int16 filter_buf_RH [FILTER_N];  

	//采集值
	for(i=0;i<FILTER_N;i++)
	{
		filter_buf_LH[i]= adc_mean_filter_convert(ADC_CH8_P00,10);
	  filter_buf_LS[i] =adc_mean_filter_convert(ADC_CH9_P01,10);

    filter_buf_RS[i] =adc_mean_filter_convert(ADC_CH13_P05,10);
    filter_buf_RH[i] =adc_mean_filter_convert(ADC_CH14_P06,10);
	}

//去除极值求平均
		adc_date[0] = I_Median_Average_Filter(filter_buf_LH,4);
		adc_date[1] = I_Median_Average_Filter(filter_buf_LS,4);

		adc_date[2] = I_Median_Average_Filter(filter_buf_RS,4);
		adc_date[3] = I_Median_Average_Filter(filter_buf_RH,4);
}


void Read_adc(void)
{
			AD_Fitier();//滤波
	
	    L_H=Adc_Normalize(adc_date[0],adc_min[0],adc_max[0]);
			L_S=Adc_Normalize(adc_date[1],adc_min[1],adc_max[1]);

			R_S=Adc_Normalize(adc_date[2],adc_min[2],adc_max[2]);
			R_H=Adc_Normalize(adc_date[3],adc_min[3],adc_max[3]);
	

			L_ALL=L_H+L_S;
			R_ALL=R_H+R_S;
			ALL_DG=L_ALL+R_ALL;

	    Track_error=Deviation_Calculate(L_H,L_S,R_S,R_H);//将四路电感分成左右然后根号差比和计算赛道误差

}



//int16 Adc_Normalize(int16 value, int16 min, int16 max) 
//{ 
//    int16 result;
//    
//    // 边界保护（防止负值和超出范围）
//    if (value <= min) return 1;
//    if (value >= max) return 100;
//    
//    // 计算归一化值
//    result = 1 + (value - min) * 99 / (max - min);
//    
//    // 二次保险（防止整数运算异常）
//    if (result < 1) return 1;
//    if (result > 100) return 100;
//    
//    return result;
//}
 
 /** 
 * @brief 归一化函数，将输入值映射到1-100范围 
 *  
 * @param value 输入初始电感值（此时为整型） 
 * @return float 归一化后的值（1-100） 
 */ 
 
 
float Adc_Normalize(int value, float min, float max) 
{ 
    float normalized = 0; 
     
    normalized = (float)(value - min) / (max - min) * 100.0f;  // 计算归一化值 
     
    // 限幅保护，确保返回值在1-100范围内 
    return normalized >= 100.0 ? 100.0 : (normalized < 1.0 ? 1.0 : normalized); 
}




/** 
* 中位值平均滤波函数 
* @param arr 输入数据数组 
* @param times 数据采样次数 
* @return 滤波后的平均值 
*/ 
int I_Median_Average_Filter(int* arr, int times) 
{ 
		int min = arr[0], max = arr[0], sum = 0; 
		int i = 0; 

		// 遍历数组，找出最大值、最小值并求和 
		for (i = 0; i < times; i++) 
		{ 
		if (arr[i] < min) min = arr[i]; 
		if (arr[i] > max) max = arr[i]; 
		sum += arr[i]; 
		} 
// 计算去掉最大值和最小值后的平均值 
return (sum - min - max) / (times - 2); 
}


int16 Deviation_Calculate(int16 l, int16 ls, int16 rs, int16 r)
{
    #define A   1   // 直线循迹响应
    #define B   1   // 弯道内切强度
    #define C   1   // 弯道曲率补偿
    #define P   100 // 输出增益
    
    int32 sub = A * (l - r) + B * (ls - rs);
    int32 add = A * (l + r) + C * abs(ls - rs) + 1;
    
    return (int16)(sub * P / add);
}
void ADC_text(void)
{
//	ips114_show_string(0,16*0,"L_H");              ips114_show_int16(8*10,16*0,L_H);
//	ips114_show_string(0,16*1,"L_S");              ips114_show_int16(8*10,16*1,L_S);
//	ips114_show_string(0,16*2,"R_S");              ips114_show_int16(8*10,16*2,R_S);
//	ips114_show_string(0,16*3,"R_H");              ips114_show_int16(8*10,16*3,R_H);
	
	ips114_show_string(0,16*0,"L_H");              ips114_show_int32(8*10,16*0,adc_date[0],5);
	ips114_show_string(0,16*1,"L_S");              ips114_show_int32(8*10,16*1,adc_date[1],5);
	ips114_show_string(0,16*2,"R_S");              ips114_show_int32(8*10,16*2,adc_date[2],5);
	ips114_show_string(0,16*3,"R_H");              ips114_show_int32(8*10,16*3,adc_date[3],5);
	
	
    ips114_show_string(0,16*4,"T_e");              ips114_show_int32(8*10,16*4,Track_error,5);
    ips114_show_string(0,16*5,"ALL");              ips114_show_int32(8*10,16*5,L_H+L_S+R_S+R_H,5);
//				printf("%d,%d,%d.%d\r\n",L_H,L_S,R_S,R_H);

}
