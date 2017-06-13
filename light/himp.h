#ifndef __HIMP_H__
#define __HIMP_H__

typedef struct _him_data{
	unsigned int light_level; //to smart phone
	unsigned int light_set;   //from smart phone
	int cds;		  //to smart phone
	char ultra[32];		  //to smart phone
}__attribute__((packed))Him_data;


#endif //__HIMP_H_
