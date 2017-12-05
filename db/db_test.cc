//
//  main.cpp
//  leveldb
//
//  Created by dencesun on 01/12/2017.
//  Copyright © 2017 Sun. All rights reserved.
//

#include <iostream>
#include "leveldb/db.h"

using namespace std;
using namespace leveldb;

int main() {
    DB *db ;
    Options op;
    op.create_if_missing = true;
    Status s = DB::Open(op,"/tmp/testdb",&db);
    
    if(s.ok()){
        cout << "create table success..." << endl;
        s = db->Put(WriteOptions(),"abcd","1234");
        if(s.ok()){
            cout << "insert data success..." << endl;
            string value;
            s = db->Get(ReadOptions(),"abcd",&value);
            if(s.ok()){
                cout << "get data success value:" << value << endl;
            }
            else{
                cout << "get data failure..." << endl;
            }
        }
        else{
            cout << "insert data failure..." << endl;
        }
    }
    else{
        cout << "create database failure..." << endl;
    }
    delete db;
    return 0;
}
