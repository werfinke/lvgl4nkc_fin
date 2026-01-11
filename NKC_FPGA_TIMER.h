//#ifndef NKC_TIMER
//#define NKC_TIMER

//smed's timer lib, Feb 2025

typedef unsigned char  byte; // .b
typedef   signed short word; // .w

// Define bit manipulation macros
#define SET_BIT(value, bit)    ((value) |= (1 << (bit)))   // Set bit
#define CLEAR_BIT(value, bit)  ((value) &= ~(1 << (bit)))  // Clear bit
#define TOGGLE_BIT(value, bit) ((value) ^= (1 << (bit)))   // Toggle bit
#define CHECK_BIT(value, bit)  (((value) >> (bit)) & 1)    // Check bit

/*#ifdef NKC
	#if CPU == 1
		#define TIMER1_CTRL BYTE_AT(0xFFFFFFF4)
		#define TIMER1_HI   BYTE_AT(0xFFFFFFF5)
		#define TIMER1_LO   BYTE_AT(0xFFFFFFF6)
	#elif CPU == 2
		#define TIMER1_CTRL BYTE_AT(0xFFFFFFE8)
		#define TIMER1_HI   BYTE_AT(0xFFFFFFEA)
		#define TIMER1_LO   BYTE_AT(0xFFFFFFEC)
	#endif

#else
	//fake, otherise seg fault on WIN
	byte TIMER1_CTRL;
	word TIMER1_HI;
	word TIMER1_LO;
#endif*/

#define TRUE 1

#define RUN 0
#define TOVF 6
#define INTERRUPT_ENABLE 7

/* ----------------------------------------------------------------------------- TIMER ------------------------------------------------------------------------------------------------------------- */

//Doku von Andi:

//#define TIMER1_BASE       0xFFFFFFF4*cpu
//#define TIMER1_CTRL       0xFFFFFFF4*cpu
//#define TIMER1_HI         0xFFFFFFF5*cpu
//#define TIMER1_LO         0xFFFFFFF6*cpu	

//$FFFFFFF4 - Control Register 7 6 5 4 3 2 1 0   
//							   | | | | | | | |   
//							   | | | | | | |  --- Run(Timer run)   
//							   | | | | |  ------- WRM(Timer Register Write Mode)  
//							   | | | | |          0 = Write only reload register   
//							   | | | | |          1 = Write only timer register   
//							   | | | | |          2 = Write both   
//							   | | -------------  frei 
//							   | ---------------  TOVF Timer Overflow Flag. 
//							   |			      Muss per Software zur?ckgesetzt werden
//							   -----------------  IE(Interrupt enable)

// Das Counter und Reload Register ist je 16 bit breit. Der Timer z?lt mit 1 MHZ abw?ts. 
// Wenn das High - byte beschrieben wird dann
// wird der Wert in einem Zwischenregister gespeichert. Wird dann das Low - byte
// geschrieben wird dieser Wert zusammen mit dem zwischengespeicherten High - Byte
// in das Counter oder Reload (oder beide) ubernommen (somit werden immer alle 16 bit
// auf einmal geschrieben). Gelesen wird immer das Counter - Register. F?r den 
// Interrupt - Betrieb muss der Jumper auf der FPGA - Karte eingesetzt werden.

/* ----------------------------------------------------------------------------- TIMER ------------------------------------------------------------------------------------------------------------- */

void Timer_run() {

	CLEAR_BIT(TIMER1_CTRL, TOVF);
	SET_BIT(TIMER1_CTRL, RUN);
}

void Timer_stop() {

	CLEAR_BIT(TIMER1_CTRL, RUN);
}

void Timer_enable_interrupt(unsigned char status) {

	if (status) {
		SET_BIT(TIMER1_CTRL, INTERRUPT_ENABLE);
	} else {
		CLEAR_BIT(TIMER1_CTRL, INTERRUPT_ENABLE);
	}
}

void Timer_load(word value) {

	TIMER1_HI = (byte)(value >> 8);
	TIMER1_LO = (byte)(value);
	TIMER1_CTRL = (unsigned char)0b00000010;  //load only timer, clear overflow flag and disable interrupt, don't run
}

int Timer_read() {

	return (TIMER1_HI << 8) + TIMER1_LO;
}


void Timer_clear_overrun() {
	CLEAR_BIT(TIMER1_CTRL, TOVF);
}


byte Timer_read_ctrl() {

	return TIMER1_CTRL;
}

int Timer_lap() {

	int result = 0;  //max value = 0xFFFF + 0xFFFF = 131,070 @ 1Mhz = 131ms
	int value = (TIMER1_HI << 8) + TIMER1_LO;

	result = 0xFFFF - value;

	if (CHECK_BIT(TIMER1_CTRL, TOVF)) {
		result += 0xFFFF;
	} 

	Timer_load((word)0xFFFF);  
	Timer_run();

	return result;
}

//test interrupts
void Timer_test_interrupt() {

	Timer_load((word)0xFFFF);  //137 uS  =  7.26 kHz
	Timer_enable_interrupt(TRUE);
	Timer_run();

	//will run forever and issue interrupts on every overrun 
	while (TRUE) {
		Timer_clear_overrun();
	}	
}

