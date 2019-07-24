#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HASHARRAYSIZE 50

typedef char String[50];
String command;
String name1;
String name2;
String relation;
String end="end";
String s;

//----------------------------------ENTITY HASH ARRAY-------------------------------------------
int h=0;
//HashArray
struct hash_entity{
    String name;
    struct hash_entity *next;
};
typedef struct hash_entity entity;

entity *HashArray[HASHARRAYSIZE];//hash array per la gestione delle entità

//funzione di hashing
int hash(String s){
    int hash;
    for(hash=0;*s!='\0';s++){
        hash+=*s;
    }
    return hash%HASHARRAYSIZE;
}

//inserimento in HashArray (in testa)
void insert_ent(String s,int h){
    
    
        entity *punt;
        punt = malloc(sizeof(entity));
        strcpy(punt->name, s);
        punt->next=HashArray[h];
        HashArray[h]=punt;
        printf("(INSERT) inserimento %s\n",s);
    }   /*}
    else{
        entity *punt;
        punt = malloc(sizeof(entity));
        strcpy(punt->name,s);
        punt->next=HashArray[h];
        HashArray[h]=punt;
    }
    
}*/
//ricerca in HashArray
 int search_ent(entity *lista, String s){
    entity *cursore;
    if (lista!=NULL){
        cursore = lista;
        while(cursore!=NULL){
            if(strcmp(cursore->name, s)==0)
            {
                printf("(SEARCH) trovata la stringa\n");
                return 1;
            }
            cursore=cursore->next;
        }
    }
    printf("(SEARCH) NON trovata la stringa\n");
    return 0;
    
    //int h=hash(s);
    /*if(lista==NULL){
        printf("(SEARCH) la lista è vuota!\n");
        return 0;
    }else{
       if(strcmp(lista->name, s)==0){
            return 1;
       }else{
            return search_ent(lista->next, s);
       }
    } */
 }
 //cancellazione enità da HashArray
 int delete_ent(entity *lista, String s){
    entity * punttemp;
    if(lista != NULL){
        if(strcmp(lista->name, s)==0){
            printf("(DELETE)elimino %s\n",s);//sistemareeeeee
            punttemp=lista;
            lista=punttemp->next;
            free(punttemp);
        }
        else delete_ent((lista->next), s);   
    }
 }

//-------------------------------------------------------------------------------------------------


//main
int main(){
    /*int lung=0;
    for(int i=0, i<=HASHARRAYSIZE;i++){
        lung++;
    }
    printf("%d lunghezza di hasharray", lung);*/
    do{
        strcpy(command,"");
        strcpy(name1,"");
        strcpy(name2,"");
        strcpy(relation,"");
        fgets(s,50,stdin);
        //printf("%s",s);
        sscanf(s, "%s %s %s %s",command, name1, name2, relation);
        if(strcmp(command, "addent")==0){
            //funzioni per aggiungere entità
            h=hash(name1);
            printf("MAIN ADD) la chiave di %s è %d\n",name1, h);
            if(search_ent(HashArray[h], name1)==0)//se l'entità non è monitorata
            {
                printf("(MAIN ADD)stringa non trovata, la aggiungo\n");
                insert_ent(name1, h);//inserisce l'entità in testa alla lista
                
            }else{
                printf("(MAIN ADD) stringa trovata, non faccio nulla\n");
            }
            //printf("aggiungi entità\n");
            
        }else if(strcmp(command, "delent")==0){
            //funzioni per eliminare entità
            h=hash(name1);
            printf("(MAIN DELETE) la chiave di %s è %d\n",name1, h);
            if(search_ent(HashArray[h], name1)==1){//se presente
                printf("(MAIN DELETE) TROVATO, la elimino\n");
                delete_ent(HashArray[h], name1);//elimina entità (manca la gestione delle relazione che coinvolgono quell'entità)
            }else{//cerca l'entità da eliminare
                printf("(MAIN DELETE)stringa non trovata non faccio nulla\n");
            }
            
            //printf("elimina entità\n");
        }else if(strcmp(command, "addrel")==0){
            //funzioni per aggiungere relazione
            printf("aggiungi realzione\n");
        }else if(strcmp(command, "delrel")==0){
            //funzioni per aggeliminare relazione
            printf("elimina realzione");
        }else if(strcmp(command, "report\n")==0){
            //funzione per stampare output
            printf("stampa");
        }
    
    }while(strcmp(command,end)!=0);
    return 0;
}
//fine main