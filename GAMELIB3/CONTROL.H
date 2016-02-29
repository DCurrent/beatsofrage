#ifndef	CONTROL_H
#define	CONTROL_H

// Generic control stuff (keyboard+joystick).


#define		CONTROL_JOY_1		256
#define		CONTROL_JOY_2		257
#define		CONTROL_JOY_3		258
#define		CONTROL_JOY_4		259
#define		CONTROL_JOY_UP		260
#define		CONTROL_JOY_DOWN	261
#define		CONTROL_JOY_LEFT	262
#define		CONTROL_JOY_RIGHT	263
#define		CONTROL_JOY_5		264
#define		CONTROL_JOY_6		265



#define		CONTROL_ESC		0x01
#define		CONTROL_DEFAULT_START	0x1C
#define		CONTROL_DEFAULT_UP	0xC8
#define		CONTROL_DEFAULT_DOWN	0xD0
#define		CONTROL_DEFAULT_LEFT	0xCB
#define		CONTROL_DEFAULT_RIGHT	0xCD
#define		CONTROL_DEFAULT_FIRE1	0x1D		// ctrl
#define		CONTROL_DEFAULT_FIRE2	0x38		// alt
#define		CONTROL_DEFAULT_FIRE3	0x2A		// lshift
#define		CONTROL_DEFAULT_FIRE4	0x36		// rshift
#define		CONTROL_DEFAULT_FIRE5	0x39		// spacebar
#define		CONTROL_DEFAULT_FIRE6	0x9D		// r-ctrl
#define		CONTROL_DEFAULT_SCREENSHOT	88	// F12




typedef struct{
	int		settings[32];
	unsigned long	keyflags, newkeyflags;
	int		kb_break;
}s_playercontrols;





void control_exit();
void control_init(int joy_enable);
int control_usejoy(int enable);
int control_getjoyenabled();

void control_setkey(s_playercontrols * pcontrols, unsigned int flag, int key);
int control_scankey();

char * control_getkeyname(unsigned int keycode);
void control_update(s_playercontrols ** playercontrols, int numplayers);


#endif

