#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#define PORT 3000

extern int errno;

typedef struct thData{
	int idThread;
	int cl;
}thData;


void cautareFisier(char file[],char rezultat[])
{
    FILE* fisier;
    fisier=fopen("FilesDetails.txt","r");
    if(fisier==NULL)
    {
        perror("[masina]Eroare la deschidere fisier detalii!\n");
        return errno;
    }

    char sir[1000],sep[]=" ";
    bzero(sir,1000);

    while(fgets(sir,1000,fisier) != NULL)
    {
        char *p=strtok(sir,sep);
        if(strcmp(p,file)==0)
        {
            p=strtok(NULL,sep);
            while(p!=NULL)
            {
                char s[100];
                bzero(s,100);
                strcpy(s,p);
                sprintf(rezultat,"%s %s",rezultat,s);
                p=strtok(NULL,sep);
            }
            break;
        }
    }

    if(fclose(fisier) == -1)
    {
        perror("[masina]Eroare la inchidere fisier cu datele stocate!\n");
        return errno;
    }

    if(rezultat[0]==NULL)
    {
        char text[]="File not found!";
        strcpy(rezultat,text);
    }
}

void adaugareFisier(char fileComplete[])
{
    FILE* fisier;
    fisier=fopen("FilesDetails.txt","a");
    if(fisier==NULL)
    {
        perror("[masina]Eroare la deschidere fisier detalii!\n");
        return errno;
    }

    fileComplete[strlen(fileComplete)]='\n';
    fprintf(fisier,"%s",fileComplete);

    if(fclose(fisier) == -1)
    {
        perror("[masina]Eroare la inchidere fisier cu datele stocate!\n");
        return errno;
    }
}

void gasireDate(char rezultat[])
{
    FILE* fisier;
    fisier=fopen("FilesDetails.txt","r");
    if(fisier==NULL)
    {
        perror("[masina]Eroare la deschidere fisier detalii!\n");
        return errno;
    }

    char sir[1000];
    bzero(sir,1000);

    while(fgets(sir,1000,fisier) != NULL)
    {
        sprintf(rezultat,"%s%s",rezultat,sir);
    }

    if(fclose(fisier) == -1)
    {
        perror("[masina]Eroare la inchidere fisier cu datele stocate!\n");
        return errno;
    }
}


void tratareCereri(void *arg)
{
    struct thData tdL;
	tdL= *((struct thData*)arg);

    char string_received[1000];
    bzero(string_received, 1000);

    if (read (tdL.cl, string_received, 1000) <= 0)
    {
        printf("[masina]- thread %d - ",tdL.idThread);
        fflush(stdout);
        perror ("Eroare la read() de la server.\n");
        return errno;
    }

    printf ("[masina]- thread %d - Cererea serverului a fost receptionata...%s\n", tdL.idThread, string_received);

    char string_send[10000];
    bzero(string_send,10000);

    if(string_received[0]=='0')
    {
        strcpy(string_received,string_received+1);
        cautareFisier(string_received,string_send);
    }
    else if(string_received[0]=='1')
    {
        for(int i=1;i<=strlen(string_received);++i) string_received[i-1]=string_received[i];

        char aux[1000],sep[]=" ";
        bzero(aux,1000);
        strcpy(aux,string_received);
        char *file=strtok(aux,sep);

        char rezultat[255];
        bzero(rezultat,255);

        cautareFisier(file,rezultat);

        if(strcmp(rezultat,"File not found!")==0)
        {
            adaugareFisier(string_received);
            strcpy(string_send,"Adaugat!");
        }
        else strcpy(string_send,"Este adaugat deja!");
    }
    else if(string_received[0]=='2')
    {
        gasireDate(string_send);
    }

    printf("[masina]- thread %d - Trimitem raspunsul...\n",tdL.idThread);

    if (write (tdL.cl, string_send, strlen(string_send)) <= 0)
    {
        printf("[masina]- thread %d - ",tdL.idThread);
        fflush(stdout);
        perror ("[masina]Eroare la write() catre server.\n");
        return errno;
    }
	else printf ("[masina]- thread %d - Raspunsul a fost trasmis cu succes.\n",tdL.idThread);

}

static void *treat(void * arg)
{
    struct thData tdL;
    tdL = *((struct thData*)arg);

    printf ("[masina]- thread %d - Se asteapta cererea serverului ...\n", tdL.idThread);
    fflush (stdout);

    if(pthread_detach(pthread_self()) !=0)
    {
        printf("[masina]- thread %d - ", tdL.idThread);
        fflush(stdout);
        perror("Eroare la detach!");
        return errno;
    }

    tratareCereri((struct thData*)arg);

    close ((intptr_t)arg);
    return(NULL);

}

int main ()
{
    struct sockaddr_in machine;
    struct sockaddr_in server;
    int sd;

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[masina]Eroare la socket().\n");
        return errno;
    }

    bzero (&machine, sizeof (machine));
    bzero (&server, sizeof (server));

    machine.sin_family = AF_INET;
    machine.sin_addr.s_addr = htonl (INADDR_ANY);
    machine.sin_port = htons (PORT);


    if (bind (sd, (struct sockaddr *) &machine, sizeof (struct sockaddr)) == -1)
    {
        perror ("[masina]Eroare la bind().\n");
        return errno;
    }

    int optval=1;
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));

    if (listen (sd, 5) == -1)
    {
        perror ("[masina]Eroare la listen().\n");
        return errno;
    }


    pthread_t th[100];
	int i=0;
    while (1)
    {
        int client;
        int length = sizeof (server);
        thData * td;

        printf ("[masina]Asteptam la portul %d...\n",PORT);
        fflush (stdout);

        client = accept (sd, (struct sockaddr *) &server, &length);

        if (client < 0)
        {
            perror ("[masina]Eroare la accept().\n");
            continue;
        }

        td=(struct thData*)malloc(sizeof(struct thData));
        td->idThread=i++;
        td->cl=client;

        pthread_create(&th[i], NULL, &treat, td);

    }
}

