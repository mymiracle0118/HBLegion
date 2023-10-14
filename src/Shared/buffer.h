#ifndef BUFFER_H
#define BUFFER_H
#pragma once

#include	"typedefs.h"

void Push(char * &cp, uint8 value);
void Push(char * &cp, uint16 value);
void Push(char * &cp, uint32 value);
void Push(char * &cp, uint64 value);
void Push(char * &dest, char * src, uint32 len);

void Pop(char * &cp, uint8 &v);
void Pop(char * &cp, uint16 &v);
void Pop(char * &cp, uint32 &v);
void Pop(char * &cp, uint64 &v);
void Pop(char * &src, char * dest, uint32 len);

#endif //BUFFER_H