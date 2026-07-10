

#define Buzzer_pin  IO_P67

#define KEY1  			IO_P70
#define KEY2  			IO_P71
#define KEY3  			IO_P72
#define KEY4  			IO_P73
#define Switch1  		IO_P75
#define Switch2  		IO_P76

#define L_DIR  			IO_P60
#define L_PWM  			PWMA_CH2P_P62
#define R_DIR  			IO_P64
#define R_PWM  			PWMA_CH4P_P66

#define M_MAX				8000
#define M_MIN				-8000



//#define FY_pin			PWMB_CH3_P33

//#define MAX_DRIVE_DUTY   8000
//#define MIN_DRIVE_DUTY  -8000



//----------------------------------------------------------------无刷负压电机



#define FY_pin			PWMB_CH4_P77




// 50Hz下：0% → duty 500（停转），100% → duty 1000（满速）
#define BLDC_DUTY_MIN      (500)           // 0%   对应占空比
#define BLDC_DUTY_MAX      (1000)          // 100% 对应占空比




//----------------------------------------------------------------



#define ENCODER_DIR_1                 	(TIM0_ENCOEDER)                         // 正交编码器对应使用的编码器接口 这里使用QTIMER1的ENCOEDER1
#define ENCODER_DIR_DIR_1              	(IO_P35)            				 	// DIR 对应的引脚
#define ENCODER_DIR_PULSE_1            	(TIM0_ENCOEDER_P34)            			// PULSE 对应的引脚

#define ENCODER_DIR_2                 	(TIM3_ENCOEDER)                         // 带方向编码器对应使用的编码器接口 这里使用QTIMER1的ENCOEDER2
#define ENCODER_DIR_DIR_2           		(IO_P53)             					// DIR 对应的引脚
#define ENCODER_DIR_PULSE_2       			(TIM3_ENCOEDER_P04)            			// PULSE 对应的引脚


extern uint8 key1_flag;					
extern uint8 key2_flag;
extern uint8 key3_flag;
extern uint8 key4_flag;

extern int16 LA_SPEED;
extern int16 RA_SPEED;
extern int16 Distance;


void Buzzer_init(void);
void Buzzer_check(int TIME);

void Key_init(void);
void Key_scan(void);

void DRV8701_init(void);
void DRV8701_D_motor_ctrl(int32 L_SPEED,int32 R_SPEED);

//void FY_init(void);
//void FY_S_motor_ctrl(int SPEED);
//void NEG_motor_ctrl(int32 SPEED, int32 step);

void FY_init(void);
void bldc_set_speed(uint8 percent);

void Dir_encoder_init(void);
void Dir_encoder_pulse_get(void);
void Encoder_text(void);