

extern uint8 pit_0_state ;
extern uint8 pit_1_state ;
extern int32 TIME_1;

void pit_0_ms_init(int Time);
void pit_0_handler (void);


void pit_1_ms_init(int Time);
void pit_1_handler (void);