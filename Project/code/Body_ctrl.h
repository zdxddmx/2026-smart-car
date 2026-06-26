
extern int HD_flag;


typedef enum {
    S_NORMAL,           // 正常循迹
    S_OFF_TRACK,        // 丢线停车
    S_R_CIRC_PRE,         // 右环岛·预判
    S_R_CIRC_JIN,         // 右环岛·进入
    S_R_CIRC_INTO,        // 右环岛·驶入
    S_R_CIRC_CHU,         // 右环岛·驶出
	  S_L_CIRC_PRE,         // 左环岛·预判
    S_L_CIRC_JIN,         // 左环岛·进入
    S_L_CIRC_INTO,        // 左环岛·驶入
    S_L_CIRC_CHU,         // 左环岛·驶出
} state_t;



void A_navigation(void);
void State_scan(void);
void State_execution(void);