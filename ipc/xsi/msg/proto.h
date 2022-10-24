#ifndef _PROTO_H__
#define _PROTO_H__

#define KEYPATH "/etc/services"
#define KEYPROJ 'g'

#define NAMESIZE 32

struct msg_st
{
	long mtype; //msg type,  手册中要求结构体里必应有mtype, (man msgop)  mtype > 10;
	char name[NAMESIZE];
	int math;
	int chinese;
};


#endif

