#include <db/Tuple.h>

using namespace db;

//
// Tuple
//

// TODO pa1.1: implement
Tuple::Tuple(const TupleDesc &td, RecordId *rid) {
    tupleDesc=td;
    for (size_t i = 0; i <tupleDesc.numFields() ; ++i) {
        fields.push_back(nullptr);
    }
    if(rid){
    recordId=std::make_unique<RecordId>(*rid);
    }
}

const TupleDesc &Tuple::getTupleDesc() const {
    // TODO pa1.1: implement
    return tupleDesc;
}

const RecordId *Tuple::getRecordId() const {
    // TODO pa1.1: implement
    return recordId.get();
}

void Tuple::setRecordId(const RecordId *id) {
    // TODO pa1.1: implement
    recordId=std::make_unique<RecordId>(*id);
}

const Field &Tuple::getField(int i) const {
    // TODO pa1.1: implement
    return *(fields[i]);
}

void Tuple::setField(int i, const Field *f) {
    // TODO pa1.1: implement
    //fields[i]=std::make_unique<Field>(f);
    fields[i]=f->copy();
}

Tuple::iterator Tuple::begin() const {
    // TODO pa1.1: implement
    return fields.begin();
}

Tuple::iterator Tuple::end() const {
    // TODO pa1.1: implement
    return fields.end();
}

std::string Tuple::to_string() const {
    // TODO pa1.1: implement
    std::string output="[";
    for (size_t i= 0; i < tupleDesc.numFields(); ++i) {
        output += fields[i]->to_string();
        if (i < tupleDesc.numFields() - 1) {
            output += ", ";
        }
    }
        output+="]";
    return output;
}
