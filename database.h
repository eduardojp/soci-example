#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <iostream>

#include <vector>

//using namespace oracle::occi;
using namespace std;

class Database {
    private:
        string hostAddress;
        int port;
        string dbName;
        string user;
        string password;

    public:
        Database(string hostAddress, int port, string dbName, string user, string password);
        ~Database();

        void insertBind(int c1, string c2);
        void insertRow();
        void updateRow(int c1, string c2);
        void deleteRow(int c1, string c2);
        
        //Connection* connect();
        void loadBinaryTree(/*BinaryTree* binaryTree, */string hashingRefs, string videoTimeBeginString, string videoTimeEndString, int hashWindow) throw(runtime_error);
        void loadBinaryTree(/*BinaryTree* binaryTree, */string hashingRefs, string videoTimeBeginString, string videoTimeEndString, int hashWindow, uint8_t prefix) throw(runtime_error);
        void loadHashes(string hashingRefs, string begin, string end, int hashWindow);
        void split(string s, char delim, vector<string>* elems);
        string getVHCQuery(string hashingRefs, string beginString, string endString, int hashWindow);

};

#endif