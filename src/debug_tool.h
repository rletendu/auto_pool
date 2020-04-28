
#ifndef __DEBUG_H__
#define __DEBUG_H__

void debug_init(void);
void debug_loop(void);



#define debugV(fmt, ...) rdebugVln(fmt, ##__VA_ARGS__)
#define debugD(fmt, ...) rdebugDln(fmt, ##__VA_ARGS__)
#define debugI(fmt, ...) rdebugIln(fmt, ##__VA_ARGS__)
#define debugW(fmt, ...) rdebugWln(fmt, ##__VA_ARGS__)
#define debugE(fmt, ...) rdebugEln(fmt, ##__VA_ARGS__)
#define debugA(fmt, ...) rdebugAln(fmt, ##__VA_ARGS__)


#endif // __DEBUG_H__