#include <iostream>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <mysql.h>

using namespace std;


void finish_with_error(MYSQL* conn)
{
    cout<<"ERROR";
    mysql_close(conn);
    exit(1);
}
void bazadedate()
{
    MYSQL* conn;
    conn = mysql_init(NULL);
    if (mysql_real_connect(conn,"localhost","root","parola@licenta123","trenuri",0,NULL,0) !=0)
    {
        cout << "Succesfully  Connected to MySQL database" << endl;
    }
    else
    {
        cout<< "not connect";
    }
    if (mysql_query(conn, "SELECT * FROM id_trenuri"))
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
            cout<< row[i]<<" ";
        }
        cout<<endl;

    }

    mysql_free_result(result);
    mysql_close(conn);

}
int main()
{
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    SOCKADDR_IN serverAddress, clientAddress;
    int clientAddressSize = sizeof(clientAddress);
    char buffer[1024];

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

    std::cout << "Serverul este pornit și așteaptă conexiuni de la clienți..." << std::endl;

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


//===============================================================================================
    // Trimite un răspuns la client
    std::string response = "Mesaj primit de la server.";
    if (send(clientSocket, response.c_str(), response.length(), 0) == SOCKET_ERROR)
    {
        std::cerr << "Eroare la trimiterea răspunsului la client." << std::endl;
        return -1;
    }

    // Închiderea socketului și Winsock
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();


    return 0;
}
