#include "500client.h"
#include "500global.h"
#include <iostream>
#include <cstdio>
using namespace std;

int main() {
	char ip[64];
	char name[64];
	char NAME[64];
	cout << "HELLO WHAT IS YOUR NAME?\n";
	cin >> name;
	for(int i=0;i<strlen(name);i++) NAME[i]=name[i]+(name[i]>='a' && name[i]<='z')*('A'-'a');
	NAME[strlen(name)]='\0';
	cout << "HI THERE " << NAME << ", TO WHICH IP ADDRESS WOULD YOU LIKE TO CONNECT?\n";
	cin >> ip;
	c500Client client;
	if(client.init(ip,"1500")) {
		client.start();
		client.setName(name);
		client.setStdin(true);
		while(client.getConnected()) client.update();
		client.shutdown();
	}
	return 0;
}
