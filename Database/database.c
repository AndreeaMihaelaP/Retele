#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>


int main(void) {
    
    sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open("shopper.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    
    char *sql = "DROP TABLE IF EXISTS Useri;" 
                "CREATE TABLE Useri(Id INT, NUME TEXT, PAROLA TEXT);" 
                "INSERT INTO Useri VALUES(1, 'andreea', 'andreea');" 
                "INSERT INTO Useri VALUES(2, 'vali', 'bala');" 
                "INSERT INTO Useri VALUES(3, 'cristi', 'cristi');" 
                "INSERT INTO Useri VALUES(4, 'sanda', 'sanda');" ;

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return 1;
    } 
    
    sqlite3_close(db);
    
    return 0;
}