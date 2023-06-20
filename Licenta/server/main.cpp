#include <iostream>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <mysql.h>
#include <cstring>
#include <string>
#include <cctype>

using namespace std;

MYSQL* conn;
void finish_with_error(MYSQL* conn)
{
    cout<<"ERROR";
    mysql_close(conn);
    exit(1);
}
void inchideBD()
{
    mysql_close(conn);
}
void bazadedate()
{
    conn = mysql_init(NULL);
    if (mysql_real_connect(conn,"localhost","root","parola@licenta123","trenuri",0,NULL,0) !=0)
    {
        cout << "Succesfully  Connected to MySQL database" << endl;
    }
    else
    {
        cout<< "not connect";
    }
}
void get_DB_id_nume(char &buffer)
{
    if (mysql_query(conn, "SELECT id_tren,nume_tren FROM rute_trenuri"))
    {
        finish_with_error(conn);
    }
    MYSQL_RES *result = mysql_store_result(conn);

    if (result == NULL)
    {
        finish_with_error(conn);
    }

    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)))
    {
        for(int i = 0; i < num_fields; i++)
        {
            strcat(&buffer, row[i]);
        }
        strcat(&buffer, "\n");

    }

    mysql_free_result(result);
}
void get_DB_detalii(char *buffer,char *idTren)
{
    char comanda[1000]="SELECT ora_plecare,ora_destinatie,pret_bilet,nr_vagoane,vagon_clasa1,vagon_clasa2 FROM rute_trenuri WHERE id_tren ='";
    strcat(comanda,idTren);
    strcat(comanda,"'");
    if (mysql_query(conn,comanda))
    {
        finish_with_error(conn);
    }

    MYSQL_RES *result = mysql_store_result(conn);

    if (result == NULL)
    {
        finish_with_error(conn);
    }

    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)))
    {
        for(int i = 0; i < num_fields; i++)
        {
            strcat(buffer, row[i]);
        }
        strcat(buffer,"\n");

    }

    mysql_free_result(result);
}
int main()
{
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    SOCKADDR_IN serverAddress, clientAddress;
    int clientAddressSize = sizeof(clientAddress);
    char buffer[5024];
    memset(buffer, 0, sizeof(buffer));

    while(1)
    {
        // Initializarea Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cerr << "Eroare la initializarea Winsock." << std::endl;
            return -1;
        }

        // Crearea socketului server
        if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        {
            std::cerr << "Eroare la crearea socketului." << std::endl;
            return -1;
        }

        // Setarea informațiilor despre server
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(8888);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        // Asocierea adresei serverului cu socketul
        if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
        {
            std::cerr << "Eroare la asocierea adresei serverului cu socketul." << std::endl;
            return -1;
        }

        // Ascultarea conexiunilor de la clienți
        if (listen(serverSocket, 1) == SOCKET_ERROR)
        {
            std::cerr << "Eroare la ascultarea conexiunilor de la clienți." << std::endl;
            return -1;
        }

        std::cout << "Serverul este pornit si asteapta conexiuni de la clienti..." << std::endl;

        // Așteptarea unei conexiuni de la un client
        if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize)) == INVALID_SOCKET)
        {
            std::cerr << "Eroare la acceptarea conexiunii de la client." << std::endl;
            return -1;
        }

        std::cout << "Un client s-a conectat." << std::endl;

        // Primirea mesajului de la client
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == SOCKET_ERROR)
        {
            std::cerr << "Eroare la primirea mesajului de la client." << std::endl;
            return -1;
        }
//===============================================================================================
        bazadedate();
        std::cout << "Mesaj primit de la client: " << buffer << std::endl;
        if(!strcmp(buffer,"cereRute"))
        {
            memset(buffer, 0, sizeof(buffer));

            get_DB_id_nume(*buffer);
            int lastPosition = 0;
            while (buffer[lastPosition] != '\0')
            {
                lastPosition++;
            }

            cout<<lastPosition<<endl;
            buffer[lastPosition-1]='\0';
        }
        else if(buffer[0]=='d' and buffer[1]=='e' and buffer[2]=='t')
        {
            int i=7,poz=0;
            char idTren[20];
            bool check1=0,check2=0;
            while(1)
            {
                if(isalpha(buffer[i])!=0 and check1==0)
                    check1=1;
                else if(isalpha(buffer[i])!=0 and check1==1 and check2==1)
                        break;
                else if(isdigit(buffer[i])!=0)
                    check2=1;
                idTren[poz]=buffer[i];
                poz++;
                i++;
            }

            idTren[poz]='\0';
            cout<<buffer<<" "<<idTren<<endl;
            memset(buffer, 0, sizeof(buffer));
            get_DB_detalii(buffer,idTren);
            cout<<buffer<<endl;
        }



        inchideBD();

//===============================================================================================
        // Trimite un răspuns la client
        std::string response = buffer;
        if (send(clientSocket, response.c_str(), response.length(), 0) == SOCKET_ERROR)
        {
            std::cerr << "Eroare la trimiterea răspunsului la client." << std::endl;
            return -1;
        }

        // Închiderea socketului și Winsock
        memset(buffer, 0, sizeof(buffer));
        closesocket(clientSocket);
        closesocket(serverSocket);
        WSACleanup();

    }
    return 0;
}
