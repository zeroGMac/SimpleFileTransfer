/**********************************************************
File name   : sft_command_handler.c
Description : function for handling and dispatching typed in 
              commands
@author     : Rakesh Balasubramanian
@created    : 9 Sep 2014
**********************************************************/

#include "global.h"
#include "sft_command_handler.h"
#include "sft_client_register.h"
#include "sft_command_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

/******* Global constants *********/
const char *gbl_commandArray[]= {"CREATOR\0","HELP\0",
                                 "MYIP\0","MYPORT\0","REGISTER\0","CONNECT\0","LIST\0",
                                 "TERMINATE\0","EXIT\0","UPLOAD\0","DOWNLOAD\0","STATISTICS\0","CLEAR\0"
                                };

char * listening_port;

/******* Macros *********/


/******* Function declarations *********/
bool checkIfCommand(char *commandString,int *commandIndex);
int commandDispatch(int argc, char **argv, int commandIndex,
                    RUNNING_MODE runningMode, client_list **theList);

void commandHelp(RUNNING_MODE runningMode, char *command);
void commandMyip();
void commandMyport();

/******* Function definitions *********/


/****************************************************************
Description  : Process input from STDIN and dispatch to corresponding
               methods
@arg         : argc token count
@arg         : argv token array
@arg         : runningMode client/server mode
@return      : status of processing command
*****************************************************************/
int processCommandArray(int argc, char **argv, RUNNING_MODE runningMode, client_list **theList)
{

    /******* Check if valid command and get command index *********/
    int commandIndex;
    if(!checkIfCommand(argv[0],&commandIndex))
    {
        fprintf(stderr, "Unknown command: %s", argv[0]);
        return 1;
    }


    /******* Dispatch commands to their functions *********/
    return commandDispatch(argc,argv,commandIndex,runningMode,theList);


}

/****************************************************************
Description  : Check if command present in recognized list
@arg         : commandString the command to be checked
@arg         : index out variable command index
@return      : boolean specifying if command is present
*****************************************************************/
bool checkIfCommand(char *commandString,int *index)
{


    /******* Length hard coded. Will have to change if no. of commands change *********/
    int i;
    for (i = 0; i < 13; ++i)
    {
        if (strcasecmp(commandString,gbl_commandArray[i])==0)
        {
            *index = i;
            return true;
        }
    }
    return false;
}

/****************************************************************
Description  : Process input from STDIN and dispatch to corresponding
               methods
@arg         : argc token count
@arg         : argv token array
@arg         : commandIndex index of command in the command list
@arg         : runningMode client/server mode
@arg         : theList address of the list pointer
@return      : status of processing command
*****************************************************************/
int commandDispatch(int argc, char **argv, int commandIndex,
                    RUNNING_MODE runningMode, client_list **theList)
{
    switch (commandIndex)
    {
        /******* Handle CREATOR *********/
        case 0:
        {
            if (argc>1)
            {
                fprintf(stderr, "Too many arguments");
                break;
            }
            printf("\nName	  : Rakesh Balasubramanian\nUBIT Name : rbalasub\n\
E-mail	  : rbalasub@buffalo.edu\n");
            printf("I have read and understood the course academic integrity \
policy located at\nhttp://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/\
index.html#integrity\n\n");
            break;
        }
        /******* dispatch HELP *********/
        case 1:
        {
            char *command = "FalseString";
            if (argc>2)
            {
                fprintf(stderr, "Too many arguments");
                break;
            }
            else if(argc==2)
            {
                command = argv[1];
            }
            commandHelp(runningMode,command);
            break;
        }
        /******* Dispatch MYIP  *********/
        case 2:
        {
            if (argc>1)
            {
                fprintf(stderr, "Too many arguments");
                break;
            }
            commandMyip();
            printf("IP address:%s", my_ip_addr );
            break;
        }
        /******* Handle MYPORT *********/
        case 3:
        {
            if (argc>1)
            {
                fprintf(stderr, "Too many arguments");
                break;
            }
            printf("Port number:%s", listening_port);
            break;
        }

        /******* Dispatch REGISTER *********/
        case 4:
        {
            if (runningMode == kSERVER_MODE)
            {
                fprintf(stderr, "Command available only in client\n");
                break;
            }
            else if(argc!=3)
            {
                fprintf(stderr, "Wrong usage\n Usage: REGISTER <SERVER> <PORT>");
                break;
            }

            command_connect(argv[1],argv[2],theList,REGISTER_FL);
            break;
        }

        case 5:
        {
            if (runningMode == kSERVER_MODE)
            {
                fprintf(stderr, "Command available only in client\n");
                break;
            }
            else if(argc!=3)
            {
                fprintf(stderr, "Wrong usage\n Usage: CONNECT <SERVER> <PORT>");
                break;
            }

            command_connect(argv[1],argv[2],theList,CONNECT_FL);
            break;
        }

        /******* Dispatch LIST *********/
        case 6:
        {
            if (argc>1)
            {
                fprintf(stderr, "Too many arguments");
                break;
            }
            commandList(*theList,runningMode);
            break;
        }
        /******* Dispatch TERMINATE *********/

        case 7:
        {
            if (argc!=2)
            {
                fprintf(stderr, "Wrong usage\n Usage: TERMINATE <CONNNECTION ID>\n");
                break;
            }
            commandTerminate(theList,runningMode,strtol(argv[1],NULL,10));
            break;
        }

        /******* Dispatch EXIT *********/
        case 8:
        {
            if (argc>1)
            {
                fprintf(stderr, "Too many arguments");
                break;
            }
            commandExit(theList);
            return -1;
        }
        /******* Dispatch Upload *********/
        case 9:
        {
            if (runningMode == kSERVER_MODE)
            {
                fprintf(stderr, "Command available only in client\n");
                break;
            }
            if (argc!=3)
            {
                fprintf(stderr, "Wrong usage\nUsage: UPLOAD <connection id> <file path>\n");
                break;
            }
            if ((argc%2)==0)
            {
                fprintf(stderr, "Invalid parameter '%s' dropped\n", argv[argc-1]);
                argc--;
            }

            /******* Dispatch params accordingly *********/
            int cid = strtol(argv[1],NULL,10);
            command_upload(*theList,cid, argv[2],kUP_FL);

            break;
        }
        /******* Dispatch DOWNLOAD *********/
        case 10:
        {
            if (runningMode == kSERVER_MODE)
            {
                fprintf(stderr, "Command available only in client\n");
                break;
            }
            if (argc>7)
            {
                fprintf(stderr, "Too many arguments\n");
                break;
            }
            if (argc<3)
            {
                fprintf(stderr, "Too few arguments\n");
                break;
            }
            if ((argc%2)==0)
            {
                fprintf(stderr, "Invalid parameter '%s' dropped\n", argv[argc-1]);
                argc--;
            }

            /******* Dispatch params accordingly *********/
            int ll;
            for (ll = 0; ll < argc/2; ll++)
            {
                int cid = strtol(argv[(ll*2)+1],NULL,10);
                command_download(*theList,cid, argv[(ll*2)+2]);
            }
            break;
        }

        /******* Handle STATISTICS *********/
        case 11:
        {
            if (argc!=1)
            {
                printf("Wrong usage.\n USAGE: STATISTICS\n");
                break;
            }

            command_statistics(*theList,runningMode);
            break;
        }

        /******* Handle CLEAR *********/
        case 12:
        {
            printf("\033[2J\033[1;1H");
            break;
        }
    }

    return 1;

}

/****************************************************************
Description  : Display help information
@arg         : runningMode client/server mode
@arg         : command command string 
@return      : void
*****************************************************************/
void commandHelp(RUNNING_MODE runningMode,char * command)
{



    /******* Array to print *********/

    char *clientPrintArray[13] =
    {
        "CREATOR - Prints Author information",
        "HELP - Prints help information",
        "MYIP - Display the IP address of this process",
        "MYPORT - Display the port on which this process \
is listening for incoming connections.",
        "REGISTER - Registers the client with the server.\
\n  Usage: REGISTER <SERVER IP> <PORT NO>",
        "CONNECT - Connects to a peer.\nUsage: CONNECT <DE\
STINATION> <PORT NO>",
        "LIST - Display all connections",
        "TERMINATE - Terminate a connection using the connection id\
\n  <Usage: TERMINATE <CONNECTION ID>",
        "EXIT - Close all connections and exit the process",
        "UPLOAD - Uploads a file to a peer.\n  Usage: UPLOAD \
<CONNECTION ID> <FILE NAME>",
        "DOWNLOAD - Downloads a file from a peer.\n  Usage: DOWNLOAD \
<CONNECTION ID1> <FILE NAME1> <CONNECTION ID2> <FILE NAME2> ..",
        "STATISTICS - Display statistics",
        "CLEAR - Clear the screen"
    };

    /******* Print single command help if opted *********/
    int commandIndex;
    if (checkIfCommand(command,&commandIndex))
    {
        printf("%s\n",clientPrintArray[commandIndex] );
        return;
    }


    int i;
    printf("\n");
    for (i = 0; i < 13; ++i)
    {
        /******* Skip command indexes if server*********/
        if(runningMode == kSERVER_MODE)
        {
            switch (i)
            {
            case 4:
            case 5:
            case 9:
            case 10:
            {
                continue;
            }
            default:
                sleep(0);
            }
        }
        printf ("%s\n",clientPrintArray[i]);
    }
}


/****************************************************************
Description  : Display own ip address
@return      : void
Ref: http://man7.org/linux/man-pages/man3/getifaddrs.3.html
*****************************************************************/
void commandMyip()
{


    /******* ifaddr Inner structure *********/
    // struct ifaddrs  *ifa_next;     Next item in list
    // char            *ifa_name;    /* Name of interface */
    // struct sockaddr *ifa_addr;    /* Address of interface */

    struct ifaddrs *ifaddr,*ifa;

    if (getifaddrs(&ifaddr)==-1)
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for(ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        int ip_family = ifa->ifa_addr->sa_family;
        void * addr;
        char * ipVersion;
        if ((ip_family == AF_INET) && !(strcmp(ifa->ifa_name,"lo")==0))
        {
            /******* Break after first IPv4 address encountered
            which is not the loop back address *********/
            struct sockaddr_in *ipv4 =  (struct sockaddr_in*)ifa->ifa_addr;
            addr = &(ipv4->sin_addr);
            char ipstr[INET6_ADDRSTRLEN];
            inet_ntop(ip_family,addr,ipstr,sizeof ipstr);
            my_ip_addr = strdup(ipstr);
            break;
        }
    }
    freeifaddrs(ifaddr);
}


