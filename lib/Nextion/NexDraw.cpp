#include "NexDraw.h"


bool NexDrawFill(uint32_t x,uint32_t y,uint32_t w,uint32_t h,uint32_t color)
{
    String cmd = String("fill ");
    cmd += x;
    cmd += ',';
    cmd += y;
    cmd += ',';
    cmd += w;
    cmd += ',';
    cmd += h;
    cmd += ',';
    cmd += color;
    sendCommand(cmd.c_str());
    return recvRetCommandFinished();
}

bool NexDrawRect(uint32_t x1,uint32_t y1,uint32_t x2,uint32_t y2,uint32_t color)
{
    String cmd = String("draw ");
    cmd += x1;
    cmd += ',';
    cmd += y1;
    cmd += ',';
    cmd += x2;
    cmd += ',';
    cmd += y2;
    cmd += ',';
    cmd += color;
    sendCommand(cmd.c_str());
    return recvRetCommandFinished();
}

bool NexDrawLine(uint32_t x1,uint32_t y1,uint32_t x2,uint32_t y2,uint32_t color)
{
    String cmd = String("line ");
    cmd += x1;
    cmd += ',';
    cmd += y1;
    cmd += ',';
    cmd += x2;
    cmd += ',';
    cmd += y2;
    cmd += ',';
    cmd += color;
    sendCommand(cmd.c_str());
    return recvRetCommandFinished();
}

bool NexDrawCirle(uint32_t x,uint32_t y,uint32_t radius,uint32_t color)
{
    String cmd = String("cir ");
    cmd += x;
    cmd += ',';
    cmd += y;
    cmd += ',';
    cmd += radius;
    cmd += ',';
    cmd += color;
    sendCommand(cmd.c_str());
    return recvRetCommandFinished();
}

bool NexDrawDisc(uint32_t x,uint32_t y,uint32_t radius,uint32_t color)
{
    String cmd = String("cirs ");
    cmd += x;
    cmd += ',';
    cmd += y;
    cmd += ',';
    cmd += radius;
    cmd += ',';
    cmd += color;
    sendCommand(cmd.c_str());
    return recvRetCommandFinished();
}
