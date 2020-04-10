/**
 * @file NexXfloat.cpp
 *
 * The implementation of class NexXfloat. 
 *
 * @author  huang xianming (email:<xianming.huang@itead.cc>)
 * @date    2015/8/13
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */
#include "NexSystem.h"

bool NexDim(uint8_t dim)
{
	char buf[10] = {0};
	itoa(dim, buf, 10);
	String cmd = String("dim=");
	cmd += buf;
	sendCommand(cmd.c_str());
	return recvRetCommandFinished();
}

bool NexSleep(void)
{
	sendCommand("thsp=10");
	recvRetCommandFinished();
	sendCommand("thup=1");
	recvRetCommandFinished();
	//sendCommand("sleep=1");
	//return recvRetCommandFinished();
}
