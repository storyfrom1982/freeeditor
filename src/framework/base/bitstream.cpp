///////////////////////////////////////////////////////////
//  Bitstream.cpp
//  Implementation of the Class Bitstream
//  Created on:      2013-01-25 11:15:13
//  Original author: Yiping Yin (ypyin)
///////////////////////////////////////////////////////////


//#include "stdafx.h"
#include "bitstream.h"

using namespace freee;

Bitstream::Bitstream(const unsigned char *buf, int len )
	: m_ar(std::ios::binary | std::ios::in | std::ios::out )
	, m_tempLen(0)
	, m_tempBuffer1(0)
{
	m_ar.write((const char*)buf, len);
	m_ar.seekg(0, std::ios::beg);
	needRead();
}

Bitstream::Bitstream( )
	: m_ar(std::ios::binary | std::ios::in | std::ios::out )
	, m_tempLen(0)
	, m_tempBuffer1(0)
{
	 
}

Bitstream::~Bitstream()
{
}

Bitstream & Bitstream::operator << (unsigned int v){
	write( v, sizeof(v)*8);
	return *this;
}

Bitstream & Bitstream::operator >> (unsigned int& v) {
	read((int&)v, sizeof(v)*8);
	return *this;
}

Bitstream & Bitstream::operator << (int v){
	write( v, sizeof(v)*8);
	return *this;
}

Bitstream & Bitstream::operator >> (int& v) {
	read((int&)v, sizeof(v)*8);
	return *this;
}


Bitstream & Bitstream::operator << (unsigned short v){
	write( v, sizeof(v)*8);
	return *this;
}

Bitstream & Bitstream::operator >> (unsigned short& v) {
	int tmp;
	read(tmp, sizeof(v)*8);
	v = tmp;
	return *this;
}

Bitstream & Bitstream::operator << (short v){
	write( v, sizeof(v)*8);
	return *this;
}

Bitstream & Bitstream::operator >> (short& v) {
	int tmp;
	read(tmp, sizeof(v)*8);
	v = tmp;
	return *this;
}


Bitstream & Bitstream::operator << (unsigned char v){
	write( v, sizeof(v)*8);
	return *this;
}

Bitstream & Bitstream::operator >> (unsigned char& v) {
	int tmp;
	read(tmp, sizeof(v)*8);
	v =  tmp;
	return *this;
}

Bitstream & Bitstream::operator << (char v){
	write( v, sizeof(v)*8);
	return *this;
}

Bitstream & Bitstream::operator >> (char& v) {
	int tmp;
	read(tmp, sizeof(v)*8);
	v = tmp;
	return *this;
}

int Bitstream::write( unsigned char* value, int bytes){
	for (int i=0; i<bytes; i++){
		write(value[i], 8);
	}
	return 0;
}

int Bitstream::read( char* value, int bytes){
	for (int i=0; i<bytes; i++){
		int v;
		read(v, 8);
		value[i] = v;
	}
	return 0;
}
 
void Bitstream::skipg(int bytes) {
	for (int i = 0; i<bytes; i++) {
		int v;
		read(v, 8);
	}
}

bool  Bitstream::needRead() {
	if ( m_ar.eof() )
		return false;
	int value = m_ar.get();
	if (value == -1)
		return false;
	m_tempBuffer1 = value;
	m_tempLen = 8;
	return true;
}

bool  Bitstream::needWrite(){
	m_ar.put(m_tempBuffer1);
	m_tempLen = 0;
	m_tempBuffer1 = 0;
	return true;
}

Bitstream & Bitstream::read( int& value, int bits){
	
	unsigned int mask;
	value = 0;
	while (  m_tempLen <= bits && !m_ar.eof()) 
	{
		mask = ~0;
		mask  <<= m_tempLen;
		value <<= m_tempLen;
		value |= m_tempBuffer1 & ~mask; 
		bits -= m_tempLen;
		needRead();
	}
	if ( m_tempLen >= bits ) 
	{
		mask = ~0;
		value <<= bits;
		mask  <<= bits;
		m_tempLen -= bits;
		value |= (m_tempBuffer1 >> m_tempLen)  & ~mask;
	}
	return *this;
}

Bitstream & Bitstream::write( int value, int bits){
	unsigned int mask;
	int   negBits;
	 
	while (  (negBits = 8 - m_tempLen) <=  bits) 
	{
		mask = ~0;
		mask <<= negBits;
		bits -= negBits;
		m_tempBuffer1 <<= negBits;
		m_tempBuffer1 |= (value >> bits) &~mask;
		
		needWrite();
	}
	if ( bits > 0 ) 
	{
		mask = ~0;
		mask <<= bits;
		m_tempBuffer1 <<= bits;
		m_tempLen += bits; 
		m_tempBuffer1 |= (value & ~mask);
	} 
	return *this;
}

int Bitstream::readUE(int &value){
	int v = 0;
	int i = 0;
	while (   i < 32 )   {
		if ( m_ar.eof() )
		   break;
	   read(v, 1);
	   if ( v > 0)
		   break;
	   i++;
	}
	read(v, i);
	value = v + (1<<i) -1  ;
	return 0;
}

int Bitstream::readSE(int &value){
	readUE(value);

	value = value & 0x1 ? (value+1)/2 : -(value/2);
	return value;
}

 int Bitstream::writeUE(int value){
	int temp = ++value;
	int sz = 0;
	while(temp>0) {
		temp >>= 1;
		sz++;
	}
	write(sz*2-1, value);
	return 0;
}


 int Bitstream::writeSE(int &value){
	 writeUE(value <= 0 ? -value*2 : value*2 - 1);
	 return 0;
 }
