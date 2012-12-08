#define log_okMSG() log_MSG('Y')
#define log_failMSG() log_MSG('F')
#define log_unkMSG() log_MSG('?')

#define log_str(p) log_str_ex(p,1)
#define log_strn(p) log_str_ex(p,0)


//template
/**************************************************************
 * name: 	
 * desc.:	
 * accetps:	
 * returns:	
 * note:	
 * TODO:	
 **************************************************************/

/**************************************************************
 * name: 	log_str_ex
 * desc.:	puts some string on display, with on/off scroll
 * accetps:	pointer on char array and numbe, nd. param decides to scroll or not (zero not, other scroll)
 * returns:	nothing
 * note:	-
 * TODO:	done
 **************************************************************/
void log_str_ex(char *text,uint8_t NL);

/**************************************************************
 * name: 	log_redraw
 * desc.:	loads frame buffer into display
 * accetps:	nothing
 * returns:	nothing
 * note:	automaticaly called by other log_* functions, should be changed if we don't wanna scroll output
 * TODO:	done
 **************************************************************/
void log_redraw();

/**************************************************************
 * name: 	log_busyMSG
 * desc.:	puts message on display with [B] (means BUSY) in the end, indicates status of something (eg. when waiting)
 * accetps:	pointed on array of chars
 * returns:	nothing
 * note:	next called function should be log_MSG to change status from BUSY to fail=F or ok=Y
 * TODO:	done
 **************************************************************/
void log_busyMSG(char text[]);

/**************************************************************
 * name: 	log_bar
 * desc.:	draws status bar on display with percentage
 * accetps:	number between 0 and 100 incl.
 * returns:	nothing
 * note:	for every redraw of bar, call this function untill 100% is reached
 * TODO:	simplyfy
 **************************************************************/
void log_bar(uint8_t per);

/**************************************************************
 * name: 	log_MSG
 * desc.:	called after log_busyMSG, changes BUSY sign B into another, use macros for it: log_failMSG() F, log_okMSG() Y, log_unkMSG() ?
 * accetps:	any character
 * returns:	nothing
 * note:	-
 * TODO:	done
 **************************************************************/
void log_MSG(char status);

/**************************************************************
 * name: 	itos
 * desc.:	converts signed long into array of characters
 * accetps:	signed long numbers, -2147483648 to 2147483647
 * returns:	address on buffer which stores output characters
 * note:	max lng. is 10 digits + 1 character (sign)
 * TODO:	simplify, too much modulo and divisions
 **************************************************************/
char *itos(int32_t num);

//allocate space for number conversions
char str_buffer[]="XXXXXXXXXXX\0";

//allocation of space for display MAP (needed for srolling etc.), sth. like frame buffer :-)
char DSP[4][24]={{""},{""},{""},{""}};

//variables stroing some long time data
uint8_t busyMSG=0,bar=0;
