#define log_okMSG() log_MSG('Y')
#define log_failMSG() log_MSG('F')
#define log_unkMSG() log_MSG('?')

#define log_str(p) log_str_ex(p,1)
#define log_strn(p) log_str_ex(p,0)

#define log_num(p) log_num_ex(p,1)
#define log_numn(p) log_num_ex(p,0)


void log_str_ex(char *text,uint8_t NL);
void log_redraw();
void log_busyMSG(char text[]);
void log_bar(uint8_t per);
void log_MSG(char status);
void itos(uint32_t num, char *output);
void log_num_ex(int32_t number,uint8_t tmp);


char DSP[4][24]={{""},{""},{""},{""}};
uint8_t busyMSG=0,bar=0;
