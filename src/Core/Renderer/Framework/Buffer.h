#pragma once

/*
 *
 * Buffer.h
 * 
 * Type definitions for buffer handles
 * 
 */

typedef unsigned int hBuffer;

enum eBufferType
{
	Buffer_None = 0,
	Buffer_Vertex,
	Buffer_Index
};

struct sBuffer
{
	hBuffer handle;
	eBufferType type;
};