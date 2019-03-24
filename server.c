#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <dirent.h>

#define PORT 2025

extern int errno;

typedef struct thData{
	int idThread;
	int cl;
}thData;



void gasireIPCuFisierDat(char file[], char rezultat[])
{
    char masiniConectate[1000];
    bzero(masiniConectate,1000);

    FILE* fisier;
    fisier=fopen("MachinesIPs.txt","r");

    char ip_masina[11];
    bzero(ip_masina,11);
    for(int i=strlen(file)-1;i>=0;--i) file[i+1]=file[i];
    file[0]='0';

    while( fgets(ip_masina,11,fisier) != NULL )
    {
        int sd_masina;
        struct sockaddr_in masina;

        int port_masina = atoi (ip_masina);

        if ((sd_masina = socket (AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror ("[server]Eroare la socket().\n");
            return errno;
        }

        char adresa[]="127.0.0.1";

        masina.sin_family = AF_INET;
        masina.sin_addr.s_addr = inet_addr(adresa);
        masina.sin_port = htons (port_masina);

        if (connect (sd_masina, (struct sockaddr *) &masina,sizeof (struct sockaddr)) == -1)
        {
            continue;
        }

        sprintf(masiniConectate,"%s%s",masiniConectate,ip_masina);

        if (write (sd_masina, file, strlen(file)) == -1)
        {
            perror ("[server]Eroare la write() spre masina.\n");
            return errno;
        }

        char raspuns_masina[1000];
        bzero(raspuns_masina, 1000);

        if (read (sd_masina, raspuns_masina, 1000) == -1)
        {
            perror ("[server]Eroare la read() de la masina.\n");
            return errno;
        }

        if(strcmp(raspuns_masina,"File not found!")!=0) sprintf(rezultat,"%s%s",rezultat,ip_masina);

        close(sd_masina);

    }

    if(fclose(fisier) == -1)
    {
        perror("[server]Eroare la inchidere fisier cu ip-urile stocate!\n");
        return errno;
    }

    fisier=fopen("MachinesIPs.txt","w");
    if(fisier==NULL)
    {
        perror("[server]Eroare la deschidere fisier detalii!\n");
        return errno;
    }

    fprintf(fisier,"%s",masiniConectate);

    if(fclose(fisier) == -1)
    {
        perror("[server]Eroare la inchidere fisier cu ip-urile stocate!\n");
        return errno;
    }

}

void actualizareFisiereInMasini()
{
    char masiniConectate[1000];
    bzero(masiniConectate,1000);

    FILE* fisier;
    fisier=fopen("MachinesIPs.txt","r");

    char ip_masina[11];
    bzero(ip_masina,11);
    char sir[]="2";

    while( fgets(ip_masina,11,fisier) != NULL )
    {
        int sd_masina;
        struct sockaddr_in masina;

        int port_masina = atoi (ip_masina);

        if ((sd_masina = socket (AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror ("[server]Eroare la socket().\n");
            return errno;
        }

        char adresa[]="127.0.0.1";

        masina.sin_family = AF_INET;
        masina.sin_addr.s_addr = inet_addr(adresa);
        masina.sin_port = htons (port_masina);

        if (connect (sd_masina, (struct sockaddr *) &masina,sizeof (struct sockaddr)) == -1)
        {
            continue;
        }

        sprintf(masiniConectate,"%s%s",masiniConectate,ip_masina);

        if (write (sd_masina, sir, strlen(sir)) == -1)
        {
            perror ("[server]Eroare la write() spre masina.\n");
            return errno;
        }

        char raspuns_masina[10000];
        bzero(raspuns_masina, 10000);

        if (read (sd_masina, raspuns_masina, 10000) == -1)
        {
            perror ("[server]Eroare la read() de la masina.\n");
            return errno;
        }

        raspuns_masina[strlen(raspuns_masina)]='\n';
        char sep[]="\n";
        char *p=strtok(raspuns_masina,sep);

        while(p)
        {
            char aux[1000];
            bzero(aux,1000);

            strcpy(aux,p);

            adaugareFisierInMasini(aux);
            p=strtok(NULL,sep);
        }

        close(sd_masina);

    }

    if(fclose(fisier) == -1)
    {
        perror("[server]Eroare la inchidere fisier cu ip-urile stocate!\n");
        return errno;
    }

    fisier=fopen("MachinesIPs.txt","w");
    if(fisier==NULL)
    {
        perror("[server]Eroare la deschidere fisier detalii!\n");
        return errno;
    }

    fprintf(fisier,"%s",masiniConectate);

    if(fclose(fisier) == -1)
    {
        perror("[server]Eroare la inchidere fisier cu ip-urile stocate!\n");
        return errno;
    }
}

void cautareFisierCuIP(char machine_ip[], char file[], char rezultat[])
{
    char raspuns[1000];
    bzero(raspuns,1000);

    FILE* fisier;
    fisier=fopen("MachinesIPs.txt","r");
    if(fisier==NULL)
    {
        perror("[server]Eroare la deschidere fisier detalii!\n");
        return errno;
    }

    char ip_masina[11];
    bzero(ip_masina,11);
    int masina_gasita = 0;
    int masina_conectata = 1;
    for(int i=strlen(file)-1;i>=0;--i) file[i+1]=file[i];
    file[0]='0';


    while( fgets(ip_masina,11,fisier) != NULL )
    {
        int sd_masina;
        struct sockaddr_in masina;

        int port_masina = atoi (ip_masina);

        if(port_masina == atoi(machine_ip))
        {
            masina_gasita=1;

            if ((sd_masina = socket (AF_INET, SOCK_STREAM, 0)) == -1)
            {
                perror ("[server]Eroare la socket().\n");
                return errno;
            }

            char adresa[]="127.0.0.1";

            masina.sin_family = AF_INET;
            masina.sin_addr.s_addr = inet_addr(adresa);
            masina.sin_port = htons (port_masina);

            if (connect (sd_masina, (struct sockaddr *) &masina,sizeof (struct sockaddr)) == -1)
            {
                masina_conectata = 0;
                break;
            }

            if (write (sd_masina, file, strlen(file)) == -1)
            {
                perror ("[server]Eroare la write() spre masina.\n");
                return errno;
            }

            char raspuns_masina[1000];
            bzero(raspuns_masina, 1000);

            if (read (sd_masina, raspuns_masina, 1000) == -1)
            {
                perror ("[server]Eroare la read() de la masina.\n");
                return errno;
            }

            strcpy(raspuns,raspuns_masina);

            close (sd_masina);

            break;
        }
    }

    if(fclose(fisier) == -1)
    {
        perror("[server]Eroare la inchidere fisier cu ip-urile stocate!\n");
        return errno;
    }

    if(masina_gasita)
    {
        if(masina_conectata)
        {
            char text[]="IP Masina gasit si conectat!\n";
            if(strcmp(raspuns,"File not found!")!=0) sprintf(rezultat,"%s%s",text,raspuns);
            else
            {
                char aux[]="Fisierul cautat nu se gaseste in masina cu ip-ul dat!\n";
                sprintf(rezultat,"%s%s",text,aux);
            }
        }
        else
        {
            char text[]="IP Masina gasita dar neconectat!\nMasini deconectate sterse!\nAdrese ip a masinilor conectate unde poate fi gasit fisierul:\n";
            strcpy(rezultat,text);

            char connected_machines_with_file[1000];
            bzero(connected_machines_with_file,1000);

            strcpy(file,file+1);

            gasireIPCuFisierDat(file,connected_machines_with_file);

            sprintf(rezultat,"%s%s",rezultat,connected_machines_with_file);
        }
    }
    else
    {
        FILE* fisier_ip;
        fisier_ip=fopen("MachinesIPs.txt","a");
        if(fisier_ip==NULL)
        {
            perror("[server]Eroare la deschidere fisier detalii!\n");
            return errno;
        }

        machine_ip[strlen(machine_ip)]='\n';
        fprintf(fisier_ip,"%s",machine_ip);

        if(fclose(fisier_ip) == -1)
        {
            perror("[server]Eroare la inchidere fisier cu ip-urile stocate!\n");
            return errno;
        }

        actualizareFisiereInMasini();

        char text[]="IP Masina dat nu a fost gasit, dar a fost adaugat la lista daca masina este activa!\n";
        strcpy(rezultat,text);
    }

}

void cautareFisierFaraIP(char file[], char rezultat[])
{
    char masiniConectate[1000];
    bzero(masiniConectate,1000);

    FILE* fisier;
    fisier=fopen("MachinesIPs.txt","r");
    if(fisier==NULL)
    {
        perror("[server]Eroare la deschidere fisier detalii!\n");
        return errno;
    }

    char ip_masina[11];
    bzero(ip_masina,11);
    for(int i=strlen(file)-1;i>=0;--i) file[i+1]=file[i];
    file[0]='0';

    while( fgets(ip_masina,11,fisier) != NULL )
    {
        int sd_masina;
        struct sockaddr_in masina;

        int port_masina = atoi (ip_masina);

        if ((sd_masina = socket (AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror ("[server]Eroare la socket().\n");
            return errno;
        }

        char adresa[]="127.0.0.1";

        masina.sin_family = AF_INET;
        masina.sin_addr.s_addr = inet_addr(adresa);
        masina.sin_port = htons (port_masina);

        if (connect (sd_masina, (struct sockaddr *) &masina,sizeof (struct sockaddr)) == -1)
        {
            continue;
        }

        sprintf(masiniConectate,"%s%s",masiniConectate,ip_masina);

        if (write (sd_masina, file, strlen(file)) == -1)
        {
            perror ("[server]Eroare la write() spre masina.\n");
            return errno;
        }

        char raspuns_masina[1000];
        bzero(raspuns_masina, 1000);

        if (read (sd_masina, raspuns_masina, 1000) == -1)
        {
            perror ("[server]Eroare la read() de la masina.\n");
            return errno;
        }

        if(strcmp(raspuns_masina,"File not found!")!=0) strcpy(rezultat,raspuns_masina);
        else strcpy(rezultat,"Fisierul cautat nu se gaseste in nici o masina!\n");

        close (sd_masina);

    }

    if(fclose(fisier) == -1)
    {
        perror("[server]Eroare la inchidere fisier cu ip-urile stocate!\n");
        return errno;
    }

    fisier=fopen("MachinesIPs.txt","w");
    if(fisier==NULL)
    {
        perror("[server]Eroare la deschidere fisier detalii!\n");
        return errno;
    }

    fprintf(fisier,"%s",masiniConectate);

    if(fclose(fisier) == -1)
    {
        perror("[server]Eroare la inchidere fisier cu ip-urile stocate!\n");
        return errno;
    }

    if(rezultat[0]==NULL)
    {
        char aux[]="Fisierul cautat nu se gaseste in nici o masina!\n";
        strcpy(rezultat,aux);
    }

}

void cautareFisierPrintreDirectoare(char path[],char file[], char rezultat[])
{
    struct stat statut;
    stat(path,&statut);
    if( S_ISDIR(statut.st_mode) )
    {
        DIR *director;
        director=opendir(path);
        if(director==NULL)
        {
            perror("Eroare la deschidere director!\n");
            return errno;
        }
        else
        {
            struct dirent *fisier_actual;
            while( ( fisier_actual=readdir(director) ) != NULL )
            {
                stat(fisier_actual->d_name,&statut);
                if( S_ISDIR(statut.st_mode) && strcmp(fisier_actual->d_name,".")!=0 && strcmp(fisier_actual->d_name,"..")!=0)
                {
                    char noul_director[256];
                    sprintf(noul_director, "%s/%s", path, fisier_actual->d_name);
                    cautareFisierPrintreDirectoare(noul_director,file,rezultat);
                }
                if(strstr(fisier_actual->d_name,file)!=NULL)
                {
                    char path_complet[256];
                    bzero(path_complet,256);
                    sprintf(rezultat,"%s/%s",path,fisier_actual->d_name);
                }
            }
        }
        if(closedir(director) == -1)
        {
            perror("Eroare la inchidere director!\n");
            return errno;
        }
    }
}

void determinareStatutFisier(char path[], char rezultat[])
{
    char comanda[255];
    bzero(comanda,255);

    sprintf(comanda,"%s %s","stat",path);

    FILE *fd;
    fd=popen(comanda,"r");
    if(fd==NULL)
    {
        perror("[server]Eroare la rulare comanda!\n");
        return errno;
    }

    char sir[255];
    bzero(sir,255);

    while(fgets(sir,255,fd) != NULL)
    {
        char sep[]=" \t\n";
        char *p=strtok(sir,sep);
        while(p)
        {
            if(strcmp(p,"File:")==0) break;
            else
            {
                if('a'<=*(p+0) && *(p+0)<='z') sprintf(rezultat,"%s %s",rezultat,"Type:");
                sprintf(rezultat,"%s %s",rezultat,p);
                p=strtok(NULL,sep);
            }
        }
    }
}

void adaugareFisierInMasini(char file[])
{
    char masiniConectate[1000];
    bzero(masiniConectate,1000);

    FILE* fisier;
    fisier=fopen("MachinesIPs.txt","r");

    char ip_masina[11];
    bzero(ip_masina,11);
    for(int i=strlen(file)-1;i>=0;--i) file[i+1]=file[i];
    file[0]='1';


    while( fgets(ip_masina,11,fisier) != NULL )
    {
        int sd_masina;
        struct sockaddr_in masina;

        int port_masina = atoi (ip_masina);

        if ((sd_masina = socket (AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror ("[server]Eroare la socket().\n");
            return errno;
        }

        char adresa[]="127.0.0.1";

        masina.sin_family = AF_INET;
        masina.sin_addr.s_addr = inet_addr(adresa);
        masina.sin_port = htons (port_masina);

        if (connect (sd_masina, (struct sockaddr *) &masina,sizeof (struct sockaddr)) == -1)
        {
            continue;
        }

        sprintf(masiniConectate,"%s%s",masiniConectate,ip_masina);

        if (write (sd_masina, file, strlen(file)) == -1)
        {
            perror ("[server]Eroare la write() spre masina.\n");
            return errno;
        }

        char raspuns_masina[100];
        bzero(raspuns_masina, 100);

        if (read (sd_masina, raspuns_masina, 100) == -1)
        {
            perror ("[server]Eroare la read() de la masina.\n");
            return errno;
        }

        close(sd_masina);

    }

    if(fclose(fisier) == -1)
    {
        perror("[server]Eroare la inchidere fisier cu ip-urile stocate!\n");
        return errno;
    }

    fisier=fopen("MachinesIPs.txt","w");
    if(fisier==NULL)
    {
        perror("[server]Eroare la deschidere fisier detalii!\n");
        return errno;
    }

    fprintf(fisier,"%s",masiniConectate);

    if(fclose(fisier) == -1)
    {
        perror("[server]Eroare la inchidere fisier cu ip-urile stocate!\n");
        return errno;
    }
}

void tratareCereri(void *arg)
{
    struct thData tdL;
	tdL= *((struct thData*)arg);

    char string_received[255];
    bzero (string_received, 255);

    if (read (tdL.cl, string_received, 255) <= 0)
    {
        printf("[server]- thread %d - ",tdL.idThread);
        fflush(stdout);
        perror ("Eroare la read() de la client.\n");
        return errno;
    }

    printf ("[server]- thread %d - Cererea a fost receptionata...%s\n", tdL.idThread, string_received);

    char file[100];
    bzero (file, 100);

    char machine_ip[11];
    bzero(machine_ip,11);

    if('0' <= string_received[0] && string_received[0] <= '9')
    {
        char sep[]=" ";
        char *p=strtok(string_received,sep);
        strcpy(machine_ip,p);
        p=strtok(NULL,sep);
        strcpy(file,p);
    }
    else strcpy(file,string_received);

    char string_send[1000];
    bzero(string_send,1000);

    if(machine_ip[0]!=NULL) cautareFisierCuIP(machine_ip,file,string_send);
    else cautareFisierFaraIP(file,string_send);

    printf("[server]- thread %d - Trimitem raspunsul...\n",tdL.idThread);

    if (write (tdL.cl, string_send, strlen(string_send)) <= 0)
    {
        printf("[server]- thread %d - ",tdL.idThread);
        fflush(stdout);
        perror ("Eroare la write() catre client.\n");
        return errno;
    }
	else printf ("[server]- thread %d - Mesajul a fost trasmis cu succes.\n",tdL.idThread);

	if(strstr(string_send,"Fisierul cautat nu se gaseste in masina cu ip-ul dat!\n")!=0 || strcmp(string_send,"Fisierul cautat nu se gaseste in nici o masina!\n")==0)
    {
        bzero (string_received, 255);

        if (read (tdL.cl, string_received, 255) <= 0)
        {
            printf("[server]- thread %d - ",tdL.idThread);
            fflush(stdout);
            perror ("Eroare la read() de la client.\n");
            return errno;
        }

        if(string_received[0]=='d' && string_received[1]=='a')
        {
            strcpy(file,file+1);
            printf("[server]- thread %d - Fisier ce trebuie adaugat...%s\n",tdL.idThread, file);

            char path[255];
            bzero(path,255);

            if(string_received[3]!=NULL)
            {
                char sep[]=" ";
                char *p=strtok(string_received,sep);
                p=strtok(NULL,sep);
                strcpy(path,p);
            }
            else cautareFisierPrintreDirectoare("/home/cristian_adam",file,path);

            char statFile[1000];
            bzero(statFile,1000);

            determinareStatutFisier(path,statFile);

            sprintf(file,"%s%s",file,statFile);

            adaugareFisierInMasini(file);
        }

        bzero(string_send,255);
        strcpy(string_send,"Fisier adaugat!\n");

        if (write (tdL.cl, string_send, strlen(string_send)) <= 0)
        {
            printf("[server]- thread %d - ",tdL.idThread);
            fflush(stdout);
            perror ("Eroare la write() catre client.\n");
            return errno;
        }
        else printf ("[server]- thread %d - Mesajul a fost trasmis cu succes.\n",tdL.idThread);
    }

}

static void *treat(void * arg)
{
    struct thData tdL;
    tdL = *((struct thData*)arg);

    printf ("[server]- thread %d - Se asteapta cererea clientului ...\n", tdL.idThread);
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
    struct sockaddr_in server;
    struct sockaddr_in from;
    int sd;

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[server]Eroare la socket().\n");
        return errno;
    }

    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (PORT);


    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
        perror ("[server]Eroare la bind().\n");
        return errno;
    }

    int optval=1;
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));

    if (listen (sd, 5) == -1)
    {
        perror ("[server]Eroare la listen().\n");
        return errno;
    }

    pthread_t th[100];
	int i=0;
    while (1)
    {
        int client;
        int length = sizeof (from);
        thData * td;

        printf ("[server]Asteptam la portul %d...\n",PORT);
        fflush (stdout);

        client = accept (sd, (struct sockaddr *) &from, &length);

        if (client < 0)
        {
            perror ("[server]Eroare la accept().\n");
            continue;
        }

        td=(struct thData*)malloc(sizeof(struct thData));
        td->idThread=i++;
        td->cl=client;

        pthread_create(&th[i], NULL, &treat, td);

    }
}
