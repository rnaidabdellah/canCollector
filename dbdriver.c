//-----------------------------------------------------------------------------
/*!
   \internal
   \file          service.c
   \brief         AGV
   \implementation
   copyright   LeclanchÃ©
   license
   created     07.01.2019 Rachid Naid-Abdellah
   \endimplementation
   \internal
   \history
   Date(dd.mm.yyyy)  Author                  Description
   07.01.2019        Rachid Naid-Abdellah    file created
   \endhistory
*/
//-----------------------------------------------------------------------------/*

#include <sqlite3.h>
#include <stdio.h>

#include "types.h"
#include "appConfig.h"

char sql[1024];

void createBMSTable()
{
    int iCounter = 0;
    int i;

    //
    iCounter += snprintf(sql+iCounter,1024,"%s","DROP TABLE IF EXISTS Cars;");

    //Create Header
    iCounter += snprintf(sql+iCounter,1024,"%s","CREATE TABLE Cars(");


    //for(i=0;i<maxDefVars;i++)
    {
        iCounter += snprintf(sql+iCounter,1024,"%s TEXT,","Tag");
        iCounter += snprintf(sql+iCounter,1024,"%s INT,","Value");
        iCounter += snprintf(sql+iCounter,1024,"%s TEXT,","Unit");
        iCounter += snprintf(sql+iCounter,1024,"%s INT,","Timeout");
    }
    iCounter--;
    iCounter += snprintf(sql+iCounter,1024,"%s",");");

    //Insert values for testing
    //iCounter += snprintf(sql+iCounter,1024,"%s","INSERT INTO Cars VALUES(1, 'Volvo', 2015, 1977);");

    printf("\nheader:%d: %s\n",iCounter,sql);

}

int updatedbAllRow()
{
    int iCounter = 0;
    static int ivalue=0;
    sqlite3 *db;
    char *err_msg = 0;
    static iSelectRow = 0;

    int rc = sqlite3_open("/home/m4iot001/ccode/dbsqlite/test.db", &db);
    if (rc != SQLITE_OK) {

        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }


    iCounter += snprintf(sql+iCounter,1024,"UPDATE cars SET Value=%ld, Timeout=%d WHERE Tag='%s';",ALL_VAR_ARRAY[iSelectRow].value,ALL_VAR_ARRAY[iSelectRow].isFresh,ALL_VAR_ARRAY[iSelectRow].tag);
    //iCounter += snprintf(sql+iCounter,1024,"UPDATE cars SET Value=%ld, Timeout=%ld WHERE Tag='%s';",ALL_VAR_ARRAY[iSelectRow].value,ALL_VAR_ARRAY[iSelectRow].counter,ALL_VAR_ARRAY[iSelectRow].tag);
    printf("\nupdate index:%d: %s\n",iSelectRow,sql);

    /*ivalue++;
    ALL_VAR_ARRAY[ivar].value=ivalue;*/

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    iSelectRow++;
    iSelectRow=iSelectRow%maxDefVars;

    //Close
    sqlite3_close(db);

    return 0;

}

/*int updatedbEntry_Value(int ivar)
{
    int iCounter = 0;
    static int ivalue=0;
    sqlite3 *db;
    char *err_msg = 0;
    //char sql[1024];

    int rc = sqlite3_open("/home/m4iot001/ccode/dbsqlite/test.db", &db);
    if (rc != SQLITE_OK) {

        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }


    iCounter += snprintf(sql+iCounter,1024,"UPDATE Cars SET Value=%ld WHERE Tag='%s';",ALL_VAR_ARRAY[ivar].value,ALL_VAR_ARRAY[ivar].tag);
    printf("\nupdate:%d: %s\n",iCounter,sql);


    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    //Close
    sqlite3_close(db);

    return 0;

}*/

void insertBMSEntry()
{
    int iCounter = 0;
    int i=0;

    //
    //iCounter += snprintf(sql+iCounter,1024,"%s","INSERT INTO Cars VALUES('HB1', 10, 'None', 0);");

    for(i=0;i<maxDefVars;i++)
    {
        iCounter += snprintf(sql+iCounter,1024,"INSERT INTO Cars VALUES(");
        iCounter += snprintf(sql+iCounter,1024,"'%s',",ALL_VAR_ARRAY[i].tag);
        iCounter += snprintf(sql+iCounter,1024,"'%ld',",ALL_VAR_ARRAY[i].value);
        iCounter += snprintf(sql+iCounter,1024,"'%s',",ALL_VAR_ARRAY[i].unit);
        iCounter += snprintf(sql+iCounter,1024,"'%u',",ALL_VAR_ARRAY[i].isFresh);
        iCounter--;
        iCounter += snprintf(sql+iCounter,1024,"%s",");");
    }

    //printf("\sql:%d: %s\n",iCounter,sql);
}

int createBMSdynamically(void) {

    sqlite3 *db;
    char *err_msg = 0;
    //char sql[1024];

    int rc = sqlite3_open("/home/m4iot001/ccode/dbsqlite/test.db", &db);

    if (rc != SQLITE_OK) {

        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }


    createBMSTable();
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    insertBMSEntry();
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }


    //Close
    sqlite3_close(db);

    return 0;
}


