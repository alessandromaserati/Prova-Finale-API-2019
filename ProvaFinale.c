#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STRINGSIZE 150 //lunghezza Stringa
#define ENTITYHASHARRAYSIZE 50
#define RELATIONHASHARRAYSIZE 50
#define REPORTARRAYSIZE 50

typedef char String[STRINGSIZE];
String command;
String name1;
String name2;
String rel;
String end="end";
String s;

int RelTypes=0;

//--------------------------------------------------STRUTTURE DATI--------------------------------------------------------------------------------
//RelationHashArray
struct relation_instance{
    String id_rel;
    struct hash_entity* orig;
    struct hash_entity *dest;
    struct relation_instance* prev;
    struct relation_instance* next;
};
typedef struct relation_instance ri;

ri *RelationHashArray[RELATIONHASHARRAYSIZE];

//Lista di puntatori a relazione (ogni entità ne possiede una)
struct instance_pointer{
    struct relation_instance* rel;
    struct instance_pointer* next;
};
typedef struct instance_pointer ip;

//EntityHashArray
struct hash_entity{
    String name;
    struct instance_pointer *relations;
    struct hash_entity *next;
};
typedef struct hash_entity entity;

entity *EntityHashArray[ENTITYHASHARRAYSIZE];//hash array per la gestione delle entità

//ReportArray
struct entityWithNumber{//lista doppiamente concatenata per il report 
    struct hash_entity *ent;
    int n;
    struct entityWithNumber *next; 
    struct entityWithNumber *prev; 
};
typedef struct entityWithNumber ewn;

struct id_relation{
    String id_rel;
    struct entityWithNumber *report_list;   
};
typedef struct id_relation relation;

relation ReportArray[REPORTARRAYSIZE];//array per i vari tipi di relazione

//----------------------------------------------------------------------------------------------------------------------------------
//funzione di hashing per EntityHashArray
int ent_hash(String s){
    int hash;
    for(hash=0;*s!='\0';s++){
        hash+=*s;
    }
    //printf("(HASH) la key di %s è %d\n", s, hash%ENTITYHASHARRAYSIZE);
    return hash%ENTITYHASHARRAYSIZE;
}
//funzione di hashing per RelationHashArray
int rel_hash(String rel, int orig, int dest){
    int hash;
     for(hash=0;*rel!='\0';rel++){
        hash+=*rel;
    }
    hash=hash+orig+dest; 
    return hash%RELATIONHASHARRAYSIZE;
}

//-------------------------------------------ADDENT---------------------------------------------------------------------------------------
entity *search_ent(String n, int pos){
    if(EntityHashArray[pos]==NULL){
       // printf("(SEARCH_ENT)lista vuota, entità non trovata\n");
        return NULL;
    }else{
        entity* cursore = EntityHashArray[pos];
        //printf("(SEARCH_ENT) %s  %s, res=%d\n", n, cursore->name, strcmp(n, cursore->name));
        while(strcmp(n, cursore->name)!=0){
            cursore=cursore->next;
            if(cursore==NULL){//modificato per storia kelden e jothan
                //printf("(SEARCH_ENT)entità non trovata\n");
                return NULL;
            }
        };
        //printf("(SEARCH_ENT)entità trovata\n");
        return cursore;
    }
}

void insert_ent(String name, int pos){//inserimento in testa alla lista per le entità
    entity *punt;
    punt = malloc(sizeof(entity));
    strcpy(punt->name, name);
    punt->relations=NULL;
    punt->next=EntityHashArray[pos];
    EntityHashArray[pos]=punt;
    //printf("(INSERT_ENT) inserimento %s\n",name);
}


void addent(String name){
    //printf("(ADDENT)start\n");
    int h=ent_hash(name);
    //printf("(ADDENT in EntHashArray[%d]\n", h);
    if(search_ent(name, h)!=NULL){
        //printf("(ADDENT)questa entità è già monitorata\n");
    }else{
        //printf("(ADDENT)questa entità non è ancora monitorata, la aggiungo\n");
        insert_ent(name, h);
    }
}
//----------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------ADDREL--------------------------------------------------------------------------
ri *search_rel(String r, entity *o, entity *d, int pos){
    if(RelationHashArray[pos]==NULL){
        //printf("(SEARCH_REL)lista vuota, relazione non trovata\n");
        return NULL;
    }else{
        ri* cursore;
        cursore = RelationHashArray[pos];
        while(strcmp(r, cursore->id_rel)==1||cursore->orig!=o||cursore->dest!=d){
            cursore=cursore->next;
            if(cursore==NULL){
               // printf("(SEARCH_REL)relazione non trovata\n");
                return NULL;
            }
        };
        //printf("(SEARCH_REL)relazione trovata\n");
        return cursore;
    }
}

void insert_rel_pointer(entity *ent, ri* rel){
    ip *punt;
    punt = malloc(sizeof(ip));
    punt->rel=rel;
    punt->next=ent->relations;
    ent->relations=punt;
    //printf("(INSERT_REL_POINTER) inserimento %s %s --> %s\n",ent->relations->rel->id_rel, ent->relations->rel->orig->name, ent->relations->rel->dest->name); 
}

void insert_rel(String r, entity *o, entity *d, int pos){//inserisce in testa alla lista delle relazioni
    ri *punt;
    punt = malloc(sizeof(ri));
    strcpy(punt->id_rel, r);
    punt->orig=o;
    punt->dest=d;
    punt->next=RelationHashArray[pos];//ok
    punt->prev=NULL;//ok
    RelationHashArray[pos]=punt;//ok
    if(punt->next!=NULL){
        punt->next->prev=punt;
    }
    insert_rel_pointer(o, punt);
    insert_rel_pointer(d, punt);
    //printf("(INSERT_REL) inserimento %s %s -> %s\n",RelationHashArray[pos]->id_rel, RelationHashArray[pos]->orig->name, RelationHashArray[pos]->dest->name);
}

int search_position_report_array(String rel){//trova la posizione di ReportArray nella quale aggiornare la report_list
    int i;
    if(RelTypes==0){
        return 0;
    }
    for(i=0;i<RelTypes;i++){
        if(ReportArray[i].id_rel==""||strcmp(ReportArray[i].id_rel, rel)>=0){
            return i;
        }
    }
}

void change_position_up(ewn *punt, int pos){
   // printf("(CHANGEPOSITIONUP)start\n");
    ewn *cursore = punt;//stacco
    cursore->prev->next=cursore->next;
    if(cursore->next!=NULL){
        cursore->next->prev=cursore->prev;
    }
    while(punt->n > cursore->prev->n ||((punt->n == cursore->prev->n)&&(strcmp(punt->ent->name, cursore->prev->ent->name)<0))){
        
        //printf("(CHANGEPOSITIONUP)incremento cursore\n");
        cursore=cursore->prev;
        if(cursore->prev==NULL){
            //inserire in testa
            //printf("(CHANGEPOSITIONUP)inserisco in testa\n");
            cursore->prev=punt;
            punt->next=cursore;
            punt->prev=NULL;
            ReportArray[pos].report_list=punt;
            return;
        }
    };
    //cursore punta alla posizione successiva a quella dove viene inserito
    if(cursore==punt){//non ho mai incrementato, riattacco
    //printf("(CHANGEPOSITIONUP)riattacco nella stessa posizione\n");
        punt->prev->next=cursore;
        if(punt->next==NULL){
            return;
        }else{
            punt->next->prev=cursore;
        }
    }else{
       // printf("(CHANGEPOSITIONUP)attacco in mezzo\n");
        punt->prev=cursore->prev;
        punt->next=cursore;
        cursore->prev->next=punt;
        cursore->prev=punt;
    }
    return;
}

void modify_report_list(int position, entity *dest){
    //printf("(MODIFYREPORTLIST) start\n");
    ewn *cursore = ReportArray[position].report_list;
    while(1){
        if(cursore->ent==dest){//incremento il contatore e sistemo
            (*cursore).n++;
            if(cursore->prev==NULL){//incremento e non sistemo perche è già il primo
                //printf("(MODIFYREPORTLIST) è gia il primo incremento e basta\n");
                return;
            }else{
                change_position_up(cursore, position);
                return;
            }
        }else if(strcmp(dest->name, cursore->ent->name)<0&&cursore->n==1){//inserimento in mezzo con n=1
            //inserisco prima di cursore con n=1
            //printf("(MODIFYREPORTLIST) inserisco in pos corretta con n=1\n");
            ewn *punt=malloc(sizeof(ewn));
            punt->ent=dest;
            punt->n=1;
            punt->next=cursore;
            punt->prev=cursore->prev;
            if(cursore->prev!=NULL){//in testa con n=1
                cursore->prev->next=punt;
                cursore->prev=punt;
                return;
            }else{
                ReportArray[position].report_list=punt;
                cursore->prev=punt;
            }
            return;
        }else if(cursore->next==NULL){//inserimento in coda
           // printf("(MODIFYREPORTLIST) inserisco in coda\n");
            ewn *punt=malloc(sizeof(ewn));
            punt->ent=dest;
            punt->n=1;
            punt->next=NULL;
            punt->prev=cursore;
            cursore->next=punt;
            return;
        }
       // printf("(MODIFYREPORTLIST) incremento cursore\n");
        cursore=cursore->next;//incremento cursore
    };
    
}

void add_rel_type(String rel_name, int position, entity *dest){
    //printf("(ADDRELTYPE) start\n");
    if(ReportArray[position].id_rel==""){
        ewn *punt=malloc(sizeof(ewn));
        punt->ent=dest;
        punt->n=1;
        punt->next=NULL;
        punt->prev=NULL;
        ReportArray[position].report_list=punt;
        RelTypes++;
        return;
    }else{
        int j;
        for(j=RelTypes-1;j>=position;j--){
            
            strcpy(ReportArray[j+1].id_rel,ReportArray[j].id_rel);
            ReportArray[j+1].report_list=ReportArray[j].report_list;
        }
        
        strcpy(ReportArray[position].id_rel, rel_name);
        ewn *punt=malloc(sizeof(ewn));
        punt->ent=dest;
        punt->n=1;
        punt->next=NULL;
        punt->prev=NULL;
        ReportArray[position].report_list=punt;
        RelTypes++;
        //printf("(ADDRELTYPE) %s\n", ReportArray[position].id_rel);
        return;
    }
}
 
void add_to_report_list(String id_rel, entity *orig, entity* dest){
   // printf("(ADDTOREPORTLIST) start\n");
    int pos = search_position_report_array(id_rel);
    //printf("(ADDTOREPORTLIST) %d %s\n", pos,ReportArray[pos].id_rel);
    if(strcmp(id_rel, ReportArray[pos].id_rel)==0){
        //printf("(ADDTOREPORTLIST) tipo di relazione trovato, modifico la report list\n");
        modify_report_list(pos, dest);
    }else{
        //printf("(ADDTOREPORTLIST) tipo di relazione non trovato, aggiungo il tipo di relazione\n");
        add_rel_type(id_rel,pos, dest);    
    }
}

void addrel(String rel, String orig, String dest){
   // printf("(ADDREL)start\n");
    int h_o, h_d;
    h_o=ent_hash(orig);
    h_d=ent_hash(dest);
    entity *o=search_ent(orig, h_o);
    entity *d=search_ent(dest, h_d);
    if(o==NULL||d==NULL){
        //printf("(ADDREL)una delle due entità non è monitorata!\n");
        return;
    }else{
        int h = rel_hash(rel, h_o, h_d);
        //printf("(ADDREL)pos = %d\n", h);
        ri *r = search_rel(rel, o, d, h);
        if(r!=NULL){
            //printf("(ADDREL)istanza di relazione trovata, non faccio nulla!\n");
            return;
        }else{
            insert_rel(rel, o, d, h);
            add_to_report_list(rel, o, d);
            return;
        }
    }

}

//----------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------DELREL-------------------------------------------------------------------------
void delete_rel(ri* punt, int pos){
    //printf("(DELETEREL)start\n");
    if(punt->prev==NULL&&punt->next==NULL){//caso unico in lista
        //printf("(DELETEREL)caso unico lista\n");
        RelationHashArray[pos]=NULL;
        return;
    }else if(punt->prev!=NULL&&punt->next==NULL){//caso coda
        //printf("(DELETEREL)caso coda lista\n");
        punt->prev->next=punt->next;
        punt->prev=NULL;
        return;
    }else if(punt->prev==NULL&&punt->next!=NULL){//caso testa
        //printf("(DELETEREL)caso testa lista\n");
        punt->next->prev=NULL;
        RelationHashArray[pos]=punt->next;
        punt->next=NULL;
        return;
    }else if(punt->prev!=NULL&&punt->next!=NULL){//caso centrale
        //printf("(DELETEREL)caso centrale\n");
        punt->prev->next=punt->next;
        punt->next->prev=punt->prev;
        punt->prev=NULL;
        punt->next=NULL;
        return;
    }
}

void change_position_down(ewn *punt, int pos){//considera solo casi con n>=1
    //printf("(CHANGEPOSDOWN)start\n");
    ewn *cursore=punt;
    //stacco
    if(cursore->next==NULL){//un solo ewn oppure in fondo alla lista
        return;    
    }
    if(cursore->prev==NULL){//in testa
        ReportArray[pos].report_list=cursore->next;
        cursore->next->prev=NULL;
    }else{//caso generale
        cursore->prev->next=cursore->next;
        cursore->next->prev=cursore->prev;
    }
    //fine stacco
    while(punt->n < cursore->next->n ||((punt->n == cursore->next->n)&&(strcmp(punt->ent->name, cursore->next->ent->name)>0))){
        //printf("(CHANGEPOSDOWN)incremento cursore\n");
        cursore=cursore->next;
        if(cursore->next==NULL){//inserimento in fondo
            //printf("(CHANGEPOSDOWN)inserisco in fondo\n");
            cursore->next=punt;
            punt->next=NULL;
            punt->prev=cursore;
            return;
        }
    };
    //cursore punta alla posizione precedente a quella dove viene inserito
    if(cursore==punt){//non ho mai incrementato, riattacco
        //printf("(CHANGEPOSITIONDOWN)riattacco nella stessa posizione\n");
        cursore->next->prev=punt;
        if(cursore->prev==NULL){
            ReportArray[pos].report_list=punt;
        }else{
            cursore->prev->next=punt;
        }
    }else{
        //printf("(CHANGEPOSITIONDOWN)attacco in mezzo\n"); 
        cursore->next->prev=punt;
        punt->next=cursore->next;
        punt->prev=cursore;
        cursore->next=punt;
    }
    return;
}

void del_rel_type(int pos){//cancella tipo di relazione in seguito all'eliminazione dell'ultimo ewn presente
    //printf("(DELRELTYPE)start\n");
    int i;
    for(i=pos+1;i<=RelTypes;i++){
        strcpy(ReportArray[i-1].id_rel, ReportArray[i].id_rel);
        ReportArray[i-1].report_list = ReportArray[i].report_list;
    }
    RelTypes--;
}

void modify_report_list2(int position, entity *dest){//modifica la report list, chiama change_position_down
    //printf("(MODIFYREPORTLIST2) start\n");
    ewn *cursore = ReportArray[position].report_list;
    while(1){//teoricamente dovrebbe trovarlo sempre
        
        if(cursore->ent==dest){
            //printf("(MODIFYREPORTLIST2) trovato\n");
            (*cursore).n--;
            if(cursore->n==0){
                //posso toglierlo direttamente
                //printf("(MODIFYREPORTLIST2) quest'entità ha 0 relazioni entranti, la tolgo dalla reportlist\n");
                if(cursore->next==NULL&&cursore->prev==NULL){//caso unicoin lista
                    //printf("(MODIFYREPORTLIST2) caso unico in lista\n");
                    ReportArray[position].report_list=NULL;
                    //printf("chiamo delreltype\n");
                    del_rel_type(position);
                    return;
                }else if(cursore->next==NULL){//caso ultimo lista
                    //printf("(MODIFYREPORTLIST2) caso ultimo lista\n");
                    cursore->prev->next=NULL;
                    cursore->prev=NULL;
                    return;
                }else if(cursore->prev==NULL){//caso primo lista
                    //printf("(MODIFYREPORTLIST2) caso primo lista\n");
                    cursore->next->prev=NULL;
                    ReportArray[position].report_list=cursore->next;
                    cursore->next=NULL;
                    return;
                }else{//caso centrale
                    //printf("(MODIFYREPORTLIST2) caso centrale\n");
                    cursore->prev->next=cursore->next;
                    cursore->next->prev=cursore->prev;
                    cursore->prev=NULL;
                    cursore->next=NULL;
                }
                return;
            }else{
                //printf("(MODIFYREPORTLIST2) chiamo change pos down\n");
                change_position_down(cursore, position);
                return;
            }
            
        }
        cursore=cursore->next;
    };
}



void delete_from_report_list(String id_rel, entity *orig, entity* dest){
    //printf("(DELETEFROMREPORTLIST) start\n");
    int pos = search_position_report_array(id_rel);
    modify_report_list2(pos, dest);
    return;
}

void delrel(String rel, String orig, String dest){
    //printf("(DELREL)start\n");
    int h_o, h_d;
    h_o=ent_hash(orig);
    h_d=ent_hash(dest);
    entity *o=search_ent(orig, h_o);
    entity *d=search_ent(dest, h_d);
    if(o==NULL||d==NULL){
        //printf("(DELREL)una delle due entità non è monitorata!\n");
        return;
    }else{
        int h = rel_hash(rel, h_o, h_d);
        //printf("(ADDREL)pos = %d\n", h);
        ri *r = search_rel(rel, o, d, h);
        if(r==NULL){
           //printf("(DELREL)relazione non monitorata!\n"); 
           return;
        }else{
            delete_rel(r, h);
            delete_from_report_list(rel, o, d);
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------DELENT-------------------------------------------------------------------------
short caso=0;
entity *search_ent_del(String n, int pos){
    //printf("(SEARCH_ENT_DEL)start\n");
    if(EntityHashArray[pos]==NULL){
        //printf("(SEARCH_ENT_DEL)lista vuota, entità non trovata\n");
        return NULL;
    }else{
        entity* cursore = EntityHashArray[pos];
        if(strcmp(n, cursore->name)==0){//caso in cui l'entità è la prima della lista
            //printf("(SEARCH_ENT_DEL)entità da eliminare è la prima\n");
            caso=1;
            return cursore;//controllare questo caso di la
        }
        //printf("(SEARCH_ENT_DEL) %s  %s, res=%d\n", n, cursore->name, strcmp(n, cursore->name));
        if(cursore->next==NULL){
            //printf("(SEARCH_ENT_DEL)entità non trovata\n");
            return NULL;
        }
        while(strcmp(n, cursore->next->name)!=0){
            cursore=cursore->next;
            if(cursore->next==NULL){//modificato per storia kelden e jothan
                //printf("(SEARCH_ENT_DEL)entità non trovata\n");
                return NULL;
            }
        };
        //printf("(SEARCH_ENT_DEL)entità trovata\n");
        return cursore;
    }
}

void delete_ent(entity *punt, int pos){
    //printf("(DELETEENT)start\n");
    
    if(caso == 1){
        //printf("(DELETEENT)entità da eliminare è la prima\n"); 
        ip *cursore=punt->relations;
        while(cursore!=NULL){
            //printf("(DELETEENT)istanza di relazione %s -> %s\n", cursore->rel->orig->name, cursore->rel->dest->name );
            int ho=ent_hash(cursore->rel->orig->name);
            int hd=ent_hash(cursore->rel->dest->name);
            int h=rel_hash(cursore->rel->id_rel, ho, hd);
            if(cursore->rel->prev==NULL&&cursore->rel->next==NULL&&RelationHashArray[h]!=cursore->rel){//mah vediamo
                //printf("(DELETEENT)istanza di relazione non più esistente\n" );
            }else{
                int ho=ent_hash(cursore->rel->orig->name);
                int hd=ent_hash(cursore->rel->dest->name);
                int h=rel_hash(cursore->rel->id_rel, ho, hd);
                delete_rel(cursore->rel, h);
                delete_from_report_list(cursore->rel->id_rel, cursore->rel->orig, cursore->rel->dest);
            }
            cursore=cursore->next;
        };
       
       EntityHashArray[pos]=punt->next;//elimino
       caso=0;
    }else{
        //printf("(DELETEENT)entità da eliminare NON è la prima\n");
        ip *cursore=punt->next->relations;
        while(cursore!=NULL){
            //printf("(DELETEENT)istanza di relazione %s -> %s\n", cursore->rel->orig->name, cursore->rel->dest->name );
           int ho=ent_hash(cursore->rel->orig->name);
           int hd=ent_hash(cursore->rel->dest->name);
           int h=rel_hash(cursore->rel->id_rel, ho, hd);
           if(cursore->rel->prev==NULL&&cursore->rel->next==NULL&&RelationHashArray[h]!=cursore->rel){
                //printf("(DELETEENT)istanza di relazione non più esistente\n");
            }else{
                delete_rel(cursore->rel, h);
                delete_from_report_list(cursore->rel->id_rel, cursore->rel->orig, cursore->rel->dest);
            }
            cursore=cursore->next;
        };
        
        
        punt->next=punt->next->next;//elimino
    }
    
}

void delent(String name){
    //printf("(DELENT)start\n");
    int h=ent_hash(name);
    entity *ent=search_ent_del(name, h);
    if(ent==NULL){
        //printf("(DELENT)questa entità non è monitorata\n");
        return;
    }else{
        //printf("(DELENT)questa entità è monitorata, la elimino\n");
        delete_ent(ent, h);
    }
}


//----------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------REPORT-------------------------------------------------------------------------
void report(){
    int i,number,none=0;
    ewn *cursore;
    if(ReportArray[0].report_list==NULL){
        printf("none\n");
        return;
    }
    for(i=0 ;i<RelTypes;i++){
        
        if(i!=0){
            printf(" ");
        }
        
        number=ReportArray[i].report_list->n;
        cursore=ReportArray[i].report_list;
        printf("%s ",ReportArray[i].id_rel);
        while(cursore->n==number){
            printf("%s ",cursore->ent->name);
            if(cursore->next==NULL){ 
                break;
            }
            cursore=cursore->next;
        }
        printf("%d;",number);
        
        
        if(i==RelTypes-1){
            printf("\n");
        }
    }
    
}

//----------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------main------------------------------------------------------------------------
int main(){
    do{
        strcpy(command,"");
        strcpy(name1,"");
        strcpy(name2,"");
        strcpy(rel,"");
        fgets(s,150,stdin);
        //printf("%s",s);
        sscanf(s, "%s %s %s %s",command, name1, name2, rel);
        if(strcmp(command, "addent")==0){
            addent(name1);
            //printf("(MAIN)aggiungi entità\n");        
        }else if(strcmp(command, "delent")==0){
            delent(name1);
            //printf("elimina entità\n");
        }else if(strcmp(command, "addrel")==0){
            addrel(rel, name1, name2);
            //printf("aggiungi relazione\n");
        }else if(strcmp(command, "delrel")==0){
            delrel(rel, name1, name2);
            //printf("elimina relazione\n");
        }else if(strcmp(command, "report")==0){
            report();
            
        }
        //print_relations();
        //print_report_list();
    
    }while(strcmp(command,end)!=0);
    return 0;
}
//fine main