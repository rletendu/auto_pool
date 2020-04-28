/**
 * @file NexPage.cpp
 *
 * The implementation of class NexPage. 
 *
 * @author  Wu Pengfei (email:<pengfei.wu@itead.cc>)
 * @date    2015/8/13
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#include "NexPage.h"

NexPage::NexPage(uint8_t pid, uint8_t cid, const char *name)
    :NexTouch(pid, cid, name)
{
}

bool NexPage::show(void)
{
    uint8_t buffer[4] = {0};

    const char *name = getObjName();
    if (!name)
    {
        return false;
    }
    
    String cmd = String("page ");
    cmd += name;
    sendCommand(cmd.c_str());
    return recvRetCommandFinished();
}

bool GetPageId(uint8_t *page_id)
{
    uint32_t nb_ptleft;
    uint32_t val;
    bool ret;
    String cmd_alt = String("sendme");
    sendCommand(cmd_alt.c_str());
    ret = recvRetPageId(page_id);
    return ret; 
}
