#include <stdio.h>
#include "interface.h"
// extern void receiveDemo();

int main(int argumentCount, const char* argumentValues[]) 
{
	// 假设 /dev/ttyS1 已经和另外一个串口连接好了

	// 发送 demo
	// sendDemo();

	// 接收 demo
	receiveDemo();

	return 0;
} 