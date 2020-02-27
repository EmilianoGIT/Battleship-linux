/*
 * Battaglia navale: client 
 * Autori: Barigelli Emiliano, Ferretti Francesco
 */

#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write

void printmat(char v[]);							// dichiarazione della funzione che stampa la matrice
int setNave(char c[], char a[],int gn,char mat[]);	// dichiarazione della funzione che posiziona le navi nella matrice


int main(int argc , char *argv[]){
	char e[1];                      // codice evento
	e[0]=' ';						// valore di default dell'evento
	int sock;						// dichirazione del socket
    struct sockaddr_in server;		// struttura che contiene i dati del socket
    char mat[100];					// campo del giocatore 
    char mats[100];					// campo per segnare 
    int listgn[]={5,4,3,3,2};       // vettore che contiene le grandezze delle navi
	char cord[2], asse[1];			// stringhe contenenti coordinata e asse di una nave
    char server_reply[2000];  		// buffer per ricevere messaggi dal server
    
    printf("-BATTAGLIA NAVALE-");
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1) printf("\nImpossibile creare il socket"); 
    else{
			printf("\nCreazione del socket riuscita\nInserire l'indirizzo ip del server");		

			char addIP[20];
			printf("\nIP: ");	
			scanf("%s",addIP);
	
			server.sin_addr.s_addr = inet_addr(addIP);
			server.sin_family = AF_INET;
			server.sin_port = htons( 8888 );
    
    
			//Connect to remote server
		if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
		{
			perror("\nErrore: connessione fallita");
			return 1;
		}
		puts("\nConnessione al server riuscita\n");
		
			int i;
		
		for(i=0; i<100; i++)
		{
			mat[i]=126;
		}
		mat[100]='\0';
		for(i=0; i<100; i++)
		{
			mats[i]=' ';
		}
		mats[100]='\0';
	
	
		int k=0;
	   
		do{
				system("clear");
				printf("\n   --LE MIE NAVI--\n");
				printmat(mat);
				printf("\n   --CAMPO NEMICO--\n");
				printmat(mats);
				printf("Inserimento della %d^ nave grande %d\n" , k+1, listgn[k]);
				printf("Punto di partenza della nave(a-j,0-9): ");
				do{
					int i=0;

					char c=getchar();
        
					while(c!='\n')
					{
						cord[i]=c;
						c=getchar();
						i++; 
						cord[i]='\0';
						}
						}while((cord[0]<97 || cord[0]>106) || (cord[1]<48 || cord[1]>57));
        
		  
       
        
				do{
					int i=0;
					printf("Verso della nave: \no = orizzontale (verso destra) \nv = verticale (verso giù)\n ");
					char c=getchar();
        
					while(c!='\n')
					{
					asse[i]=c;
					c=getchar();
					i++; 
					asse[i]='\0';
					}

					}while(asse[0]!=111 && asse[0]!=118);
        	   
		int succ=setNave(cord,asse,listgn[k],mat);
	   
		if(succ==0) k++;
		
	   
	   }while(k<5);
		
	printf("\n   --LE MIE NAVI--\n");
	printmat(mat);
	printf("\n   --CAMPO NEMICO--\n");
	printmat(mats);

	
	
        
	if(send(sock , mat , strlen(mat)+1, 0 ) < 0 )	// invio il campo da gioco per la prima volta
    {
		puts("Errore: ricezione dati fallita");
		}	
	puts("In attesa...");
		    
   
	if(recv(sock , server_reply , 2000 , 0) < 0)				// aspetto lo sblocco dal server
    {
        
		puts("Errore: ricezione dati fallita");
		}
	//end of string marker
	server_reply[1] ='\0';
		
	//clear the message buffer
	memset(server_reply, 0, 2000);
   
    
    
    
    do {
			puts("In attesa che il nemico completi il suo turno");	
			if(recv(sock , server_reply , 2000 , 0) < 0)
			{
        
				puts("Errore: ricezione dati fallita");
				}
			//end of string marker
			server_reply[101] ='\0';
		
			for(i=0; i<100; i++)
			{
				mat[i]=server_reply[i];
				}
			e[0]=server_reply[100];
			
			memset(server_reply, 0, 2000);		//clear the message buffer
		
			printf("\n   --LE MIE NAVI--\n");
			printmat(mat);
			printf("\n   --CAMPO NEMICO--\n");
			printmat(mats);
			if(e[0]=='a') puts("\nIl tuo avversario ha mancato il bersaglio");
			else if(e[0]=='c') puts("\nUna tua nave è stata colpita");
			else if(e[0]=='C') puts("\nUna tua nave è stata affondata");
			else if(e[0]=='v') puts("\nSCONFITTA");
			if (e[0]!='v')				//se il valore dell'evento è vittoria si interrompe il turno e la partita termina
			{	
				e[0]=' ';
				do
				{
				int i=0;
				printf("\nCoordinata di attacco (a-j,0-9): ");
				char c=getchar();
        
				while(c!='\n')
				{
					cord[i]=c;
					c=getchar();
					i++; 
					cord[i]='\0';
				}
				}while((cord[0]<97 || cord[0]>106) || (cord[1]<48 || cord[1]>57));
	
	

				if( send(sock , cord , strlen(cord)+1, 0 ) < 0 )       //invio la cordinata al server
				{
					puts("\nErrore: invio dati fallito");
				}	
		
				if(recv(sock , server_reply , 2000 , 0) < 0)
				{        
					puts("recv failed");
				}
				
				server_reply[1] ='\0';	//end of string marker
				e[0]=server_reply[0];

				memset(server_reply, 0, 2000);				//clear the message buffer
	
				int c1=(int)cord[0];
				int c2=(int)cord[1]; 
				int k=(c1-97)+((c2-48)*10);
	    
				if(e[0]=='a') mats[k]=126;
				else if(e[0]=='c' || e[0]=='C' || e[0]=='v') mats[k]='X';
        
				printf("\n   --LE MIE NAVI--\n");
				printmat(mat);
				printf("\n   --CAMPO NEMICO--\n");
				printmat(mats);
				if(e[0]=='a') puts("\nBersaglio mancato");
				else if(e[0]=='c') puts("\nHai colpito una nave");
				else if(e[0]=='C') puts("\nHai affondato una nave");
				else if(e[0]=='v') puts("\nVITTORIA");
				if (e[0]!='v')e[0]=' ';
				}	
			}while (e[0]!='v');
  


 
 }				
 puts("GAME OVER");
 close(sock);

return 0;
	}


void printmat(char v[])                                           //stampo un vettore da 100 elementi come una matrice 10*10
{
	int c=48;
	printf("   a b c d e f g h i j\n   ___________________\n");
	printf("%s|", &c);
	
	int i;
	for(i=0; i <100; i++)
	{
		if(v[i]=='|' || v[i]=='-') printf(" O");
		else printf(" %c", v[i]);
	    if((i%10)==9 && i!=99)
		{
			c++;
			printf("\n%s|", &c);
		}
	}
	printf("\n");
	}

int setNave(char c[],char a[],int gn,char mat[])            //metodo per posizionare correttamente la nave
{		
		int f;
		int fo=0;                  //flag di errore, se è 0 alla fine piazza la nave, altrimenti non viene piazzata
		char *pm;
		pm=mat;
	
		int c1=(int)c[0];
		int c2=(int)c[1]; 
	    int k=(c1-97)+((c2-48)*10);
	   
		if(a[0]=='o')										    //posizionamento orizzontale
		{
			if( ((k%10)>((k+(gn-1))%10)) ) fo=1;            //controllo se gli estremi sono ordinati..
			else{
					for(f=k; f<=(k+(gn-1)); f++)    //controllo che la nave non vada a sovrapporsi ad altri pezzi di nave..
					{
					if(pm[f]=='|' || pm[f]=='-') fo=1;
					}  
				}
			if(fo!=1)							//se non ho riscontrato incongruenze continuo 
			{                           
				if( (k+(gn-1))%10==9)      //caso |. . . . . . . O O O|
				{
					if(pm[f]=='|' || pm[f]=='-') fo=1;
				}
				else if(k%10==0)  		//caso  |O O O . . . . . . .|
				{
					if(pm[f]=='|' || pm[f]=='-') fo=1;
				}
				else           		//altrimenti se ho una situazione come questa |. . . . O O O . . . . |
				{
					if(pm[f]=='|' || pm[f]=='-') fo=1;
				}
							
			if(fo!=1)	//se non ho riscontrato altre incongruenze continuo 
			{
				for(f=k-10; f<=((k-10)+(gn-1)); f++) //controllo se ci sono pezzi di navi adiacenti di sopra
			    {
					if(pm[f]=='|' || pm[f]=='-') fo=1;
			    }  
			   for(f=k+10; f<=((k+10)+(gn-1)); f++) //controllo se ci sono pezzi di navi adiacenti di sotto
			   {
				   if(pm[f]=='|' || pm[f]=='-') fo=1;
			   } 
			}
		    }
			   
			if(fo==1) printf("Operazione non consentita\n");
			else if(fo==0)
			{
				printf("Nave posizionata con successo\n");   
				int j;
				for(j=k; j<=k+(gn-1); j++)
				{
					pm[j]='-';
				}
			}
			   
		} //fine per posizionamento orizzontale
		else if(a[0]=='v')							//posizionamento verticale
		{
			if(k+((gn-1)*10)>99)   fo=1;
		    else 
		    {
			   for(f=k; f<=(k+((gn-1)*10)); f=f+10)    //controllo che la nave non vada a sovrapporsi ad altri pezzi di nave..
			   {
				   if(pm[f]=='|' || pm[f]=='-') fo=1;
			   }  
		    }	              
		if(fo!=1)
	    {
			if(pm[k-10]=='-'  || pm[k-10]=='|' || pm[k+(gn*10)]=='|' || pm[k+(gn*10)]=='-') fo=1;			
				if(fo!=1)	//se non ho riscontrato altre incongruenze continuo 
				{
					if (k%10==0)
					{
						for(f=k+1; f<=((k+1)+((gn-1)*10)); f=f+10) // caso |O
						{
							if(pm[f]=='|' || pm[f]=='-') fo=1;
						}
					}
					else if (k%10==9)
					{
						for(f=k-1; f<=((k-1)+((gn-1)*10)); f=f+10) // caso O|
						{
							if(pm[f]=='|' || pm[f]=='-') fo=1;
						}
					}
					else
					{
						for(f=k-1; f<=((k-1)+((gn-1)*10)); f=f+10) //controllo se ci sono pezzi di navi adiacenti di sinistra
						{
							if(pm[f]=='|' || pm[f]=='-') fo=1;
						}  
						for(f=k+1; f<=((k+1)+((gn-1)*10)); f=f+10) //controllo se ci sono pezzi di navi adiacenti di destra
						{
							if(pm[f]=='|' || pm[f]=='-') fo=1;
						} 	
					}	   
				}
	   }
		   if(fo==1) printf("Operazione non consentita\n");
		   else if(fo==0)
		   {
			    printf("Nave posizionata con successo\n");   
				int j;
				for(j=k; j<=(k+((gn-1)*10)); j=j+10)
				{
					pm[j]='|';
				}
		   }
	  }
	return fo;		
}
