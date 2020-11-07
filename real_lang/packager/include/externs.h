/**
 * @file externs.h
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once
#ifndef externs_h
#define externs_h

extern const int syntax_total = 30;
extern const char syntax[syntax_total][8] = {"call","add","sub","mul","div","inc","dec","sqrt","sin","cos","tan","floor","ceil","rnd","glbl","parm","rt","ifeq","ifneq","ifls","ifmr","iflsq","ifmrq","ifelse","ifend","loop","loopend","break","rand","set"};
extern const unsigned int default_values_length = 10;
extern const char default_values[default_values_length][7] = {"rtv","rtv1","rtv2","rtv3","rtv4","PI","RADIAN","TRUE","FALSE","NULL"};
extern const char build_version_id[6] = "0000\0";

#endif
