#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>


int main(void) {
    
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open("shopper.db", &db);
    int rcImbracaminte = sqlite3_open("shopper.db", &db);
    int rcIncaltaminte = sqlite3_open("shopper.db", &db);
    int rcMancare = sqlite3_open("shopper.db", &db);
    
    if (rc != SQLITE_OK 
        || rcImbracaminte != SQLITE_OK 
        || rcIncaltaminte != SQLITE_OK 
        || rcMancare != SQLITE_OK ) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    
    char *sql = "DROP TABLE IF EXISTS Useri;" 
                "CREATE TABLE Useri(NUME TEXT, PAROLA TEXT);" 
                "INSERT INTO Useri VALUES('andreea', 'andreea');" 
                "INSERT INTO Useri VALUES('paula', 'paula');" 
                "INSERT INTO Useri VALUES('cristi', 'cristi');" 
                "INSERT INTO Useri VALUES('sanda', 'sanda');" ;

    char *sqlImbracaminte = "DROP TABLE IF EXISTS Imbracaminte;" 
                "CREATE TABLE Imbracaminte(IMBRACAMINTE TEXT, PRET TEXT);" 
                "INSERT INTO Imbracaminte VALUES('Hanorac', '80');" 
                "INSERT INTO Imbracaminte VALUES('Pantaloni','90');" 
                "INSERT INTO Imbracaminte VALUES('Rochie', '500');" 
                "INSERT INTO Imbracaminte VALUES('Geaca', '100');" ;           

    char *sqlIncaltaminte = "DROP TABLE IF EXISTS Incaltaminte;" 
                "CREATE TABLE Incaltaminte(INCALTAMINTE TEXT, PRET TEXT);" 
                "INSERT INTO Incaltaminte VALUES('Sandale', '80');" 
                "INSERT INTO Incaltaminte VALUES('Adidasi','90');" 
                "INSERT INTO Incaltaminte VALUES('Tenisi', '500');" 
                "INSERT INTO Incaltaminte VALUES('Bocanci', '100');" ;    

    
    char *sqlMancare = "DROP TABLE IF EXISTS Mancare;" 
                "CREATE TABLE Mancare(MANCARE TEXT, PRET TEXT);" 
                "INSERT INTO Mancare VALUES('Banane', '40');" 
                "INSERT INTO Mancare VALUES('Pizza','20');" 
                "INSERT INTO Mancare VALUES('Salata', '15');" 
                "INSERT INTO Mancare VALUES('Mere', '20');" ;                     

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    rcImbracaminte = sqlite3_exec(db, sqlImbracaminte, 0, 0, &err_msg);
    rcIncaltaminte = sqlite3_exec(db, sqlIncaltaminte, 0, 0, &err_msg);
    rcMancare = sqlite3_exec(db, sqlMancare, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK
        || rcImbracaminte != SQLITE_OK 
        || rcIncaltaminte != SQLITE_OK 
        || rcImbracaminte != SQLITE_OK ){
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return 1;
    } 
    
    sqlite3_close(db);
    
    return 0;
}