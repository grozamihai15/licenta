#include <iostream>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <mysql.h>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <thread>

using namespace std;

MYSQL* conn;

struct structVag
{
    int clasa;
    int tip;
} mVagon[10];

int mPrio[10][20][10]; // [nr vagon][compartiment ][ loc]
char mStatus[10][20][10][10]; // [nr vagon][compartiment ][ loc] [litere]
char tipCompartiment[10][20][10];
char locBilet[100];

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
        cout << "Conexiunea la baza de date s-a realizat cu succes!" << endl;
    }
    else
    {
        cout<< "Nu s-a putut conecta la baza de date!";
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
void preiaBilete(char *id,char *buffer)
{
    char comanda[1000]="SELECT * FROM trenuri.";
    strcat(comanda,id);
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
            strcat(buffer,"_");
        }
        strcat(buffer,"\n");

    }

    mysql_free_result(result);
}
void procesareBilete(char *buffer)
{
    memset(mPrio, 0, sizeof(mPrio));
    memset(mVagon, 0, sizeof(mVagon));
    memset(mStatus, 0, sizeof(mStatus));
    char status[20];
    char* token = strtok(buffer, "_\n");
    while (token != nullptr)
    {
        int nrvag = atoi(token);
        token = strtok(NULL, "_\n");

        int tipvag = atoi(token);
        token = strtok(NULL,"_\n");

        int clasavag = atoi(token);
        token = strtok(NULL,"_\n");

        int nrcompartiment = atoi(token);
        token = strtok(NULL,"_\n");

        int loc = atoi(token);
        token = strtok(NULL,"_\n");

        int prioritatea = atoi(token);
        token = strtok(NULL,"_\n");

        memset(status, 0, sizeof(status));
        strcat(status,token);

        token = strtok(NULL,"_\n");

        mPrio[nrvag][nrcompartiment][loc]=prioritatea;
        strcat(mStatus[nrvag][nrcompartiment][loc],status);
        mVagon[nrvag].clasa=clasavag;
        mVagon[nrvag].tip=tipvag;
    }
}
int impartireBilete(int bn,int bs,int bp,int cb,char *buffer)
{
    int tipB;
    string b;
    if(bn>0)
        tipB=1,b="OCUPAT";
    else if(bs>0)
        tipB=2,b="STUDENT";
    else if(bp>0)
        tipB=3,b="PENSIONAR";
    for(int nrvagon=1; mVagon[nrvagon].clasa!='\0' ; nrvagon++)
    {
        if(mVagon[nrvagon].clasa == cb)
        {
            if(mVagon[nrvagon].tip==1)
            {
                for(int nrcompartiment=1; nrcompartiment <=15 ; nrcompartiment++)
                {
                    int liber=0,ocupat=0,student=0,pensionar=0;
                    if(nrcompartiment%2==1)
                    {
                        if(strcmp(mStatus[nrvagon][nrcompartiment][1],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][1],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][1],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][1],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][3],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][4],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][5],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][6],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        int tipC=0;
                        if(ocupat>0)
                            tipC=1;
                        else if(student>0)
                            tipC=2;
                        else if(pensionar>0)
                            tipC=3;

                        if(liber>=3 and (tipC==tipB or tipC==0))
                        {
                            if(strcmp(mStatus[nrvagon][nrcompartiment][5],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_5";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][1],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_1";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_6";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_4";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_3";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                        }
                    }
                    else
                    {
                        if(strcmp(mStatus[nrvagon][nrcompartiment][2],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][2],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][2],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][2],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][3],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][4],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][5],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][6],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        int tipC=0;
                        if(ocupat>0)
                            tipC=1;
                        else if(student>0)
                            tipC=2;
                        else if(pensionar>0)
                            tipC=3;

                        if(liber>=3 and (tipC==tipB or tipC==0))
                        {
                            if(strcmp(mStatus[nrvagon][nrcompartiment][6],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_6";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][2],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_2";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_3";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_5";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_4";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }

    for(int nrvagon=1; mVagon[nrvagon].clasa!='\0' ; nrvagon++)
    {
        if(mVagon[nrvagon].clasa == cb)
        {
            if(mVagon[nrvagon].tip==1)
            {
                for(int nrcompartiment=1; nrcompartiment <=15 ; nrcompartiment++)
                {
                    int liber=0,ocupat=0,student=0,pensionar=0;
                    if(nrcompartiment%2==1)
                    {
                        if(strcmp(mStatus[nrvagon][nrcompartiment][1],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][1],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][1],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][1],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][3],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][4],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][5],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][6],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        int tipC=0;
                        if(ocupat>0)
                            tipC=1;
                        else if(student>0)
                            tipC=2;
                        else if(pensionar>0)
                            tipC=3;

                        if(tipC==tipB or tipC==0)
                        {
                            if(strcmp(mStatus[nrvagon][nrcompartiment][5],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_5";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][1],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_1";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_6";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_4";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_3";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                        }
                    }
                    else
                    {
                        if(strcmp(mStatus[nrvagon][nrcompartiment][2],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][2],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][2],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][2],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][3],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][4],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][5],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][6],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        int tipC=0;
                        if(ocupat>0)
                            tipC=1;
                        else if(student>0)
                            tipC=2;
                        else if(pensionar>0)
                            tipC=3;

                        if(tipC==tipB or tipC==0)
                        {
                            if(strcmp(mStatus[nrvagon][nrcompartiment][6],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_6";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][2],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_2";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_3";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_5";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                            else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"LIBER")==0)
                            {
                                string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_4";
                                string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                                if (mysql_query(conn,comanda.c_str()))
                                {
                                    finish_with_error(conn);
                                }
                                strcpy(buffer,idloc.c_str());
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }

    for(int nrvagon=1; mVagon[nrvagon].clasa!='\0' ; nrvagon++)
    {
        if(mVagon[nrvagon].clasa == cb)
        {
            if(mVagon[nrvagon].tip==1)
            {
                for(int nrcompartiment=1; nrcompartiment <=15 ; nrcompartiment++)
                {
                    int liber=0,ocupat=0,student=0,pensionar=0;
                    if(nrcompartiment%2==1)
                    {
                        if(strcmp(mStatus[nrvagon][nrcompartiment][1],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][1],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][1],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][1],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][3],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][4],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][5],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][6],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        int tipC=0;
                        if(ocupat>0)
                            tipC=1;
                        else if(student>0)
                            tipC=2;
                        else if(pensionar>0)
                            tipC=3;

                        if(strcmp(mStatus[nrvagon][nrcompartiment][5],"LIBER")==0)
                        {
                            string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_5";
                            string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                            if (mysql_query(conn,comanda.c_str()))
                            {
                                finish_with_error(conn);
                            }
                            strcpy(buffer,idloc.c_str());
                            return 1;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][1],"LIBER")==0)
                        {
                            string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_1";
                            string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                            if (mysql_query(conn,comanda.c_str()))
                            {
                                finish_with_error(conn);
                            }
                            strcpy(buffer,idloc.c_str());
                            return 1;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"LIBER")==0)
                        {
                            string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_6";
                            string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                            if (mysql_query(conn,comanda.c_str()))
                            {
                                finish_with_error(conn);
                            }
                            strcpy(buffer,idloc.c_str());
                            return 1;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"LIBER")==0)
                        {
                            string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_4";
                            string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                            if (mysql_query(conn,comanda.c_str()))
                            {
                                finish_with_error(conn);
                            }
                            strcpy(buffer,idloc.c_str());
                            return 1;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"LIBER")==0)
                        {
                            string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_3";
                            string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                            if (mysql_query(conn,comanda.c_str()))
                            {
                                finish_with_error(conn);
                            }
                            strcpy(buffer,idloc.c_str());
                            return 1;
                        }
                    }
                    else
                    {
                        if(strcmp(mStatus[nrvagon][nrcompartiment][2],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][2],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][2],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][2],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][3],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][4],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][5],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        if(strcmp(mStatus[nrvagon][nrcompartiment][6],"LIBER")==0)
                        {
                            liber++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"OCUPAT")==0)
                        {
                            ocupat++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"STUDENT")==0)
                        {
                            student++;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][6],"PENSIONAR")==0)
                        {
                            pensionar++;
                        }

                        int tipC=0;
                        if(ocupat>0)
                            tipC=1;
                        else if(student>0)
                            tipC=2;
                        else if(pensionar>0)
                            tipC=3;

                        if(strcmp(mStatus[nrvagon][nrcompartiment][6],"LIBER")==0)
                        {
                            string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_6";
                            string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                            if (mysql_query(conn,comanda.c_str()))
                            {
                                finish_with_error(conn);
                            }
                            strcpy(buffer,idloc.c_str());
                            return 1;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][2],"LIBER")==0)
                        {
                            string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_2";
                            string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                            if (mysql_query(conn,comanda.c_str()))
                            {
                                finish_with_error(conn);
                            }
                            strcpy(buffer,idloc.c_str());
                            return 1;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][3],"LIBER")==0)
                        {
                            string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_3";
                            string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                            if (mysql_query(conn,comanda.c_str()))
                            {
                                finish_with_error(conn);
                            }
                            strcpy(buffer,idloc.c_str());
                            return 1;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][5],"LIBER")==0)
                        {
                            string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_5";
                            string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                            if (mysql_query(conn,comanda.c_str()))
                            {
                                finish_with_error(conn);
                            }
                            strcpy(buffer,idloc.c_str());
                            return 1;
                        }
                        else if(strcmp(mStatus[nrvagon][nrcompartiment][4],"LIBER")==0)
                        {
                            string idloc = to_string(nrvagon) + "_" + to_string(mVagon[nrvagon].tip) + "_" + to_string(mVagon[nrvagon].clasa) + "_" + to_string(nrcompartiment) + "_4";
                            string comanda = "UPDATE ir11 SET status = '" + b + "' WHERE id_loc = '"  + idloc + "';" ;
                            if (mysql_query(conn,comanda.c_str()))
                            {
                                finish_with_error(conn);
                            }
                            strcpy(buffer,idloc.c_str());
                            return 1;
                        }

                    }
                }
            }
        }
    }

    strcpy(buffer,"NU MAI SUNT LOCURI!");
    return 0;
}







void clientHandler(SOCKET clientSocket)
{
    char buffer[5024];
    memset(buffer, 0, sizeof(buffer));

    // Receive message from client
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == SOCKET_ERROR)
    {
        std::cerr << "Eroare la primirea mesajului de la client!" << std::endl;
        closesocket(clientSocket);
        return;
    }

        std::cout << "Mesaj primit de la client: " << buffer << std::endl;

        bazadedate();
        if(!strcmp(buffer,"cereRute"))
        {
            memset(buffer, 0, sizeof(buffer));

            get_DB_id_nume(*buffer);
            int lastPosition = 0;
            while (buffer[lastPosition] != '\0')
            {
                lastPosition++;
            }


            buffer[lastPosition-1]='\0';
        }
        else if(buffer[0]=='d' and buffer[1]=='e' and buffer[2]=='t')
        {

            char idTren[20];
            int poz=0;
            for(int i=7; i<sizeof(buffer); i++)
            {
                idTren[poz]=buffer[i];
                poz++;
            }

            memset(buffer, 0, sizeof(buffer));
            get_DB_detalii(buffer,idTren);

        }
        else
        {
            int nrv=0,poz=0,i=0;
            char id[10],bil[100],clasaBilet[10];
            while(buffer[i]!='\0')
            {

                if(buffer[i]==',')
                    nrv++,poz=0;
                else if(nrv==0)
                {
                    id[poz]=tolower(buffer[i]);
                    poz++;
                }
                else if(nrv==1)
                {
                    bil[poz]=buffer[i];
                    poz++;
                }
                else if(nrv==2)
                {
                    clasaBilet[poz]=buffer[i];
                    poz++;
                }
                i++;
            }
            int bn,bs,bp;
            if(strcmp(bil,"Bilet normal")==0)
            {
                bn=1;
            }
            else if(strcmp(bil,"Bilet student")==0)
            {
                bs=1;
            }
            else if(strcmp(bil,"Bilet pensionar")==0)
            {
                bp=1;
            }

            int cb=clasaBilet[6]-'0';
            memset(buffer, 0, sizeof(buffer));
            preiaBilete(id,buffer);
            procesareBilete(buffer);
            int rezultat;
            rezultat=impartireBilete(bn,bs,bp,cb,buffer);

        }

        inchideBD();




    // Prepare the response
    std::string response = buffer;

    // Send response back to the client
    if (send(clientSocket, response.c_str(), response.length(), 0) == SOCKET_ERROR)
    {
        std::cerr << "Error sending response to client." << std::endl;
    }
    else
    {
        cout<<"Mesajul trimis catre client:"<<endl<<buffer<<endl<<endl;
    }

    // Close the client socket
    closesocket(clientSocket);
}

int main()
{
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    SOCKADDR_IN serverAddress, clientAddress;
    int clientAddressSize = sizeof(clientAddress);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Error initializing Winsock." << std::endl;
        return -1;
    }

     // Create server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        std::cerr << "Error creating server socket." << std::endl;
        WSACleanup();
        return -1;
    }

    // Setarea informațiilor despre server
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8888);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

     // Bind server address to the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Error binding server address to the socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    // Start listening for client connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Error listening for client connections." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

        std::cout << "Serverul este pornit si asteapta conexiuni de la clienti..." << std::endl;

        while (true)
    {
        // Accept a new client connection
        if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize)) == INVALID_SOCKET)
        {
            std::cerr << "Error accepting client connection." << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return -1;
        }

        std::cout << "Client conectat." << std::endl;

        // Create a new thread to handle the client
        std::thread clientThread(clientHandler, clientSocket);
        clientThread.detach();  // Detach the thread to run independently
    }

    // Close the server socket and cleanup Winsock
    closesocket(serverSocket);
    WSACleanup();

    return 0;


}
