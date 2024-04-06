#include <gtest/gtest.h>
#include "3DESCA/TDES.hpp"


const TDESCA::chunk64 msg1(0x6162636465666768); // 'abcdefgh'
const TDESCA::chunk64 result1(0x0438B47607ADDDA0);
const TDESCA::chunk64 msg2(0x6867666564636261); // 'hgfedcba'
const TDESCA::chunk64 result2(0xF468A83547B48F76);
const TDESCA::chunk64 key1(0x0001112223334445);
const TDESCA::chunk64 key2(0x55666777888999AA);
const TDESCA::chunk64 key3(0xABBBCCCDDDEEEFFF);

TEST(TDES, Encode)
{
    TDESCA::TDES cipherAlgorithm;
    TDESCA::chunk64 cipherText = cipherAlgorithm.Encode(key1, key2, key3, msg1);

    ASSERT_EQ(result1.val, cipherText.val) << "In hex: " << std::endl << std::hex << std::setfill('0') <<
        cipherText.val << " vs expected " << result1.val << std::endl << std::endl;


    cipherText = cipherAlgorithm.Encode(key1, key2, key3, msg2);

    ASSERT_EQ(result2.val, cipherText.val) << "In hex: " << std::endl << std::hex << std::setfill('0') <<
        cipherText.val << " vs expected " << result2.val << std::endl << std::endl;
}

TEST(TDES, Decode)
{
    TDESCA::TDES cipherAlgorithm;
    TDESCA::chunk64 cipherText = cipherAlgorithm.Encode(key1, key2, key3, msg1);
    TDESCA::chunk64 decMsg1 = cipherAlgorithm.Decode(key1, key2, key3, cipherText);

    ASSERT_EQ(msg1.val, decMsg1.val) << "In hex: " << std::endl << std::hex << std::setfill('0') <<
        decMsg1.val << " vs expected " << msg1.val << std::endl << std::endl;

    cipherText = cipherAlgorithm.Encode(key1, key2, key3, msg2);
    TDESCA::chunk64 decMsg2 = cipherAlgorithm.Decode(key1, key2, key3, cipherText);

    ASSERT_EQ(msg2.val, decMsg2.val) << "In hex: " << std::endl << std::hex << std::setfill('0') <<
        decMsg2.val << " vs expected " << msg2.val << std::endl << std::endl;
}

TEST(TDES, SwappedKeys)
{
    TDESCA::TDES cipherAlgorithm;
    TDESCA::chunk64 cipherText = cipherAlgorithm.Encode(key1, key2, key3, msg2);
    TDESCA::chunk64 decMsg2 = cipherAlgorithm.Decode(key1, key2, key3, cipherText);
    ASSERT_EQ(msg2.val, decMsg2.val);

    TDESCA::chunk64 errMsg;
    errMsg = cipherAlgorithm.Decode(key1, key3, key2, cipherText);
    ASSERT_NE(msg2.val, errMsg.val);
    errMsg = cipherAlgorithm.Decode(key2, key1, key3, cipherText);
    ASSERT_NE(msg2.val, errMsg.val);
    errMsg = cipherAlgorithm.Decode(key2, key3, key1, cipherText);
    ASSERT_NE(msg2.val, errMsg.val);
    errMsg = cipherAlgorithm.Decode(key3, key2, key1, cipherText);
    ASSERT_NE(msg2.val, errMsg.val);
    errMsg = cipherAlgorithm.Decode(key3, key1, key2, cipherText);
    ASSERT_NE(msg2.val, errMsg.val);
}