#pragma once
//
// @(#) 2019-09-28 (c) 2019 W.Kager
//
// This module describes the class as been found below;
// Headerfile and class name normally is similar
//
// This class is used to deal an access to a MariaDB Database. So that several different modules of one process can use a connection to a
// database and don't need to deal with database connection etc. 
//

class KwoDbAccess
{
protected:
  sql::Driver* m_SqlDriver;             // Nomen est omen: Driver to mySql/MariaDB database
  sql::Connection* m_SqlCon;            // Nomen est omen: Connection to mySql/Maria database
  sql::Statement* m_SqlStmt;            // Statement
  sql::ResultSet* m_SqlRes;             // Result of a call to mySql/Maria database
  std::string     m_Schema;             // should contain the name of the used database schema

  long long   m_InsertCntr;             // static counter which counts the number of database "inserts" down

  static KwoDbAccess * m_MySelf;        // Pointer to the one and only Instance
  KwoDbAccess();                        // nomen est omen: constructor

public:
  static KwoDbAccess *getInstance();                                   // An instance of this class should only exist once per process
                                                                       // This is the accessor of the one and only instance

  ~KwoDbAccess();                                                      // nomen est omen: destructor

  sql::Connection  *init(const char* _schema);                         // preset the database schema
  inline const long long& IncInsertCnt() { return ++m_InsertCntr; };   // increments the insert counter
  inline const long long& InsertCnt() { return m_InsertCntr; };        // reads the insert counter
};

