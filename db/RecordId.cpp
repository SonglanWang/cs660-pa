#include <db/RecordId.h>
#include <stdexcept>

using namespace db;

//
// RecordId
//

// TODO pa1.4: implement
RecordId::RecordId(const PageId *pid, int tupleno) :Pid(pid),tupleNo(tupleno){

}

bool RecordId::operator==(const RecordId &other) const {
    // TODO pa1.4: implement
    return (*Pid == *(other.Pid)) && (tupleNo == other.tupleNo);

}

const PageId *RecordId::getPageId() const {
    // TODO pa1.4: implement
    return Pid;
}

int RecordId::getTupleno() const {
    // TODO pa1.4: implement
    return tupleNo;
}

//
// RecordId hash function
//

std::size_t std::hash<RecordId>::operator()(const RecordId &r) const {
    // TODO pa1.4: implement
}
