#include <db/TupleDesc.h>

using namespace db;

//
// TDItem
//

bool TDItem::operator==(const TDItem &other) const {
    // TODO pa1.1: implement
    return (fieldType==other.fieldType)&&(fieldName==other.fieldName);
}

std::size_t std::hash<TDItem>::operator()(const TDItem &r) const {
    // TODO pa1.1: implement
    std::size_t hashType=std::hash<Types::Type>()(r.fieldType);
    std::size_t hashFieldName=std::hash<std::string>()(r.fieldName);
    return hashType^(hashFieldName<<1);

}

//
// TupleDesc
//

// TODO pa1.1: implement
TupleDesc::TupleDesc(const std::vector<Types::Type> &types) {
    for(const auto &type:types){
        Items.emplace_back(type,"");
    }
}

// TODO pa1.1: implement
TupleDesc::TupleDesc(const std::vector<Types::Type> &types, const std::vector<std::string> &names) {
    for(std::size_t i=0;i<types.size();i++){
        Items.emplace_back(types[i],names[i]);
    }
}

size_t TupleDesc::numFields() const {
    // TODO pa1.1: implement
    return Items.size();
}

std::string TupleDesc::getFieldName(size_t i) const {
    // TODO pa1.1: implement
    return Items[i].fieldName;
}

Types::Type TupleDesc::getFieldType(size_t i) const {
    // TODO pa1.1: implement
    return Items[i].fieldType;
}

int TupleDesc::fieldNameToIndex(const std::string &fieldName) const {
    // TODO pa1.1: implement
    for(size_t i=0;i<Items.size();i++){
        if(fieldName==Items[i].fieldName){
            return i;
        }
    }
    return -1;
}

size_t TupleDesc::getSize() const {
    // TODO pa1.1: implement
    size_t sizeOfType=0;
    for(size_t i=0;i<Items.size();i++){
        sizeOfType+=Types::getLen(Items[i].fieldType);
    }
    return sizeOfType;

}

TupleDesc TupleDesc::merge(const TupleDesc &td1, const TupleDesc &td2) {
    // TODO pa1.1: implement
    TupleDesc mergeTD;
    for (size_t i=0;i<td1.numFields();i++){
        mergeTD.Items.push_back(td1.Items[i]);
    }
    for (size_t i = 0; i <td2.numFields() ; i++) {
        mergeTD.Items.push_back(td2.Items[i]);
    }
    return mergeTD;
}

std::string TupleDesc::to_string() const {
    // TODO pa1.1: implement
    std::string stringTD;
    for (size_t i = 0; i < numFields(); ++i) {
    stringTD+=Types::to_string(Items[i].fieldType)+"["+std::to_string(i)+"]("+Items[i].fieldName+")";
    if(i!=numFields()-1){
        stringTD+=",";
    }
    }
    return stringTD;
}

bool TupleDesc::operator==(const TupleDesc &other) const {
    // TODO pa1.1: implement
    if (numFields()!=other.numFields()){
        return false;
    }
    for (size_t i = 0; i <numFields() ; ++i) {
        if (Items[i]!=other.Items[i]){
            return false;
        }
    }
    return true;
}

TupleDesc::iterator TupleDesc::begin() const {
    // TODO pa1.1: implement
    return Items.begin();
}

TupleDesc::iterator TupleDesc::end() const {
    // TODO pa1.1: implement
    return Items.end();
}

std::size_t std::hash<db::TupleDesc>::operator()(const db::TupleDesc &td) const {
    // TODO pa1.1: implement
    size_t hashResult=0;
   for(const auto &item:td.getItems()){
       hashResult ^= std::hash<TDItem>()(item) + 0x9e3779b9 + (hashResult << 6) + (hashResult >> 2);

   }
    return hashResult;
}
