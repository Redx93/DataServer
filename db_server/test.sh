#!/bin/sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:lib
./db_server "CREATE TABLE students (id INT, first_name VARCHAR(8), last_name VARCHAR(8));"
./db_server ".tables"
./db_server ".schema students"
./db_server "DROP TABLE students;"
./db_server "INSERT INTO students VALUES (42, 'Oscar', 'Svensson');"
./db_server "SELECT * FROM students;"
./db_server "CREATE TABLE students (id INT, first_name VARCHAR(7), last_name VARCHAR(8), PRIMARY KEY(id));"
./db_server "SELECT first_name, last_name FROM students;"
./db_server "DELETE FROM students WHERE id=42;"
./db_server "UPDATE students SET first_name='Emil', last_name='Johansson' WHERE id=42;"
