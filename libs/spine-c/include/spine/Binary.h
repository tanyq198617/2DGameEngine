#pragma  once
#include <string>

struct binary
{
	unsigned char* binary_data;
	int datalen;

	int index;
};

struct NewData
{
	char* tempData;
	int len;

};

binary* Binary_Create(const std::string& filename);
int readInt(bool optimizePositive);
int readInt();

const std::string readString();
bool readBoolean();
//void readUtf8_slow(int charCount, int charIndex, int b);
void readUtf8_slow( int charCount, int charIndex, int b ,const char* str);
void binary_dispose(binary* b);
static binary *Binary_new (void);
unsigned char* read();
short readShort();
binary* getBinary();

float readfloat();
int readInteger();
