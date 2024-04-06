#include "Bit.hpp"
#include "DES.hpp"

#define HALFKEY_LEN 28

const uint8_t sBox[8][4][16] = {
    {
        { 14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7 },
        { 0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8 },
        { 4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0 },
        { 15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13 }
    },
     {
        { 15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10 },
        { 3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5 },
        { 0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15 },
        { 13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9 }
    },
    {
        { 10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8 },
        { 13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1 },
        { 13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7 },
        { 1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12 }
    },
    {
        { 7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15 },
        { 13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9 },
        { 10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4 },
        { 3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14 }
    },
    {
        { 2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9 },
        { 14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6 },
        { 4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14 },
        { 11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3 }
    },
    {
        { 12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11 },
        { 10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8 },
        { 9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6 },
        { 4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13 }
    },
    {
        { 4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1 },
        { 13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6 },
        { 1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2 },
        { 6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12 }
    },
    {
        { 13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7 },
        { 1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2 },
        { 7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8 },
        { 2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11 }
    }
};

namespace TDESCA {

uint8_t DES::Substitution(uint8_t sBoxNo, uint8_t inputVal)
{
    uint8_t idxH = (inputVal >> 1) & 0xF;
    uint8_t idxV = ((inputVal >> 4) & 0x2) | (inputVal & 0x1);

    if (idxH < 16 && idxV < 4 && sBoxNo < 8)
        return sBox[sBoxNo][idxV][idxH];

    return 0xFA;
}


chunk64 DES::InitPerm(chunk64 asd)
{
    chunk64 result;
    result.val = 0;
    uint8_t index = 1;
    uint8_t size = static_cast<uint8_t>(asd.arr.size());
    for (uint8_t i = 0; i < size; i++)
    {
        for (uint8_t j = 0; j < size; j++)
        {
            // Set jth bit of result row i with INDEX bit or asd row 7-i
            uint8_t bit = GETBIT(asd.arr[size - 1 - j], index);
            SETBIT(result.arr[i], j, bit);
        }

        index += 2;
        if (index > 8)
            index = 0;
    }

    return result;
}

chunk64 DES::FinalPerm(chunk64 asd)
{
    chunk64 result;
    result.val = 0;
    uint8_t size = static_cast<uint8_t>(asd.arr.size());
    uint8_t halfSize = size / 2;
    uint8_t index = size - 1;
    for (uint8_t i = 0; i < size; i++)
    {
        for (uint8_t j = 0; j < halfSize; j++)
        {
            // Set 2*ith bit of result row i with INDEX bit of asd row j+4
            SETBIT(result.arr[i], (2 * j    ), GETBIT(asd.arr[j + halfSize], index));
            SETBIT(result.arr[i], (2 * j + 1), GETBIT(asd.arr[j           ], index));
        }
        index--;
    }

    return result;
}

chunk64 DES::Expansion(chunk32 we)
{
    chunk64 result;
    result.val = 0;
    uint64_t weDup = we.val;
    weDup = (weDup << 32) | we.val;
    weDup = Ror(weDup, 17);
    uint64_t mask = 0xFC0000000000;
    for (int i =0; i < 8; i++)
    {
        result.val = result.val | (weDup & mask);
        weDup = Ror(weDup, 2);
        mask >>= 6;
    }

    return result;
}

chunk32 DES::SBoxPermutation(chunk32 in)
{
    chunk32 result;
    result.val = 0;

    SETBIT(result.arr[3], 7, GETBIT(in.arr[2], 0));
    SETBIT(result.arr[3], 6, GETBIT(in.arr[3], 1));
    SETBIT(result.arr[3], 5, GETBIT(in.arr[1], 4));
    SETBIT(result.arr[3], 4, GETBIT(in.arr[1], 3));
    SETBIT(result.arr[3], 3, GETBIT(in.arr[0], 3));
    SETBIT(result.arr[3], 2, GETBIT(in.arr[2], 4));
    SETBIT(result.arr[3], 1, GETBIT(in.arr[0], 4));
    SETBIT(result.arr[3], 0, GETBIT(in.arr[1], 7));

    SETBIT(result.arr[2], 7, GETBIT(in.arr[3], 7));
    SETBIT(result.arr[2], 6, GETBIT(in.arr[2], 1));
    SETBIT(result.arr[2], 5, GETBIT(in.arr[1], 1));
    SETBIT(result.arr[2], 4, GETBIT(in.arr[0], 6));
    SETBIT(result.arr[2], 3, GETBIT(in.arr[3], 3));
    SETBIT(result.arr[2], 2, GETBIT(in.arr[1], 6));
    SETBIT(result.arr[2], 1, GETBIT(in.arr[0], 1));
    SETBIT(result.arr[2], 0, GETBIT(in.arr[2], 6));

    SETBIT(result.arr[1], 7, GETBIT(in.arr[3], 6));
    SETBIT(result.arr[1], 6, GETBIT(in.arr[3], 0));
    SETBIT(result.arr[1], 5, GETBIT(in.arr[1], 0));
    SETBIT(result.arr[1], 4, GETBIT(in.arr[2], 2));
    SETBIT(result.arr[1], 3, GETBIT(in.arr[0], 0));
    SETBIT(result.arr[1], 2, GETBIT(in.arr[0], 5));
    SETBIT(result.arr[1], 1, GETBIT(in.arr[3], 5));
    SETBIT(result.arr[1], 0, GETBIT(in.arr[2], 7));

    SETBIT(result.arr[0], 7, GETBIT(in.arr[1], 5));
    SETBIT(result.arr[0], 6, GETBIT(in.arr[2], 3));
    SETBIT(result.arr[0], 5, GETBIT(in.arr[0], 2));
    SETBIT(result.arr[0], 4, GETBIT(in.arr[3], 2));
    SETBIT(result.arr[0], 3, GETBIT(in.arr[1], 2));
    SETBIT(result.arr[0], 2, GETBIT(in.arr[2], 5));
    SETBIT(result.arr[0], 1, GETBIT(in.arr[3], 4));
    SETBIT(result.arr[0], 0, GETBIT(in.arr[0], 7));
    
    return result;
}

chunk64 DES::PermutedChoice1(chunk64 key)
{
    chunk64 result;
    result.val = 0;
    
    SETBIT(result.arr[6], 7, GETBIT(key.arr[0], 7));
    SETBIT(result.arr[6], 6, GETBIT(key.arr[1], 7));
    SETBIT(result.arr[6], 5, GETBIT(key.arr[2], 7));
    SETBIT(result.arr[6], 4, GETBIT(key.arr[3], 7));
    SETBIT(result.arr[6], 3, GETBIT(key.arr[4], 7));
    SETBIT(result.arr[6], 2, GETBIT(key.arr[5], 7));
    SETBIT(result.arr[6], 1, GETBIT(key.arr[6], 7));

    SETBIT(result.arr[6], 0, GETBIT(key.arr[7], 7));
    SETBIT(result.arr[5], 7, GETBIT(key.arr[0], 6));
    SETBIT(result.arr[5], 6, GETBIT(key.arr[1], 6));
    SETBIT(result.arr[5], 5, GETBIT(key.arr[2], 6));
    SETBIT(result.arr[5], 4, GETBIT(key.arr[3], 6));
    SETBIT(result.arr[5], 3, GETBIT(key.arr[4], 6));
    SETBIT(result.arr[5], 2, GETBIT(key.arr[5], 6));

    SETBIT(result.arr[5], 1, GETBIT(key.arr[6], 6));
    SETBIT(result.arr[5], 0, GETBIT(key.arr[7], 6));
    SETBIT(result.arr[4], 7, GETBIT(key.arr[0], 5));
    SETBIT(result.arr[4], 6, GETBIT(key.arr[1], 5));
    SETBIT(result.arr[4], 5, GETBIT(key.arr[2], 5));
    SETBIT(result.arr[4], 4, GETBIT(key.arr[3], 5));
    SETBIT(result.arr[4], 3, GETBIT(key.arr[4], 5));

    SETBIT(result.arr[4], 2, GETBIT(key.arr[5], 5));
    SETBIT(result.arr[4], 1, GETBIT(key.arr[6], 5));
    SETBIT(result.arr[4], 0, GETBIT(key.arr[7], 5));
    SETBIT(result.arr[3], 7, GETBIT(key.arr[0], 4));
    SETBIT(result.arr[3], 6, GETBIT(key.arr[1], 4));
    SETBIT(result.arr[3], 5, GETBIT(key.arr[2], 4));
    SETBIT(result.arr[3], 4, GETBIT(key.arr[3], 4));

    SETBIT(result.arr[3], 3, GETBIT(key.arr[0], 1));
    SETBIT(result.arr[3], 2, GETBIT(key.arr[1], 1));
    SETBIT(result.arr[3], 1, GETBIT(key.arr[2], 1));
    SETBIT(result.arr[3], 0, GETBIT(key.arr[3], 1));
    SETBIT(result.arr[2], 7, GETBIT(key.arr[4], 1));
    SETBIT(result.arr[2], 6, GETBIT(key.arr[5], 1));
    SETBIT(result.arr[2], 5, GETBIT(key.arr[6], 1));

    SETBIT(result.arr[2], 4, GETBIT(key.arr[7], 1));
    SETBIT(result.arr[2], 3, GETBIT(key.arr[0], 2));
    SETBIT(result.arr[2], 2, GETBIT(key.arr[1], 2));
    SETBIT(result.arr[2], 1, GETBIT(key.arr[2], 2));
    SETBIT(result.arr[2], 0, GETBIT(key.arr[3], 2));
    SETBIT(result.arr[1], 7, GETBIT(key.arr[4], 2));
    SETBIT(result.arr[1], 6, GETBIT(key.arr[5], 2));

    SETBIT(result.arr[1], 5, GETBIT(key.arr[6], 2));
    SETBIT(result.arr[1], 4, GETBIT(key.arr[7], 2));
    SETBIT(result.arr[1], 3, GETBIT(key.arr[0], 3));
    SETBIT(result.arr[1], 2, GETBIT(key.arr[1], 3));
    SETBIT(result.arr[1], 1, GETBIT(key.arr[2], 3));
    SETBIT(result.arr[1], 0, GETBIT(key.arr[3], 3));
    SETBIT(result.arr[0], 7, GETBIT(key.arr[4], 3));

    SETBIT(result.arr[0], 6, GETBIT(key.arr[5], 3));
    SETBIT(result.arr[0], 5, GETBIT(key.arr[6], 3));
    SETBIT(result.arr[0], 4, GETBIT(key.arr[7], 3));
    SETBIT(result.arr[0], 3, GETBIT(key.arr[4], 4));
    SETBIT(result.arr[0], 2, GETBIT(key.arr[5], 4));
    SETBIT(result.arr[0], 1, GETBIT(key.arr[6], 4));
    SETBIT(result.arr[0], 0, GETBIT(key.arr[7], 4));

    return result;
}

chunk64 DES::PermutedChoice2(chunk64 key)
{
    chunk64 result;
    result.val = 0;

    SETBIT(result.arr[5], 7, GETBIT(key.arr[5], 2));
    SETBIT(result.arr[5], 6, GETBIT(key.arr[4], 7));
    SETBIT(result.arr[5], 5, GETBIT(key.arr[5], 5));
    SETBIT(result.arr[5], 4, GETBIT(key.arr[4], 0));
    SETBIT(result.arr[5], 3, GETBIT(key.arr[6], 7));
    SETBIT(result.arr[5], 2, GETBIT(key.arr[6], 3));

    SETBIT(result.arr[5], 1, GETBIT(key.arr[6], 5));
    SETBIT(result.arr[5], 0, GETBIT(key.arr[3], 4));
    SETBIT(result.arr[4], 7, GETBIT(key.arr[5], 1));
    SETBIT(result.arr[4], 6, GETBIT(key.arr[6], 2));
    SETBIT(result.arr[4], 5, GETBIT(key.arr[4], 3));
    SETBIT(result.arr[4], 4, GETBIT(key.arr[5], 6));

    SETBIT(result.arr[4], 3, GETBIT(key.arr[4], 1));
    SETBIT(result.arr[4], 2, GETBIT(key.arr[4], 5));
    SETBIT(result.arr[4], 1, GETBIT(key.arr[5], 4));
    SETBIT(result.arr[4], 0, GETBIT(key.arr[6], 4));
    SETBIT(result.arr[3], 7, GETBIT(key.arr[3], 6));
    SETBIT(result.arr[3], 6, GETBIT(key.arr[6], 0));

    SETBIT(result.arr[3], 5, GETBIT(key.arr[5], 0));
    SETBIT(result.arr[3], 4, GETBIT(key.arr[6], 1));
    SETBIT(result.arr[3], 3, GETBIT(key.arr[3], 5));
    SETBIT(result.arr[3], 2, GETBIT(key.arr[4], 4));
    SETBIT(result.arr[3], 1, GETBIT(key.arr[5], 3));
    SETBIT(result.arr[3], 0, GETBIT(key.arr[6], 6));
    
    SETBIT(result.arr[2], 7, GETBIT(key.arr[1], 7));
    SETBIT(result.arr[2], 6, GETBIT(key.arr[0], 4));
    SETBIT(result.arr[2], 5, GETBIT(key.arr[3], 1));
    SETBIT(result.arr[2], 4, GETBIT(key.arr[2], 3));
    SETBIT(result.arr[2], 3, GETBIT(key.arr[1], 1));
    SETBIT(result.arr[2], 2, GETBIT(key.arr[0], 1));

    SETBIT(result.arr[2], 1, GETBIT(key.arr[3], 2));
    SETBIT(result.arr[2], 0, GETBIT(key.arr[2], 0));
    SETBIT(result.arr[1], 7, GETBIT(key.arr[0], 5));
    SETBIT(result.arr[1], 6, GETBIT(key.arr[1], 3));
    SETBIT(result.arr[1], 5, GETBIT(key.arr[2], 7));
    SETBIT(result.arr[1], 4, GETBIT(key.arr[1], 0));

    SETBIT(result.arr[1], 3, GETBIT(key.arr[1], 4));
    SETBIT(result.arr[1], 2, GETBIT(key.arr[0], 7));
    SETBIT(result.arr[1], 1, GETBIT(key.arr[2], 1));
    SETBIT(result.arr[1], 0, GETBIT(key.arr[0], 0));
    SETBIT(result.arr[0], 7, GETBIT(key.arr[2], 6));
    SETBIT(result.arr[0], 6, GETBIT(key.arr[0], 3));

    SETBIT(result.arr[0], 5, GETBIT(key.arr[1], 2));
    SETBIT(result.arr[0], 4, GETBIT(key.arr[1], 6));
    SETBIT(result.arr[0], 3, GETBIT(key.arr[0], 6));
    SETBIT(result.arr[0], 2, GETBIT(key.arr[2], 4));
    SETBIT(result.arr[0], 1, GETBIT(key.arr[3], 3));
    SETBIT(result.arr[0], 0, GETBIT(key.arr[3], 0));

    return result;
}

// There is a problem with "not-fully-used-numbertypes", as they cannot be rotated, etc normally
uint32_t DES::RotateHalfkeyLeft(chunk32 halfKey, uint8_t n)
{
    uint8_t rot = n % HALFKEY_LEN;

    // first, rotate the entire value left by rot numbers
    uint64_t result = halfKey.val << rot;

    // grab whatever exceeded HALFKEY_LEN bits
    uint64_t remainder = (result >> HALFKEY_LEN) & 0x0FFFFFFF;

    // and add it at the end
    return (result | remainder) & 0x0FFFFFFF;
}

uint8_t rotNumber(uint8_t round) // It's short, so can be converted into constarray for speed
{
    switch(round)
    {
        case 0:
        case 1:
        case 8:
        case 15:
            return 1;

        default:
            return 2;
    }

    return 0;
}

chunk32 DES::Feistel(chunk32 data, chunk64 subKey)
{
    // 32bit chunk is expanded to 48bits, referred to as X1
    chunk64 expandedData = Expansion(data);

    // X2 = X1 xor subkey
    expandedData.val = expandedData.val ^ subKey.val;
    chunk32 sboxedData;
    sboxedData.val = 0;

    // X2 is split into 8 6bit parts
    for (int i = 0; i < 8; i++)
    {
        // each part is put into subsitution func with ascending S-box number (from 0 - 7)
        uint8_t val = (expandedData.val >> (6 * (7 - i))) & 0x3F;
        uint32_t subVal = Substitution(i, val);

        // output of each part is put together into 32bit X3
        sboxedData.val = sboxedData.val | (subVal << (4 * (7 - i)));
    }

    // X3 goes into SBoxPermutation func and result comes out (here for R1, result is Rf1)
    return SBoxPermutation(sboxedData);
}

chunk64 DES::Process(chunk64 key, chunk64 data, bool isDecode)
{
    // remove 8bits from key for parity check - the key is now 56bit
    chunk64 strippedKey = PermutedChoice1(key);

    // split output from PC1 to left and right
    chunk32 lKey, rKey;
    lKey.val = (strippedKey.val >> 28) & 0x0FFFFFFF;
    rKey.val = strippedKey.val & 0x0FFFFFFF;

    // generate 16 subKeys - 1 for each round
    chunk64 subKeys[16];

    for (int i = 0; i < 16; i++)
    {
        uint8_t rot = rotNumber(i);
        chunk64 currentKey;
        lKey.val = RotateHalfkeyLeft(lKey, rot);
        rKey.val = RotateHalfkeyLeft(rKey, rot);
        currentKey.val = lKey.val;
        currentKey.val = (currentKey.val << 28) | rKey.val;
        subKeys[i] = PermutedChoice2(currentKey);
    }

    // get single chunk of data
    // put it into Initial Permutation
    chunk64 permData = InitPerm(data);

    // split into 32bit parts: L1 and R1
    chunk32 lData, rData, tempData;
    lData.val = permData.val >> 32;
    rData.val = permData.val & 0xFFFFFFFF;

    // Set subKeys iteration direction, as it decides whether it's encoding or decoding
    int8_t idxChange = 1;
    int8_t subkeyIdx = 0;
    if (isDecode)
    {
        subkeyIdx = 15;
        idxChange = -1;
    }
    while(subkeyIdx < 16 && subkeyIdx >= 0)
    {
        tempData = rData;

        // L2 = R1, R2 = L1 xor Rf1 (R1 goes into Feistel, becomes Rf1)
        rData.val = lData.val ^ Feistel(rData, subKeys[subkeyIdx]).val;
        lData = tempData;
        subkeyIdx += idxChange;
    }

    lData = rData;
    rData = tempData;

    // join L = L16 xor Rf16 and R = R16
    chunk64 cipher;
    cipher.val = lData.val;
    cipher.val = (cipher.val << 32) | rData.val;

    // put into Final Permutation
    // u got single chunk of ciphertext
    return FinalPerm(cipher);
}

chunk64 DES::Encode(chunk64 key, chunk64 data)
{
    return Process(key, data, false);
}

chunk64 DES::Decode(chunk64 key, chunk64 data)
{
    return Process(key, data, true);
}

} // namespace TDESCA
