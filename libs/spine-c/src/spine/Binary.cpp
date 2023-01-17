#include <spine/Binary.h>
#include <stdio.h>
#include <ctype.h>
#include <spine/extension.h>
#include <iostream>
#include <fstream>
#include <assert.h>

using namespace std;

static binary* m_Binary=nullptr;
//static NewData* m_data;


int readInt( bool optimizePositive)
{
	int b=*read();
	int result = (b) & 0x7F;
	if ((b & 0x80) != 0) {
		b=*read();
		result=result | ((b & 0x7F) << 7);
		if ((b & 0x80) != 0) {
			b=*read();
			result=result | ((b & 0x7F) << 14);
			if ((b & 0x80) != 0) {
			    b=*read();
				result=result | ((b & 0x7F) << 21);
				if ((b & 0x80) != 0) {
					b=*read();
					result=result | ((b & 0x7F) << 28);
				}
			}
		}
	}
	return optimizePositive ? result : (((unsigned int)(result) >> 1)^-(result & 1));
}

int readInteger()
{

	int ch1 = *read();
	int ch2 = *read();
	int ch3 = *read();
	int ch4 = *read();
	assert((ch1 | ch2 | ch3 | ch4) >= 0);
		
	
	return ((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0));

}

short readShort() 
{
	int ch1 = *read();
	int ch2 = *read();
	if ((ch1 | ch2) < 0)
	{
        
	}
	return (short)((ch1 << 8) + (ch2 << 0));
}


unsigned char* read()
{
	/*std::string filePath =cocos2d::FileUtils::getInstance()->fullPathForFilename("skeleton.skel");
	static std::FILE* fout=fopen(filePath.c_str(),"r");
	ifstream fin;
	fin.open(filePath,ifstream::binary);
	int m = 0;
	bool fss;
	fin.read((char *)&fss,sizeof(bool));
	fin.read((char *)&m,sizeof(int));*/

	assert(m_Binary!=nullptr);
	assert(m_Binary->index<=m_Binary->datalen-1);
	unsigned char* data=m_Binary->binary_data;
	data +=m_Binary->index;
	m_Binary->index++;
	return data;

}

const std::string readString()
{
	int charCount = readInt(true);
	if (charCount==0)
	{
		return std::string("");
	}
	charCount--;
	std::string name;
	int charIndex = 0;
	int b = 0;
	while (charIndex < charCount) {
		b = *read();
		if (b > 127) break;
		
		char code=b;
		name.append(1,code);
		++charIndex;
		//free((void*)code);
		//name[charIndex++]=(char)b;
		//m_data->tempData[charIndex++] = (char)b;
	}
	// If a char was not ASCII, finish with slow path.
	if (charIndex < charCount) {
        name.resize(100);
        readUtf8_slow(charCount, charIndex, b,name.c_str());
        name.resize(strlen(name.c_str()));
    }
    
    //memcpy(name,m_data->tempData,charCount);
	//delete m_data;
	
	return name;
}

static binary *Binary_new (void) {
	return (binary*)CALLOC(binary, 1);
}

binary* Binary_Create( const std::string& filename )
{
    int length;
    char* data = _spUtil_readFile(filename.c_str(), &length);

	assert(data && length);

	if (!m_Binary)
	{
		m_Binary=Binary_new();
	}
	//if (!m_Binary) return false; 
	/*m_data=new NewData;
	m_data->tempData=new char[32];
	m_data->len=32;*/
//	m_Binary->binary_data=(unsigned char*)malloc(sizeof(unsigned char)*length);
	//m_Binary->binary_data=data.getBytes();
    
    m_Binary->binary_data = reinterpret_cast<unsigned char *>(data);
	
	m_Binary->datalen=length;
	m_Binary->index=0;
	
	return m_Binary;

}

bool readBoolean()
{
	int ch = *read();
	assert(ch>=0);
		
	return (ch != 0);

}

void readUtf8_slow( int charCount, int charIndex, int b ,const char* str)
{
	char* chars = const_cast<char*> (str);
	while (true) {
		switch (b >> 4) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			chars[charIndex] = (char)b;
			break;
		case 12:
		case 13:
			chars[charIndex] = (char)((b & 0x1F) << 6 | (*read() )& 0x3F);
			break;
		case 14:
			chars[charIndex] = (char)((b & 0x0F) << 12 | (*read() & 0x3F) << 6 | (*read()) & 0x3F);
			break;
		}
		if (++charIndex >= charCount) break;
		b = (*read()) & 0xFF;
	}
    chars[charIndex] = 0;
}

float readfloat()
{
	int bits = readInteger();

	int s = ((bits >> 31) == 0) ? 1 : -1;
	int e = ((bits >> 23) & 0xff);
	int m = (e == 0) ?
		(bits & 0x7fffff) << 1 :(bits & 0x7fffff) | 0x800000;

	return (s*m*pow(2, e-150));
}


int readInt()
{
	int ch1 = *read();
	int ch2 = *read();
	int ch3 = *read();
	int ch4 = *read();
	if ((ch1 | ch2 | ch3 | ch4) < 0)
	{
        
	}
	return ((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0));
}

void binary_dispose( binary* b )
{
	if (b!=nullptr)
	{
		if (b->binary_data)
		{
			
			FREE(b->binary_data);
            b->binary_data = nullptr;
		}
	}
}

binary* getBinary()
{
	if(!m_Binary)
	{
		return NULL;
	}
	return m_Binary;
}





