#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>


int main(void) {
    
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open("shopper.db", &db);
    int rcImbracaminte = sqlite3_open("shopper.db", &db);
    int rcIncaltaminte = sqlite3_open("shopper.db", &db);
    
    if (rc != SQLITE_OK || rcImbracaminte != SQLITE_OK || rcIncaltaminte != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    
    char *sql = "DROP TABLE IF EXISTS Useri;" 
                "CREATE TABLE Useri(NUME TEXT, PAROLA TEXT);" 
                "INSERT INTO Useri VALUES('andreea', 'andreea');" 
                "INSERT INTO Useri VALUES('vali', 'bala');" 
                "INSERT INTO Useri VALUES('cristi', 'cristi');" 
                "INSERT INTO Useri VALUES('sanda', 'sanda');" ;

    char *sqlImbracaminte = "DROP TABLE IF EXISTS Imbracaminte;" 
                "CREATE TABLE Imbracaminte(IMBRACAMINTE TEXT, PRET TEXT);" 
                "INSERT INTO Imbracaminte VALUES('hanorac', '80');" 
                "INSERT INTO Imbracaminte VALUES('pantaloni','90');" 
                "INSERT INTO Imbracaminte VALUES('rochie', '500');" 
                "INSERT INTO Imbracaminte VALUES('geaca', '100');" ;           

    char *sqlIncaltaminte = "DROP TABLE IF EXISTS Incaltaminte;" 
                "CREATE TABLE Incaltaminte(INCALTAMINTE TEXT, PRET TEXT);" 
                "INSERT INTO Incaltaminte VALUES('sandale', '80');" 
                "INSERT INTO Incaltaminte VALUES('adidasi','90');" 
                "INSERT INTO Incaltaminte VALUES('tenisi', '500');" 
                "INSERT INTO Incaltaminte VALUES('bocanci', '100');" ;              

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    rcImbracaminte = sqlite3_exec(db, sqlImbracaminte, 0, 0, &err_msg);
    rcIncaltaminte = sqlite3_exec(db, sqlIncaltaminte, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK  || rcImbracaminte != SQLITE_OK) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return 1;
    } 
    
    sqlite3_close(db);
    
    return 0;
}