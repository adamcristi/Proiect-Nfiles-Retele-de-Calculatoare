#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

extern int errno;

int port;


unsigned short verificareIP(char ip[])
{
    for(int i=0;i<strlen(ip);++i)
    {
        if(ip[i]<'0' || ip[i]>'9') return 0;
    }

    int numar=atoi(ip);
    if(numar<1000 || numar>9999) return 0;

    return 1;
}
void red() { printf("\033[0;31m"); }
void cyan() { printf("\033[0;36m"); }
void reset() { printf("\033[0m"); }

void afisare(char raspuns[])
{
    for(int i=1; i<strlen(raspuns); ++i)
    {
        if(raspuns[i]=='I' && raspuns[i+1]=='O')
        {
            printf("\n%c%c%c",raspuns[i],raspuns[i+1],raspuns[i+2]);
            fflush(stdout);
            i+=3;
        }
        else if(('A'<=raspuns[i] && raspuns[i]<='Z') && i!=1) printf("\n");
        printf("%c",raspuns[i]);
        fflush(stdout);
    }
}

int main (int argc, char *argv[])
{
    int sd;
    struct sockaddr_in server;
    char string_send[255];
    bzero(string_send,255);

    if (argc != 3)
    {
        printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    port = atoi (argv[2]);

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("Eroare la socket().\n");
        return errno;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons (port);

    if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
        perror ("[client]Eroare la connect().\n");
        return errno;
    }

    char answer[11];
    bzero(answer,11);
    printf("[client] Doriti sa introduceti ");
    cyan(); printf("ip-ul masinii"); reset();
    printf("? ");
    cyan(); printf("(y/n)"); reset();
    printf(": ");
    fflush(stdout);

    if(read(0,answer,11) == -1)
    {
        perror("[client] Eroare la citire raspuns!\n");
        return errno;
    }

    while( (answer[0]!='y' && answer[0]!='n') || ((answer[0]=='y' || answer[0]=='n') && answer[1]!='\n') )
    {
        printf("[client] ");
        red(); printf("Ati introdus o litera sau un cuvant gresit.\n"); reset();
        printf("[client] Doriti sa introduceti ");
        cyan(); printf("ip-ul masinii"); reset();
        printf("? ");
        cyan(); printf("(y/n)"); reset();
        printf(": ");
        fflush(stdout);
        if(read(0,answer,11) == -1)
        {
            perror("[client] Eroare la citire raspuns!\n");
            return errno;
        }
    }

    if(answer[0] == 'y')
    {
        char machine_ip[11];
        bzero(machine_ip,11);
        printf("[client] Introduceti ");
        cyan(); printf("ip-ul masinii "); reset();
        printf("in care sa se caute fisierul: ");
        fflush(stdout);

        if(read(0,machine_ip,11) == -1)
        {
            perror("[client] Eroare la citire ip masina!\n");
            return errno;
        }

         machine_ip[strlen(machine_ip)-1]='\0';

        while(verificareIP(machine_ip)==0)
        {
            printf("[client] ");
            red(); printf("Ati introdus o adresa IP invalida.\n"); reset();
            printf("[client] Reintroduceti ");
            cyan(); printf("ip-ul masinii "); reset();
            printf("in care sa se caute fisierul: ");
            fflush(stdout);

            if(read(0,machine_ip,11) == -1)
            {
                perror("[client] Eroare la citire ip masina!\n");
                return errno;
            }

            machine_ip[strlen(machine_ip)-1]='\0';
        }

        strcpy(string_send,machine_ip);
    }

    char file[100];
    bzero (file, 100);
    printf ("[client] Introduceti ");
    cyan(); printf("numele fisierului"); reset();
    printf(": ");
    fflush (stdout);

    if(read (0, file, 100) == -1)
    {
        perror("[client] Eroare la citire nume fisier!\n");
        return errno;
    }
    file[strlen(file)-1]='\0';

    if(answer[0] == 'y') sprintf(string_send,"%s %s",string_send,file);
    else strcpy(string_send,file);


    if (write (sd, string_send, 255) == -1)
    {
        perror ("[client]Eroare la write() spre server.\n");
        return errno;
    }

    char string_received[1000];
    bzero(string_received, 1000);

    if (read (sd, string_received, 1000) == -1)
    {
        perror ("[client]Eroare la read() de la server.\n");
        return errno;
    }

    if(strstr(string_received,"Fisierul cautat nu se gaseste in masina cu ip-ul dat!\n")!=0 || strcmp(string_received,"Fisierul cautat nu se gaseste in nici o masina!\n")==0)
    {
        bzero(answer,11);
        printf("[client] Doriti sa adaugati ");
        cyan(); printf("fisierul"); reset();
        printf("? ");
        cyan(); printf("(y/n)"); reset();
        printf(": ");
        fflush(stdout);

        if(read(0,answer,11) == -1)
        {
            perror("[client] Eroare la citire raspuns!\n");
            return errno;
        }

        while( (answer[0]!='y' && answer[0]!='n') || ((answer[0]=='y' || answer[0]=='n') && answer[1]!='\n') )
        {
            printf("[client] ");
            red(); printf("Ati introdus o litera sau un cuvant gresit.\n"); reset();
            printf("[client] Doriti sa adaugati ");
            cyan(); printf("fisierul"); reset();
            printf("? ");
            cyan(); printf("(y/n)"); reset();
            printf(": ");
            fflush(stdout);
            if(read(0,answer,11) == -1)
            {
                perror("[client] Eroare la citire raspuns!\n");
                return errno;
            }
        }

        if(answer[0] == 'y')
        {
            bzero(string_send, 255);
            strcpy(string_send,"da");

            bzero(answer,11);
            printf("[client] Doriti sa introduceti ");
            cyan(); printf("calea absoluta a fisierului"); reset();
            printf("? ");
            cyan(); printf("(y/n)"); reset();
            printf(": ");
            fflush(stdout);

            if(read(0,answer,11) == -1)
            {
                perror("[client] Eroare la citire raspuns!\n");
                return errno;
            }

            while( (answer[0]!='y' && answer[0]!='n') || ((answer[0]=='y' || answer[0]=='n') && answer[1]!='\n') )
            {
                printf("[client] ");
                red(); printf("Ati introdus o litera sau un cuvant gresit.\n"); reset();
                printf("[client] Doriti sa introduceti ");
                cyan(); printf("calea absoluta a fisierului"); reset();
                printf("? ");
                cyan(); printf("(y/n)"); reset();
                printf(": ");
                fflush(stdout);
                if(read(0,answer,11) == -1)
                {
                    perror("[client] Eroare la citire raspuns!\n");
                    return errno;
                }
            }

            if(answer[0]=='y')
            {
                char path[255];
                bzero (path, 255);

                printf ("[client] Introduceti ");
                cyan(); printf("calea absoluta a fisierului"); reset();
                printf(": ");
                fflush (stdout);

                if(read (0, path, 255) == -1)
                {
                    perror("[client] Eroare la citire nume fisier!\n");
                    return errno;
                }
                path[strlen(path)-1]='\0';

                sprintf(string_send,"%s %s",string_send,path);
            }

            if (write (sd, string_send, 255) == -1)
            {
                perror ("[client]Eroare la write() spre server.\n");
                return errno;
            }
            bzero(string_received, 1000);
            if (read (sd, string_received, 1000) == -1)
            {
                perror ("[client]Eroare la read() de la server.\n");
                return errno;
            }

        }

        if(strstr(string_received,"IP Masina gasit si conectat!\n")==0)
        {
            printf("[client] ");
            cyan(); printf("%s",string_received); reset();
        }
        else
        {
            char sep[]="\n";
            char *text=strtok(string_received,sep);
            printf("[client] ");
            cyan(); printf("%s\n",text); reset();
            text=strtok(NULL,sep);
            printf("[client] ");
            cyan(); printf("%s\n",text); reset();
        }
    }
    else
    {
        if(strstr(string_received,"IP Masina gasita dar neconectat!\n")!=0)
        {
            char sep[]="\n";
            char *text=strtok(string_received,sep);
            printf("[client] ");
            cyan(); printf("%s\n",text); reset();
            text=strtok(NULL,sep);
            printf("[client] ");
            cyan(); printf("%s\n",text); reset();
            text=strtok(NULL,sep);
            printf("[client] ");
            cyan(); printf("%s\n",text); reset();
            text=strtok(NULL,sep);
            while(text)
            {
                printf("%s\n",text);
                text=strtok(NULL,sep);
            }
        }
        else if(strcmp(string_received,"IP Masina dat nu a fost gasit, dar a fost adaugat la lista daca masina este activa!\n")==0)
        {
            printf("[client] ");
            cyan(); printf("%s",string_received); reset();
        }
        else
        {
            if(string_received[0]!=' ')
            {
                char aux[1000],sep[]="\n";
                bzero(aux,1000);
                strcpy(aux,string_received);
                char *text=strtok(aux,sep);
                printf("[client] ");
                cyan(); printf("%s\n",text); reset();
                text=strtok(NULL,sep);
                strcpy(string_received,text);
            }
            printf("[client] ");
            cyan(); printf("Caracteristici fisier:\n"); reset();
            afisare(string_received);
        }
    }
    close (sd);
}

