#include <soci/soci.h>
#include <soci/oracle/soci-oracle.h>

#include <exception>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>

#include "database.h"

using namespace std;
//using namespace soci;

bool getName(string &name) {
    cout << "Enter name: ";
    return !!(cin >> name);
}

int main() {
    try {
        soci::session sql(soci::oracle, "service=//192.168.100.10:1521/xe user=vhcservice password=vhcservice");
        //soci::session sql(soci::oracle, "service=(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)(HOST=192.168.100.10)(PORT=1521))(CONNECT_DATA=(SERVICE_NAME=xe))) user=vhcservice password=vhcservice");

        int count(0);
        sql << "select count(*) from VSR_VHCPROFILE", soci::into(count);
        cout << "We have " << count << " entries in the phonebook.\n";
        
        Database database("192.168.100.10", 1521, "xe", "vhcservice", "vhcservice");
        database.loadBinaryTree("GD", "", "", 30);

        string name;
        while(getName(name)) {
            string phone;
            soci::indicator ind;
            sql << "select phone from phonebook where name = :name", soci::into(phone, ind), soci::use(name);

            std::clog << sql.get_last_query() << std::endl;

            if(ind == soci::i_ok) {
                cout << "The phone number is " << phone << '\n';
            }
            else {
                cout << "There is no phone for " << name << '\n';
            }
        }
    }
    catch(exception const &e) {
        cerr << "Error: " << e.what() << '\n';
    }
}