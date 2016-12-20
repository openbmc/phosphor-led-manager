#include <gtest/gtest.h>
#include <iostream>
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    if(::testing::GTEST_FLAG(shuffle) == true)
    {
        std::cerr << "WARNING !!!.. This test needs a HARD sequencing."
                  << " Ignoring Shuffle\n";
        ::testing::GTEST_FLAG(shuffle) = false;
    }
    return RUN_ALL_TESTS();
}
