#ifndef _LOOP_LIB
        #define _LOOP_LIB


        #include <inttypes.h>

        #ifndef _NOP
                #define _NOP asm volatile("NOP");
        #endif

        #define delay delay_ms
        
        void delay_ms(uint16_t ms);
        void delay_us(unsigned long delay);//takes 8 cycles, at 16MHz means half of micro second (freq*time)/repeated

#endif
