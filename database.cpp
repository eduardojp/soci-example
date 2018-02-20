#include "database.h"
#include <string>
#include <sstream>

#include <soci/soci.h>
#include <soci/oracle/soci-oracle.h>

using namespace soci;

Database::Database(string hostAddress, int port, string dbName, string user, string password) {
    this->hostAddress = hostAddress;
    this->port = port;
    this->dbName = dbName;
    this->user = user;
    this->password = password;
    
}

Database::~Database() {
}

void Database::loadBinaryTree(/*BinaryTree* binaryTree, */string hashingRefs, string videoTimeBeginString, string videoTimeEndString, int hashWindow) throw(runtime_error) {
    /*PGconn* conn = this->connect();
    PGresult* res;
    int i;
    
    if(!conn) {
        throw runtime_error("CONNECTION_FAILED");
    }*/
    session sql(oracle, "service=//192.168.100.10:1521/xe user=vhcservice password=vhcservice");

    string query = getVHCQuery(hashingRefs, videoTimeBeginString, videoTimeEndString, hashWindow);
    cout << "QUERY >> " << query << "\n";
    
    rowset<row> rset = (sql.prepare << query);
    try {
        for(rowset<row>::const_iterator it = rset.begin(); it != rset.end(); ++it) {
            row const& row = *it;

            
            cout << "-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-\n";

            uint32_t time = row.get<long long>(0);
            cout << "time: " << time << "\n";
            
            int64_t vhc =  row.get<long long>(1);
            cout << "vhc: " << vhc << "\n";
            
            string extRef = row.get<string>(2);
            cout << "extRef: " << extRef << "\n";
            
            uint64_t timeRef = row.get<long long>(3);
            cout << "timeRef: " << timeRef << "\n";
        }
    }
    catch(runtime_error& e) {
        printf("ERROR %s\n", e.what());
        fflush(stdout);
        exit(0);
    }
        
    //stmt->closeResultSet(rset);
    //conn->terminateStatement(stmt);

    // Close the connection to the database and cleanup
    //PQfinish(conn);
}

void Database::loadBinaryTree(/*BinaryTree* binaryTree, */string hashingRefs, string videoTimeBeginString, string videoTimeEndString, int hashWindow, uint8_t prefix) throw(runtime_error) {
    /*PGconn* conn = this->connect();
    PGresult* res;
    int i;
    
    if(!conn) {
        throw runtime_error("CONNECTION_FAILED");
    }
    
    //string query = getVHCQueryWithBounds(hashingRefs, videoTimeBeginString, videoTimeEndString, hashWindow, prefix);
    string query = getVHCQuery(hashingRefs, videoTimeBeginString, videoTimeEndString, hashWindow);
    cout << "QUERY >> " << query << "\n";

    stmt = conn->createStatement(query);
    ResultSet* rset = stmt->executeQuery();
    try {
        while(rset->next()) {
            uint32_t time = rset->getUInt(1);
            uint64_t vhc = (unsigned long) rset->getNumber(2).operator unsigned long();
            string extRef = rset->getString(3);
            uint64_t timeRef = (unsigned long) rset->getNumber(4).operator long();
            
            cout <<
                "time: " << time << "\n" <<
                "vhc: " << vhc << "\n" <<
                "extRef: " << extRef << "\n" <<
                "timeRef: " << timeRef << "\n\n";
        }
    }
    catch(SQLException ex) {
        cout << "Exception thrown for displayElements" << endl;
        cout << "Error number: " << ex.getErrorCode() << endl;
        //cout << ex.getMessage() << endl;
    }
        
    stmt->closeResultSet(rset);
    conn->terminateStatement(stmt);

    // Close the connection to the database and cleanup
    //PQfinish(conn);
    */
}

/*list<VHash*>**/ void Database::loadHashes(string hashingRefs, string begin, string end, int hashWindow) {
    /*PGconn* conn = this->connect();
    PGresult* res;
    int i;
    
    if(!conn) {
        return 0;
    }
    
    string query = getVHCQuery(hashingRefs, begin, end, hashWindow);
    cout << "QUERY >> " << query << "\n";

    //list<VHash*>* hashList = new list<VHash*>();
    stmt = conn->createStatement(query);
    ResultSet* rset = stmt->executeQuery();
    try {
        while(rset->next()) {
            uint32_t time = rset->getUInt(1);
            uint64_t vhc = (unsigned long) rset->getNumber(2);
            string extRef = rset->getString(3);
            uint64_t timeRef = (unsigned long) rset->getNumber(4);
            
            cout <<
                "time: " << time << "\n" <<
                "vhc: " << vhc << "\n" <<
                "extRef: " << extRef << "\n" <<
                "timeRef: " << timeRef << "\n\n";
            
            //hashList->push_back(new VHash(timeRef+time, vhc, extRef));
        }
    }
    catch(SQLException ex) {
        cout << "Exception thrown for displayElements" << endl;
        cout << "Error number: " << ex.getErrorCode() << endl;
        //cout << ex.getMessage() << endl;
    }
        
    stmt->closeResultSet(rset);
    conn->terminateStatement(stmt);
    
    return;
    //return hashList;
    */
}

void Database::split(string s, char delim, vector<string>* elems) {
    stringstream ss(s);
    string item;

    while(getline(ss, item, delim)) {
        elems->push_back(item);
    }
}
string Database::getVHCQuery(string hashingRefs, string beginString, string endString, int hashWindow) {
    stringstream query;
    query << "SELECT vsr_vhc.hash_time,vsr_vhc.vhc,vsr_hashing.ext_ref,vsr_hashing.time_ref FROM vsr_vhc,vsr_hashing WHERE vsr_vhc.hashing_id=vsr_hashing.id";
    
    vector<string> hashingRefVector;
    split(hashingRefs, ',', &hashingRefVector);
    
    //Filtro por nome de canal ou id de obra
    if(!hashingRefVector.empty()) {
        vector<string>::iterator it = hashingRefVector.begin();
        
        query << " AND (" << "vsr_hashing.ext_ref='" << (*it) << "'";
        it++;
        
        for( ; it != hashingRefVector.end(); it++) {
            query << " OR vsr_hashing.ext_ref='" << (*it) << "'";
        }
        
        query << ")";
    }

    //Filtro temporal (início)
    if(!beginString.empty()) {
        query << " AND vsr_hashing.time_ref+vsr_vhc.hash_time>='" << beginString << "'";
    }
    
    //Filtro temporal (fim)
    if(!endString.empty()) {
        query << " AND vsr_hashing.time_ref+vsr_vhc.hash_time+" << hashWindow << "<='" << endString << "'";
    }
    
    //FIXME
    query << " ORDER BY vsr_vhc.vhc";
    
    return query.str();
}