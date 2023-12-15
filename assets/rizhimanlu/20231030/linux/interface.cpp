
#include <iostream>
#include <string.h>
#include "WzSerialPort.h"

#include "interface.h"

using namespace std;

void sendDemo()
{
	WzSerialPort w;
	if (w.open("/dev/ttyUSB0", 9600, 0, 8, 1))
	{
		for (int i = 0;i < 10;i++)
		{
			w.send("helloworld", 10);
		}
		cout << "send demo finished..."<<endl;
		w.close();
	}
	else
	{
		cout << "open serial port failed...";
	}
}

void receiveDemo()
{
	WzSerialPort w;
	if (w.open("/dev/ttyUSB0", 9600, 0, 8, 1))
	{
		char buf[1024];
		while (true)
		{
			memset(buf, 0,1024);
			w.receive(buf, 1024);
			cout << buf;
		}
	}
}

