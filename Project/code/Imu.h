

//结构体声明
typedef struct{
    float Xdata;   //零飘参数X
    float Ydata;   //零飘参数Y
    float Zdata;   //零飘参数Z
}gyro_param_t ;

typedef struct{
    float acc_x;   //x轴加速度
    float acc_y;   //y轴加速度
    float acc_z;   //z轴加速度

    float gyro_x;  //x轴角速度
    float gyro_y;  //y轴角速度
    float gyro_z;  //z轴角速度
}IMU_param_t ;

typedef struct{

    float  ROLL_1 ;
    float  ROLL_2 ;
    float  ROLL_3 ;

    float  Pitch_1;
    float  Pitch_2;
    float  Pitch_3;

    float  Yaw_1;
    float  Yaw_2;
    float  Yaw_3;
}Angle_type;

extern gyro_param_t  Gyro_Offset;
extern IMU_param_t   IMU_Data;
extern Angle_type    YAW;
extern Angle_type    PITCH;

extern int Imu_open_flag;

void Imu_init(void);
void Imu_gyro_offset_init(void);
void Imu_value_get(void);
void Imu_angle_get(void);
void Imu_handle_0(void);
void Imu_handle_x(void);
void Imu_handle_180(void);
void Imu_text(void);