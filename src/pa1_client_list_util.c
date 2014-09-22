/*************************************************
Rakesh Balasubramanian

September 19th
*************************************************/

#include "pa1_client_list_util.h"

#include "global.h"
#include "pa1_listen_procs.h"
#include "pa1_ui.h"
#include "pa1_cmd_validate.h"
#include "pa1_network_util.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*************************************************
Psuedo-code/code used from
http://www.learn-c.org/en/Linked_lists - Begin
*************************************************/
void add_to_client_list(client_list **theList, int file_desc, char *host_name, char *ip_addr)
{

    /******* Create vars for temp access and new entry *********/
    client_list *current = *theList;
    client_list *new_entry;

    if (current == NULL) //Create new list if NULL
    {
        current = calloc (1,sizeof(client_list));
        current->connection_id = 1;
        current->file_desc = file_desc;
        current->host_name = strdup(host_name);
        current->ip_addr = strdup(ip_addr);
        current->cl_next = NULL;
        *theList = current;

    }
    else //Append to existing list
    {

        int current_cid = current->connection_id;
        new_entry = calloc(1, sizeof(client_list));
        new_entry->connection_id = current_cid+1;
        new_entry->file_desc = file_desc;
        new_entry->host_name = strdup(host_name);
        new_entry->ip_addr = strdup(ip_addr);
        new_entry->cl_next = *theList;
        *theList = new_entry;
    }

    /******* Dont print when adding to sip_list *********/
    if (!(*theList == sip_list))
    {
        printf("\n%s (%s) trying to connect...\n",host_name,ip_addr);
    }

    // printClientList(*theList); //Remove this line later
}



/******* Remove an entry from client list *********/
int remove_from_client_list(client_list **theList, int file_desc){
    if (*theList==NULL )
    {
        fprintf(stderr, "All connections closed \n");
        return;
    }
    int i = 0;
    client_list * current = *theList;
    client_list * temp_node = NULL;

    if (!(*theList == sip_list))
    {
        printf("\nClient  %s (%s) disconnected...\n",current->host_name,current->ip_addr);
    }

    if ((*theList)->file_desc == file_desc) {
        return pop(theList);
    }

    while (current->cl_next->file_desc != file_desc) {
        if (current->cl_next == NULL) {
            return -1;
        }
        current = current->cl_next;
    }
    close(file_desc);
    temp_node = current->cl_next;
    current->cl_next = temp_node->cl_next;
    free(temp_node);
}

int pop(client_list ** theList) {
    int retval = -1;
    client_list * next_node = NULL;

    if (*theList == NULL) {
        return -1;
    }

    next_node = (*theList)->cl_next;
    free(*theList);
    *theList = next_node;
    return retval;
}

/*************************************************
Code psuedo code used from
http://www.learn-c.org/en/Linked_lists - End
*************************************************/

void add_port_to_client(client_list *theList, int file_desc ,char *port_num)
{

    client_list *loopList = theList;
    while(loopList!=NULL){
        if (loopList->file_desc == file_desc)
        {
            // loopList->port = (char *)calloc(6, sizeof(char));
            loopList->port = strdup(port_num);
            if (!(loopList==sip_list))
            {
                printf("%s (%s) connected on %s...\n",                
                        loopList->host_name,
                        loopList->ip_addr,
                    port_num);
            }
            return;
        }
        loopList= loopList->cl_next;

    }

    fprintf(stderr, "Couldn't add port\n");

}


/******* Print a client list *********/
void printClientList(client_list *theList)
{
    client_list *print_list;
    print_list = theList;
    if (print_list == NULL)
    {
        printf("\nNo clients registered\n");
        return;
    }
    printf("\n");
    printf("                            Available peers                              \n");
    printf("-------------------------------------------------------------------------\n");
    if (theList==sip_list)
    {
        printf("%-35s%-20s%-8s\n","Host name","IP Address","Port");
    }else{
        printf("%-5s%-35s%-20s%-8s\n","CID","Host name","IP Address","Port");
    }
    printf("-------------------------------------------------------------------------\n");
    for (; print_list!= NULL; print_list = print_list->cl_next)
    {
        if (theList==sip_list)
        {
            printf("%-35s%-20s%-8s\n",
                print_list->host_name,
                print_list->ip_addr,
                print_list->port);
        }else{
            printf("%-5d%-35s%-20s%-8s\n",
                print_list->connection_id,
                print_list->host_name,
                print_list->ip_addr,
                print_list->port);
        }

    }
    printf("\n");
}

/******* Free a linked list *********/
void freeLinkedList(client_list **theList){
    client_list *tempNode;
    while(*theList){
        tempNode = *theList;
        *theList=(*theList)->cl_next;
        free(tempNode->ip_addr);
        free(tempNode->host_name);
        free(tempNode->port);
        free(tempNode);
    }
    (*theList) = NULL;
}

