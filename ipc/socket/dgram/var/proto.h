#ifndef _PROTO_H__
#define _PROTO_H__


#define RCVPORT  "1989"

#define NAMESIZE (512-8-8) //8结构体前几位，8UDP包头

struct msg_st
{
	uint32_t math;
	uint32_t chinese;
	uint8_t name[1];
}__attribute__((packed));


#endif



