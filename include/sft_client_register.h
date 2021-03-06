/**********************************************************
File name   : sft_client_register.h
Description : functions  and enums involving REGISTER command
@author     : Rakesh Balasubramanian
@created    : 14th Sep 2014
**********************************************************/

#ifndef SFT_CLIENT_REGISTER_H_
#define SFT_CLIENT_REGISTER_H_


typedef enum {
	REGISTER_FL,
	CONNECT_FL
} connect_flag;

void command_connect(char *destingation, char *port, client_list **theList, connect_flag fl);
void parseAndPrintSIPList(char *SIPlist);



#endif
