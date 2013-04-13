// ARPThread.h

#ifndef _ARPTHREAD_H_
#define _ARPTHREAD_H_

#include <string>
#include <vector>
using namespace std;

typedef struct _st_ip_mac
{
	//char ip[16];
	string ip;
	//char mac[18];
	string mac;
}st_ip_mac;

int IPFind(string strIP, string strMask, vector<st_ip_mac> &vtIpMac);

#endif
