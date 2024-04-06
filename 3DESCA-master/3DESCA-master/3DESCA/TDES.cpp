#include "TDES.hpp"


namespace TDESCA {

chunk64 TDES::Encode(chunk64 key1, chunk64 key2, chunk64 key3, chunk64 data)
{
    chunk64 tempData = cipherMachine.Encode(key1, data);
    tempData = cipherMachine.Decode(key2, tempData);
    return cipherMachine.Encode(key3, tempData);
}

chunk64 TDES::Decode(chunk64 key1, chunk64 key2, chunk64 key3, chunk64 data)
{
    chunk64 tempData = cipherMachine.Decode(key3, data);
    tempData = cipherMachine.Encode(key2, tempData);
    return cipherMachine.Decode(key1, tempData);
}

} // namespace TDESCA