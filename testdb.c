//gcc testdb.c -o testdb  `mysql_config --cflags --libs`
//./testdb

#include <my_global.h>
#include <mysql.h>

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);        
}

int main(int argc, char **argv)
{
  MYSQL *con = mysql_init(NULL);
  
if (con == NULL) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "root", "7855", 
          NULL, 0, NULL, 0) == NULL) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
  }  
   if (mysql_query(con, "DROP DATABASE testdb")) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
  }

  if (mysql_query(con, "CREATE DATABASE testdb")) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
  }
    if (mysql_query(con, "USE testdb")) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
  }
  
  if (mysql_query(con, "DROP TABLE IF EXISTS ClientS")) {
      finish_with_error(con);
  }
  
  if (mysql_query(con, "CREATE TABLE ClientS( Name TEXT, Saldo INT)")) {      
      finish_with_error(con);
  }
  
  if (mysql_query(con, "INSERT INTO ClientS VALUES('ClientC',2000)")) {
      finish_with_error(con);
  }

   if (mysql_query(con, "SELECT * FROM ClientS")) {
      finish_with_error(con);
  }

MYSQL_RES *result = mysql_store_result(con);
  
  if (result == NULL) 
  {
      finish_with_error(con);
  }

  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;
  
  while ((row = mysql_fetch_row(result))) 
  { 
      for(int i = 0; i < num_fields; i++) 
      { 
          printf("%s ", row[i] ? row[i] : "NULL"); 
      } 
          printf("\n"); 
          int saldo = atoi(row[1]);
          printf(" el saldo de su cuenta es : %d ",saldo);
          
  }
  
  mysql_free_result(result);
  mysql_close(con);
  
  exit(0);
}
