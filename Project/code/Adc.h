#define FILTER_N    4                                                  //滤波深度


extern int16   Track_error;
extern int16   ALL_DG;                                   //电感总值
extern int16 	 L_ALL;
extern int16 	 R_ALL;

void adv_init(void);
void AD_Fitier(void);
float Adc_Normalize(int value, float min, float max);
int16 norma_lization(int16 value, int16 Max, int16 Min);
int I_Median_Average_Filter(int* arr, int times) ;
float Cha_BI_He_2(uint16 Date_1,uint16 Date_2,uint16 Date_3,uint16 Date_4);
int16 Deviation_Calculate(int16 l, int16 ls, int16 rs, int16 r);
void ADC_text(void);
void Read_adc(void);