#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h> //for threading , link with lpthread

 struct argThread{
   int *r;      	//puntatore a variabile che decreta l'inizio della partita(ready);
   int id;      	//id giocatore(verrà usato per capire chi è G1(che ha id==0) e G1(che ha id==1)
   int *s;      	//puntatore a new_socket
   int *t;       	//puntatore alla variabile di turno
   int *v;      	//puntatore alla variabile vittoria
   char *ownev;		//puntatore al proprio evento
   char *enev;		//puntatore all'evento del nemico
   char *ownmat;	//puntatore alla propria matrice(sempre relativa al giocatore)
   char *enmat;		//puntatore alla matrice nemica(sempre relativa al giocatore)
   int *e;			//puntatore a variabile end
     
};
typedef struct argThread argT;
 

void *g(void *);
char ctrlEventi(char c[], char mRead[]);
 

int main(int argc , char *argv[])
{
	char matg1[100], matg2[100];	//campo del g1, campo del g2
	int ready=0;
	int turno=2;             //variabile che decreta di chi è il turno(se turno è 0 il turno e di g1, se è 1 il turno è di g2)
	int vittoria=0;          //variabile che decreta di chi è la vittoria, se è a 0 non ha ancora vinto nessuno, se è ad 1 ha vinto G1, se è a 2 ha vinto G2
    int end=0;				//variabile che quando è a 2 il server termina
    char e1[1];				 //codice evento del g1
    char e2[1];				 //codice evento del g2
    e1[1]='\0';
    e2[1]='\0';
    e1[0]=' ';				//all'inizio i codici evento sono indefiniti
    e2[0]=' ';
    
    
    int socket_desc , new_socket , c;
    struct sockaddr_in server , client;
    
   
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("\nImpossibile creare il socket");
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("IMPOSSIBILE AVVIARE SERVER BATTAGLIA NAVALE");
        return 1;
    }
    puts("SERVER BATTAGLIA NAVALE AVVIATO");
     
    //Listen
    listen(socket_desc , 2);
     
    //Accept and incoming connection
    puts("In attesa per 2 giocatori...");
    c = sizeof(struct sockaddr_in);
    
    int nt=0;
    pthread_t tID[2];
    argT argG[2];
    
    
    do
    {
	    new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        char str[22];
		strcpy (str,"Giorcatore ");
		strcat (str," ");
		strcat (str," accettato.");
		str[11]=49+nt;
		puts(str);
		
		argG[nt].s=malloc(sizeof(argT));
		argG[nt].t=malloc(sizeof(argT));
		argG[nt].v=malloc(sizeof(argT));
		argG[nt].r=malloc(sizeof(argT));
		argG[nt].e=malloc(sizeof(argT));
		argG[nt].id=nt;
		argG[nt].s=&new_socket;
		argG[nt].t=&turno;            
		argG[nt].v=&vittoria;        
		argG[nt].r=&ready;	
		argG[nt].e=&end;			  		
		
		if(nt==0)                                 //preparazione attributi specifici del G1
		{
			argG[nt].ownmat=malloc(sizeof(argT));
			argG[nt].enmat=malloc(sizeof(argT));
			argG[nt].ownev=malloc(sizeof(argT));
			argG[nt].enev=malloc(sizeof(argT));
			argG[nt].ownmat=matg1;
			argG[nt].enmat=matg2;
			argG[nt].ownev=e1;
			argG[nt].enev=e2;
			if( pthread_create( &tID[nt] , NULL ,  g , (void*)&argG[nt]) < 0)  //struct e funzione del giocatore passata al thread
        {
            perror("could not create thread");
            return 1;
        }
       // pthread_join( tID[nt] , NULL);
        
			}
			else if(nt==1)						//preparazione attributi specifici del G2
			{
				argG[nt].ownmat=malloc(sizeof(argT));
				argG[nt].enmat=malloc(sizeof(argT));
				argG[nt].ownev=malloc(sizeof(argT));
				argG[nt].enev=malloc(sizeof(argT));
				argG[nt].ownmat=matg2;
				argG[nt].enmat=matg1;
				argG[nt].ownev=e2;
				argG[nt].enev=e1;
				if( pthread_create( &tID[nt] , NULL ,  g , (void*)&argG[nt]) < 0)  //struct e funzione del giocatore passata al thread
        {
            perror("could not create thread");
            return 1;
        }
       // pthread_join( tID[nt] , NULL);
				}
        
        nt++;
 
        
        if(nt==2)
        {
			//pthread_join( tID[0] , NULL);
			//pthread_join( tID[1] , NULL);
			puts("Giocatori pronti, in attesa che entrambi i giocatori inseriscano tutte le navi..");
			
			while(ready!=2);
			puts("Partita iniziata");
			//pthread_join( tID[0] , NULL);
			//pthread_join( tID[1] , NULL);
			while(vittoria==0);
			while(end!=2);			//quando end è a 2 il server si chiude
			}
       
  
    }while(nt<2);
    
    

    if (new_socket<0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
/*
 * Funzione che viene passata durante la creazione di ogni thread(sia per G1 che per G2), che definisce come devono arrivare ed essere ricevuti i messaggi
 */
void *g(void *args)
{
    
    argT *arg = args; 			//puntatore di tipo argT che punta a quello che punta args(cioè alla struct di tipo argT del main)
    int sock=*(int*)arg->s;
    int read_size;
    char cord[2];
    char client_message[2000];  //buffer per ricevere messaggi dal giocatore
    char msg1[101];				//messaggio che contiene ownmat aggiornata ed evento generato
	char msgSblocco[1];			//messaggio per sbloccare il giocatore
	msgSblocco[1]='\0';
	msgSblocco[0]='s';
    
    int i;
	int j;
	for(i=0; i<100; i++)
	{
		((char*)arg->ownmat)[i]=' ';
		
	}
		((char*)arg->ownmat)[100]='\0';
    
   
     if(recv(sock , client_message , 2000 , 0) < 0) 	//aspetto il campo da gioco dal client 
    {
            puts("recv failed");
            
     }
		client_message[100] ='\0';
		
		for(i=0; i<100; i++)
		{
		((char*)arg->ownmat)[i]=client_message[i];	
		}
		
		memset(client_message, 0, 2000);		//clear the message buffer
		
		 
		
			(*(int*)arg->r)++;
           if( (*(int*)arg->r)==1)	(*(int*)arg->t)=arg->id;	//chi ha inserito per primo tutte le navi si appropria del turno
			
			while((*(int*)arg->r)<2);	//aspetto finchè entrambi i giocatori non hanno messo tutte le navi
		    if( send(sock , msgSblocco , strlen(msgSblocco)+1, 0 ) < 0 )   //una volta che entrambi hanno inserito le navi invio il messaggio di sblocco al client
			{
			puts("recv failed");
			}	
        
     
        
        while((*(int*)arg->v)==0 || *(int*)arg->e!=2)          											//inizio ciclo di gioco                  
        {
			while( (*(int*)arg->t)!=arg->id);	//se non è ancora il mio turno aspetto..
			 
			 
			 for(i=0; i<100; i++)
			{
			msg1[i]=((char*)arg->ownmat)[i];	
			}
			msg1[100]=((char*)arg->enev)[0];
			 
				if( send(sock , msg1 , strlen(msg1)+1, 0 ) < 0 )  //reinvio il campo da gioco e l'evento
			{
			puts("recv failed");
			}							
			
			
			
														
         if(recv(sock , client_message , 2000 , 0) < 0)			 //aspetto la coordinata dal client
			{
            puts("recv failed");
			}
      
			client_message[2] ='\0';
     
			cord[0]=client_message[0];
			cord[1]=client_message[1];
			puts(cord);
    
     
			memset(client_message, 0, 2000);	//clear the message buffer
       
        
			int c1=(int)cord[0];
			int c2=(int)cord[1]; 
			int k=(c1-97)+((c2-48)*10);
		 
		
		  ((char*)arg->ownev)[0]=ctrlEventi(cord, ((char*)arg->enmat) );  	//vedi che evento si genera
		 if( ((char*)arg->ownev)[0]=='c' || ((char*)arg->ownev)[0]=='C') 
			 {	
				 ((char*)arg->enmat)[k]=='X';
			 }

			else if( ((char*)arg->ownev)[0]=='v') 
				{
				((char*)arg->enmat)[k]=='X';
				 *(int*)arg->v=arg->id+1;		//id per g1 è 0, se vince g1 vogliamo che vittoria sia 1, viceversa vittora ==2 (quindi sommiamo sempre +1 per avere la vittora giusta)
				 }
			
			
			   		
			if( send(sock , ((char*)arg->ownev) , strlen( ((char*)arg->ownev) )+1, 0 ) < 0 )  //invio il codice di evento al client(ci penserà lui a modificare la matrice per segnare)
			{
			puts("recv failed");
			}	
			
			if(arg->id==0) *(int*)arg->t=1;			//se sono G1 do il cambio a G2
			else *(int*)arg->t=0;					//se sono G2 do il cambio a G1
			}    																						//fine ciclo di gioco
			
			*(int*)arg->e++;
        
        
     
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    //Free the socket pointer
    free(arg->s);
     
    return 0;
}
/*
 * Funzione che restituisce un codice evento generato da un attacco
 */
char ctrlEventi(char c[], char mRead[])
{
		char e;  // =='a' acqua  =='c' colpito  =='C' colpito e affondato =='v' vittoria
		int count;
		int i;
		int j;
			
		int c1=(int)c[0];
		int c2=(int)c[1]; 
	    int k=(c1-97)+((c2-48)*10);
	    
	    
		if (mRead[k]==126) // caso colpo in acqua
		{
			e='a';
			}
			else if (mRead[k]=='-' || mRead[k]=='|') // caso nave colpita
			{
				e='c';
					if (mRead[k]=='|') // controllo se la nave è posizionata verticalmente
					{
						count=0;
						j=k;
						while (j>9 && mRead[j]!=126)
						{
							j=j-10;
							if (mRead[j]=='|') count++;
						}
						j=k;
						while (j<90 && mRead[j]!=126)
						{
							j=j+10;
							if (mRead[j]=='|') count++;
						}
					}							
			else if(mRead[k]=='-')// altrimenti la nave è posizionata orizzontalmente 
			{
				count=0;
				j=k;
				while (j%10!=0 && mRead[j]!=126)
				{
					j--;
					if (mRead[j]=='-') count++;
				}
				j=k;
				while (j%10!=9 && mRead[j]!=126)
				{
				j++;
				if (mRead[j]=='-') count++;
				}
			}	
				
				if (count==0) e='C';
				mRead[k]='X';							
				count=0;
				for(i=0; i<100; i++) // controllo se era l'ultima nave
				{
					if (mRead[i]=='|' || mRead[i]=='-') count++;
				}
				if (count==0) e='v';
					
			}
	return e;  // variabile di ritorno contenente il valore dell'evento
}

