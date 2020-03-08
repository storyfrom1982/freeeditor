#pragma once


#include <sstream>


namespace  freee{
class Bitstream{
public:
	Bitstream(const unsigned char *buf, int len );
	Bitstream( );
	 ~Bitstream();

	Bitstream & operator << (unsigned int v);
	Bitstream & operator >> (unsigned int& v);
	Bitstream & operator << (int v);
	Bitstream & operator >> (int& v);

	Bitstream & operator << (unsigned char v);
	Bitstream & operator >> (unsigned char& v);
	Bitstream & operator << (char v);
	Bitstream & operator >> (char& v);

	Bitstream & operator << (unsigned short v);
	Bitstream & operator >> (unsigned short& v);
	Bitstream & operator << (short v);
	Bitstream & operator >> (short& v);

	Bitstream & write( int value, int bits);
	Bitstream & read( int& value, int bits);
	int write( unsigned char* value, int bytes);
	int read( char* value, int bytes);
	std::string  getBuffer(){
		return m_ar.str();
	}

	int readUE(int &value);
	int readSE(int &value);
	int writeUE(int value);
	int writeSE(int &value);
	long long getReadBytes(){
		return m_ar.tellg();
	}
	long long getWriteBytes(){
		return m_ar.tellp();
	}
	long long getLeftBytes(){
		return m_ar.tellp() - m_ar.tellg();
	}
	long long getLeftBits(){
		return (m_ar.tellp() - m_ar.tellg())*8+m_tempLen;
	}
	int getTempBits(){
		return m_tempLen;
	}
	void skipg(int bytes);
protected:
	bool  needRead();
	bool  needWrite();
	std::stringstream m_ar;
	int          m_tempLen;
	unsigned char m_tempBuffer1;
	
};

}
 
 
