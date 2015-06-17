
#include <gtest/gtest.h>
#include <iostream>

class HelloWorld : public ::testing::Test
{
};

TEST_F(HelloWorld, hello_world)
{
    for (int i = 0; i < 50; ++i) {
        std::cout << "Hello world!!! - " << i << std::endl;
    }
}
