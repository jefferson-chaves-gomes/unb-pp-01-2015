
#include <gtest/gtest.h>
#include <iostream>

class HelloWorld : public ::testing::Test
{
};

TEST_F(HelloWorld, hello_world)
{
    std::cout << "Hello world!!!" << std::endl;
}
