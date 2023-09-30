#include <db/Catalog.h>
#include <db/TupleDesc.h>
#include <stdexcept>

using namespace db;

void Catalog::addTable(DbFile *file, const std::string &name, const std::string &pkeyField) {
    // TODO pa1.2: implement
    int id=file->getId();
    Table newTable(file, name, pkeyField);
    idTable[id];
    nameId[name]=id;


}

int Catalog::getTableId(const std::string &name) const {
    // TODO pa1.2: implement
    auto it = nameId.find(name);

    if (it != nameId.end()) {
        return it->second;
    } else {
        throw std::runtime_error("Table not found");
    }
}

const TupleDesc &Catalog::getTupleDesc(int tableid) const {
    // TODO pa1.2: implement
    auto it = idTable.find(tableid);
    if (it != idTable.end()) {
        return it->second.file->getTupleDesc(); // Assuming the DbFile has a method getTupleDesc.
    }

    // If table is not found, throw an exception.
    throw std::runtime_error("Table ID not found in the catalog.");
}

DbFile *Catalog::getDatabaseFile(int tableid) const {
    // TODO pa1.2: implement
    auto it = idTable.find(tableid);
    if (it != idTable.end()) {
        return it->second.file;
    }

    // If table is not found, throw an exception.
    throw std::runtime_error("Table ID not found in the catalog.");
}

std::string Catalog::getPrimaryKey(int tableid) const {
    // TODO pa1.2: implement
    auto it = idTable.find(tableid);
    if (it != idTable.end()) {
        return it->second.pkeyField;
    }

    // If table is not found, throw an exception.
    throw std::runtime_error("Table ID not found in the catalog.");
}

std::string Catalog::getTableName(int id) const {
    // TODO pa1.2: implement
    auto it = idTable.find(id);
    if (it != idTable.end()) {
        return it->second.name;
    }

    // If table is not found, throw an exception.
    throw std::runtime_error("Table ID not found in the catalog.");
}

void Catalog::clear() {
    // TODO pa1.2: implement
    idTable.clear();
    nameId.clear();
}
