#include <gtest/gtest.h>
#include "3DESCA/DES.hpp"


const TDESCA::chunk64 msg1(0x6162636465666768); // 'abcdefgh'
const TDESCA::chunk64 result1(0x8FB1F64BBB168810);
const TDESCA::chunk64 msg2(0x6867666564636261); // 'hgfedcba'
const TDESCA::chunk64 result2(0x21FE14F931B3F9D6);
const TDESCA::chunk64 key(0x0123456789ABCDEF);

TEST(DES, Encode)
{
    TDESCA::DES cipherAlgorithm;
    TDESCA::chunk64 cipherText = cipherAlgorithm.Encode(key, msg1);

    ASSERT_EQ(result1.val, cipherText.val) << "In hex: " << std::endl << std::hex << std::setfill('0') <<
        cipherText.val << " vs expected " << result1.val << std::endl << std::endl;


    cipherText = cipherAlgorithm.Encode(key, msg2);

    ASSERT_EQ(result2.val, cipherText.val) << "In hex: " << std::endl << std::hex << std::setfill('0') <<
        cipherText.val << " vs expected " << result2.val << std::endl << std::endl;
}

TEST(DES, Decode)
{
    TDESCA::DES cipherAlgorithm;
    TDESCA::chunk64 cipherText = cipherAlgorithm.Encode(key, msg1);
    TDESCA::chunk64 decMsg1 = cipherAlgorithm.Decode(key, cipherText);

    ASSERT_EQ(msg1.val, decMsg1.val) << "In hex: " << std::endl << std::hex << std::setfill('0') <<
        decMsg1.val << " vs expected " << msg1.val << std::endl << std::endl;

    cipherText = cipherAlgorithm.Encode(key, msg2);
    TDESCA::chunk64 decMsg2 = cipherAlgorithm.Decode(key, cipherText);

    ASSERT_EQ(msg2.val, decMsg2.val) << "In hex: " << std::endl << std::hex << std::setfill('0') <<
        decMsg2.val << " vs expected " << msg2.val << std::endl << std::endl;
}
