/**
 * @file NexDraw.h
 *
 */

#ifndef __NEXDRAW_H__
#define __NEXDRAW_H__

#include "NexTouch.h"
#include "NexHardware.h"

bool NexDrawFill(uint32_t x,uint32_t y,uint32_t w,uint32_t h,uint32_t color);
bool NexDrawRect(uint32_t x1,uint32_t y1,uint32_t x2,uint32_t y2,uint32_t color);
bool NexDrawLine(uint32_t x1,uint32_t y1,uint32_t x2,uint32_t y2,uint32_t color);
bool NexDrawCirle(uint32_t x,uint32_t y,uint32_t radius,uint32_t color);
bool NexDrawDisc(uint32_t x,uint32_t y,uint32_t radius,uint32_t color);

#endif /* #ifndef __NEXDRAW_H__ */
