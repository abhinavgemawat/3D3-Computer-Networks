#include "DistanceVector->h"
#include <iostream>

const int N = 6;
struct changed_cost{
    char destinations[N];

    int size = 0;

};

void pingnodes(DV *forwordtable){

    for (int i = 0; i < N ;i++){
        if(i == (forwordtable->next[i]-65) && forwordtable->cost[i]!= 0 ){

            // insert messagesender when made to send ping control message
        }

    }

};


void node_dead(DV *forwordtable, int portnum, changed_cost *list){
    
    int i = 0;

        for ( int j =0 ; j<N ;j++){
            if (forwordtable->next_port[j] == portnum){
                if(j == (forwordtable->next[j]-65)){
                forwordtable->cost[j] = -1 ;
                list->destinations[i]=forwordtable->next[j];
                i++;}
                }else{
                forwordtable->cost[j] = infinity;
                list->destinations[i]=forwordtable->next[j];
                i++;
                }

         }
    list->size = i;
};






bool comp_change(DV *forwordtable, char node, int newcost,int portnum){
   int i = 0;
   char temp;

    if(newcost == -1){
     forwordtable->cost[node-65] = newcost ;
     return true;
    }else if(newcost == infinity && forwordtable->next_port[node-65] == portnum){
        forwordtable->cost[node-65] = newcost ;
        return true;
    }else{
     /* for (int j= 0;j<N;j++){
            if (forwordtable->next[j]==forwordtable->destinations[j] && forwordtable->next_port[j]==portnum){
                temp=forwordtable->next[j];
            }
        }
        if(forwordtable->cost[node-65]>forwordtable->neighbourCost[node-65]+newcost){
        forwordtable->algo(node,temp,newcost, portnum);*/
        return false;
    }
};
