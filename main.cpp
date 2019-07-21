#include "Server.h"
#include <iostream>

void add(int a, int b, double c)
{
    std::cout << a + b << std::endl;
}

int main() 
{
    opc::Server s;
    s.addMethod("addMethod", &add);
    s.run();
}