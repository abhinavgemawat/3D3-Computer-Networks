//
//  parser
//
//
//  Created by Abhinav on 05/04/19.
//  Copyright © 2019 Abhinav. All rights reserved.
//
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <errno.h>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include <vector>

#define ROUTERS 6       //total routers

using namespace std;

/*
 Print layout-
 "destination:<destination>,source:<source>,function:1/2/3,type:1/0,message:<msg>"
 'Functions'
 1 - Initial set-up (for exchanging messages between routers)
 2 - Updating routing table (not done)
 3 - Normal messenger
 'Type' here refers to a reply type or a request type
  */

int indexPrint;      //for what menu to show in the output

struct DV {
    char node;
    int shortest_route;
    char next_router;
}dvinfo[ROUTERS];

struct routerData {     //Current router's data
    char source;
    int port;
    int num_neighbors;
}r1;

struct neighboringRouter
{     //for storing neighbor router info
    char source;
    int port;
    int cost;
}neighborRouter[ROUTERS];

struct dest_router
{      //using for storing parameters of destination
    int port;
    char destination;
};

struct node {
    char end;
    char start;
    int cost;
};


//messagesender Output Layout - "destination:<destination>,source:<source>,function:<FUNC>, type:1/0,port:<PORT>,message:<MSG>"

void messagesender(struct dest_router *dest_R, int fT, int type = 1, string msg = "", struct node *ed = NULL, int save = 0, char finalRouter = -1)

{
    int message_length = sizeof(struct sockaddr_in);
    int Bytes = 0;
    
    string MessageSent;
    char destination = dest_R->destination;
    string destination_s(1,destination);
    if(fT == 1){ //fT - function type
        if(type != 0)
            MessageSent = "final router:"+destination_s+",initial router:"+r1.source+",function:1,type:1,port:"+to_string(r1.port)+",message:"+
            msg+","+destination_s; //"to_string" converts a signed decimal integer to a string
        else{
            MessageSent = "final router:"+destination_s+",initial router:"+r1.source+",function:1,type:0,port:"+to_string(r1.port)+",message:"+
            msg+","+destination_s;
            
        }
    }
    else if(fT == 2)
    {
        string sources_string(1, ed->end);
        string destinations_string(1, ed->start);
        MessageSent = "sources-" + sources_string + ",destinations-" + destinations_string + ",W-" + to_string(ed->cost);
        MessageSent = "destination:"+destination_s+",source:"+r1.source+",function:2,type:1,port:"+to_string(r1.port)+",message:"+msg;
    }
    else if(fT == 3)
    {
        if(save == 0)
            MessageSent = "destination:"+destination_s+",source:"+r1.source+",function:3,type:1,port:"+to_string(r1.port)+",message:"+msg+","+destination_s;
        else
        {
            cout<<"\nSTORE OR SEND "<<save<<" destination: "<<finalRouter<<"\n";
            MessageSent = "destination:"+destination_s+",source:"+r1.source+",function:3,type:1,port:"+to_string(r1.port)+ ",message:"+msg+","+finalRouter;
        }
    }
    
    else
        cout<<"\n\t\tError found in function type"<<endl;
    
    char buffer[(MessageSent.length())+1];
    strcpy(buffer, MessageSent.c_str());
    int sock1 = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock1<0)
        cout<<"\n\t\tSocket could not be created\n\n";
    
    sockaddr_in sender_address = {};
    sender_address.sin_family = AF_INET;
    sender_address.sin_port = htons(dest_R->port);
    sender_address.sin_addr.s_addr = inet_addr("127.0.0.1");        //Default for now.
    
    bind(sock1, (struct sockaddr *) &sender_address, sizeof(sender_address));     //Not checking failure for now..
    if((Bytes=(sendto(sock1, buffer, strlen(buffer), 0, (struct sockaddr *) &sender_address, message_length))) == -1) {
        cout<<"\n\t\tMessage could not be send to the receiver"<<endl;
        return;
    }
    cout<<"\n\t\tMessage sent "<<Bytes<<" to "<<dest_R->destination<<" on port "
    << dest_R->port <<" using port: "<<sock1<<"\n"<<"\t\tmessage: "<< buffer;
    close(sock1);
}

struct discoveredRouters
{
    char source;
    int port;
};

void Index() {
    if (indexPrint == 1)
        cout<<"\n\nThe final router is (enter " << r1.source << " for router functions): ";
    
    
    else if (indexPrint == 2)
    {
        cout<<"\nTo initialize routers - 1, Shutting down routers - 2, For viewing Routing Table - 3\t Enter choice: ";
        
    }
    else if (indexPrint == 3)
    {
        cout<<"\nType in the message for the destination router: ";
        
    }
}vector<struct discoveredRouters*> dRouters;


//Parser Header "destination:<destination>,source:<source>,function:<func>,type:1/0,port:<port>,message:<msg>,<ActualDestination>"
//"destination:<destination>,source:<source>,function:1/2/3,type:1/0,message:<msg>"
// ActualDestination - required only if the packet is to be sent to another router)

void Parser(char *z) {
    cout<<"Message received from Router: \n\t"<<z;
    
    // "A,B,10001,3"
    char parentsource = z[21];
    char destination = z[12];
    int type = z[38] - '0';
    int fT = z[32] - '0';
    char portno[5]; // 4
    // cost , 6

    
    for(int i = 0; i<5;i++)
        portno[i] = z[45+i];
    
    int port_ascii = atoi(portno);
    
    struct dest_router finalpoint;
    string message;
    
    int ind_num = 0;
    char act_dest;
    if(type == 1) {
        for(int i = 0; i<6;i++) { //The actual destination of the packet is after the 6th comma
            ind_num++;
            while( z[ind_num] != ','  )
                ind_num++;
        }
    }
    else{
        for(int i = 0; i<8;i++) {         //The actual destination of the packet is after the 8th comma
            ind_num++;
            while(z[ind_num]!=',')
                ind_num++;
        }
    }
    ind_num++;     //The buffer seeker refers to the <Actualdestination> part of the packet
    act_dest = z[ind_num];
    
    
    if(act_dest == r1.source) {
        //To check if packet is meant to be passed on or is it for our router

            if(fT == 1)
            {
                //Send back info of all the neighbors to the source with type - 0 (reply/ack)
                if(type == 1) {
                    for(int i = 0; i<r1.num_neighbors; i++) {
                        char source = neighborRouter[i].source;
                        string source_s(1, source);
                        if(neighborRouter[i].source != -1){
                            message = "N" + to_string(i+1) + "-" + source_s + ",W" + to_string(i+1) + "-"
                    + to_string(neighborRouter[i].cost)+",P-"+to_string(neighborRouter[i].port);
                            finalpoint.destination = parentsource;
                            finalpoint.port = port_ascii;
                            messagesender(&finalpoint, fT, 0, message);
                            //Replying with my router's neighbor info
                        }
                    }
                }else if(type == 0){
                    int w,ind_num = -1;
                    int reg = 0;
                    int port_y;
                    char y;
                    for(int i = 0; i<6;i++) {
                    //Data after 6th : contains the message containing neighbor info
                        ind_num++;
                        while(z[ind_num]!=':')
                            ind_num++;
                    }
                    ind_num+=4;
                    y = z[ind_num];
                    ind_num+=5;
                    w = z[ind_num] - '0';
                    ind_num+=4;
                    char yport[5];
                    for(int i = 0; i<5;i++)
                        yport[i] = z[ind_num+i];
                    port_y = atoi(yport);
                    
                    //Has the vertex already been discovered.
                    //If not, add it to the list of DISCOVERED ROUTERS
                    
                    if (y == r1.source)
                        reg = 1;
                    
                    if(reg == 0) {
                        for(int i = 0; i<r1.num_neighbors ;i++) {
                            if(y == neighborRouter[i].source)
                                reg = 1;
                        }
                    }
                    if(reg == 0){
                        for(int i = 0; i<dRouters.size();i++)
                            if(dRouters[i]->source == y)
                                reg = 1;
                    }
                    //If reg = 0, it means the node sending this DV is new
                    
                    if(reg == 0) {
                        discoveredRouters *node = (struct discoveredRouters*) malloc(sizeof(struct discoveredRouters));
                        node->source = y;
                        node->port = port_ascii;
                        dRouters.push_back(node);
                        cout<<"New node discovered: "<<node->source<<"\t";
                        finalpoint.destination = y;
                        finalpoint.port = port_y;
                        messagesender(&finalpoint, fT, 1);
                        //for newly discovered router's neighbours
                    }
                    
                }
            }
                
                
            else if (fT == 2)
            {
                int ind_num;
                for(int i = 0; i<6; i++) {
                    ind_num++;
                    while(z[ind_num] != ',')
                        ind_num++;
                }
                ind_num--;
               
                cout<<"\n\t\tMessage from " << z[ind_num] << ": \n\t\t"<<z<<"\n\n";
               ;
                
            }
        
    }
    else {        //Passing the packet
        int reg = 0;
        int a1, a2;
        a1 = 0;
        for(int j = 0; j<6; j++) {
            a1++;
            while(z[a1] != ':')
                a1++;
        }
        a1++;
        a2 = ind_num-1;
        
        string tempMessage(z+a1, z+a2);
        for(int i = 0; i<r1.num_neighbors;i++) {
            if(neighborRouter[i].source == act_dest) {
                finalpoint.destination = act_dest;
                finalpoint.port = neighborRouter[i].port;
                reg = 1;
                messagesender(&finalpoint, 3, 1, tempMessage);
                cout<<"\nMessage sent to neighbor "<<act_dest<<"\n";
                break;
            }
        }
        
        char prevRouter = '\0';
        if(reg == 1)
            goto skipParser;
        
        if(reg == 0) {  //Router is not a neighbor, it can already be discovered
            for(int i = 0; i<ROUTERS; i++) {
                if(dvinfo[i].node == act_dest) {
                    prevRouter = dvinfo[i].next_router;
                    reg = 1;
                }
            }
        }
        
        
        if(reg == 1) {
        //Router present in discoverd routers, find the neighbor to which we need to transmit packet
            for(int i = 0; i<r1.num_neighbors; i++) {
                if(prevRouter == neighborRouter[i].source) {
                    reg = 0;       //NEIGHBOR FOUND!
                    finalpoint.destination = prevRouter;
                    finalpoint.port = neighborRouter[i].port;
                    messagesender(&finalpoint, 3, 1, tempMessage, NULL, 1, act_dest);
                    goto skipParser;
                }
            }
            while(reg){
                prevRouter = dvinfo[prevRouter%65].next_router;
                for(int i = 0; i<r1.num_neighbors; i++) {
                    if(prevRouter == neighborRouter[i].source) {
                        finalpoint.destination = prevRouter;
                        finalpoint.port = neighborRouter[i].port;
                        messagesender(&finalpoint, 3, 1, tempMessage, NULL, 1, act_dest);
                        reg = 0;       //Router is a neighbor
                        goto skipParser;
                    }
                }
            }
            cout<<"\nPacket forwarded to "<<prevRouter << " for Router " << act_dest<<"\n";
        }
        else cout<<"\nRouter not in the network\n";
    }
    
skipParser: Index();
}


