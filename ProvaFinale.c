#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HASHARRAYSIZE 50 //lunghezza HashArray per le entità
#define STRINGSIZE 50 //lunghezza Stringa
#define RELATIONSARRAYSIZE 50 //lunghezza id_rel_array (per i tipi di relazione)
#define EWNARRAYSIZE 50 //lunghezza di ogni EWNArray per indicare qual è l'entità con più relazioni di ogni tipo
#define NOTFOUND 999
int RelTypes=0;
typedef char String[STRINGSIZE];
String command;
String name1;
String name2;
String rel;
String end="end";
String s;

//----------------------------------ENTITY HASH ARRAY-------------------------------------------
int h1=0,h2=0;

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
    
 }
 //cancellazione enità da HashArray
 int delete_ent(entity *lista, String s){
    entity * punttemp;
    if(lista != NULL){
        if(strcmp(lista->name, s)==0){
            printf("(DELETE)elimino %s\n",s);
            punttemp=lista;
            lista=punttemp->next;
            free(punttemp);
        }
        else delete_ent((lista->next), s);   
        //implementare l'eliminazione delle relazioni che coinvolgono s
    }
 }

//-------------------------------------------------------------------------------------------------
//-----------------------------ID_REL_ARRAY--------------------------------------------------------
struct entityWithNumber{//array per il report 
    struct hash_entity *name;
    int n;
    //struct entityWithNumber *next; perche è un array
};
typedef struct entityWithNumber ewn;

struct id_relation{//id rel array
    String id_rel;
    struct rel_instance *instance_list;
    ewn EWNArray[EWNARRAYSIZE];
};
typedef struct id_relation relation;

relation id_rel_array[RELATIONSARRAYSIZE];//array per i vari tipi di relazione



struct rel_instance{//lista di istanze di relazione
    struct hash_entity *orig;
    struct hash_entity *dest;
    struct rel_instance *next;
};
typedef struct rel_instance instance;


//-------------------------------------------------------------ADDREL----------------------------------------------------------------
 entity *add_rel_search_ent(entity *lista, String s){//ricerca entità
    entity *cursore;
    if (lista!=NULL){
        cursore = lista;
        while(cursore!=NULL){
            if(strcmp(cursore->name, s)==0)
            {
                printf("(ADDRELSEARCHENT) trovata la stringa\n");
                
                return cursore;
            }
            cursore=cursore->next;
        }
    }
    printf("(ADDRELSEARCHENT) NON trovata la stringa\n");
    return NULL;
    
 }
 int search_position_id_rel_array(String rel){
    int i;
    for(i=0;i<RelTypes;i++){
        if(strcmp(id_rel_array[i].id_rel, rel)>0){
            return i;
        }
    }
 }
 int search_rel(String rel_name){//ricerca tipo di relazionee ritorna posizione in id_rel_array
    int i;
    
    for(i=0;i<RELATIONSARRAYSIZE;i++){
        
        if(strcmp(id_rel_array[i].id_rel, rel_name)==0){
           return i;
        }else if(strcmp(id_rel_array[i].id_rel, "")==0){
                return NOTFOUND;
        }
        
    }
 }
 void add_rel_type(String rel_name, int i){//crea lo spazio per l'aggiunta e aggiunge
    printf("(ADDRELTYPE)\n");
    int j;
    if(RelTypes==0){
        strcpy(id_rel_array[0].id_rel,rel_name);
        id_rel_array[0].instance_list=NULL;
        RelTypes++;
        return ;
    }else{
    for(j=RelTypes-1;j>=i;j--){
        strcpy(id_rel_array[j+1].id_rel,id_rel_array[j].id_rel);
        id_rel_array[j+1].instance_list=id_rel_array[j].instance_list;
        //id_rel_array[j+1].EWNArray=id_rel_array[j].EWNArray;//???
    }
    strcpy(id_rel_array[i].id_rel,rel_name);
    id_rel_array[i].instance_list=NULL;
    //ewn a;
    //id_rel_array[i].EWNArray=a;//???
    RelTypes++;
    }
    return;
 }
 int search_rel_instance(instance *lista, entity * op, entity *dp){//ricerca di determinata istanza di relazione nella lista di un tipo di relazione
    printf("(SEARCHRELINSTANCE)\n");
    instance *cursore;
    if(lista!=NULL){
        printf("(SEARCHRELINSTANCE)lista diversa da NULL!\n");
        cursore=lista;
        while(cursore!=NULL){
            printf("entrato\n");
            if(cursore->orig==op && cursore->dest==dp){
                 printf("c'è già!!!!\n");
                return 1;
            }
        cursore=cursore->next;
        }
    }
   
    return 0;
 }
 void add_rel_instance(instance *lista, entity * op, entity *dp,int p){//aggiunta di determinata istanza di relazione in testa alla lista di un tipo di relazione
    printf("(ADDRELINSTANCE)\n");
    instance *punt;
    punt = malloc(sizeof(instance));
    punt->orig=op;
    punt->dest=dp;
    punt->next=lista;
    id_rel_array[p].instance_list=punt;
    printf("(ADDRELINSTANCE) inserimento istanza di relazione \n");
    return;
 }


void add_rel(String orig_ent, String dest_ent, String r){//aggiunge la relazione e modifica i vari array, se la relazione esiste gia o se una delle due entità coinvolte non sono monitorate non fa nulla
    printf("(ADD_REL) start\n");
    h1=hash(orig_ent);
    h2=hash(dest_ent);
    int pos= search_rel(r);//posizione di id_rel_array di r
    printf("%d\n", pos);
    entity *orig_punt=add_rel_search_ent(HashArray[h1],  orig_ent);//puntatore ad origEntity
    entity *dest_punt=add_rel_search_ent(HashArray[h2],  dest_ent);//puntatore ad destEntity
    if(pos==NOTFOUND){//non esiste ancora quella relazione, la aggiungo e aggiungo anche l'istanza della relazione
        int p= search_position_id_rel_array(r);//posizione dove inserire il tipo di relazione
        add_rel_type(r,p);//aggiungo il tipo di relazione in ordine alfabetico
        add_rel_instance(id_rel_array[p].instance_list, orig_punt, dest_punt, p);//aggiungo l'istanza di relazione
        //gestione appropriata dell'array per il report
    }else if (search_rel_instance(id_rel_array[pos].instance_list, orig_punt, dest_punt)==0){//se non esiste quella relazione
            add_rel_instance(id_rel_array[pos].instance_list, orig_punt, dest_punt, pos);//la aggiunge in testa alla lista
            //gestione appropriata dell'array per il report
          }
    }

//------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------main------------------------------------------------------------------------
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
        strcpy(rel,"");
        fgets(s,50,stdin);
        //printf("%s",s);
        sscanf(s, "%s %s %s %s",command, name1, name2, rel);
        if(strcmp(command, "addent")==0){
            //funzioni per aggiungere entità
            h1=hash(name1);
            printf("MAIN ADD) la chiave di %s è %d\n",name1, h1);
            if(search_ent(HashArray[h1], name1)==0)//se l'entità non è monitorata
            {
                printf("(MAIN ADD)stringa non trovata, la aggiungo\n");
                insert_ent(name1, h1);//inserisce l'entità in testa alla lista
                
            }else{
                printf("(MAIN ADD) stringa trovata, non faccio nulla\n");
            }
            //printf("aggiungi entità\n");
            
        }else if(strcmp(command, "delent")==0){
            //funzioni per eliminare entità
            h1=hash(name1);
            printf("(MAIN DELETE) la chiave di %s è %d\n",name1, h1);
            if(search_ent(HashArray[h1], name1)==1){//se presente
                printf("(MAIN DELETE) TROVATO, la elimino\n");
                delete_ent(HashArray[h1], name1);//elimina entità (manca la gestione delle relazione che coinvolgono quell'entità)
            }else{//cerca l'entità da eliminare
                printf("(MAIN DELETE)stringa non trovata non faccio nulla\n");
            }
            
            //printf("elimina entità\n");
        }else if(strcmp(command, "addrel")==0){
            //funzioni per aggiungere relazione
            add_rel(name1, name2, rel);
            //printf("aggiungi relazione\n");
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