//
// Created by root on 17-9-4.
//
#include "../Timestamp.h"
#include <vector>

void passByConstRefer(const Timestamp& x)
{
    printf("%s\n",x.toString().c_str());
}

void passByValue( Timestamp x)
{
    printf("%s\n",x.toString().c_str());
}


void benchmark()
{
    const int kNumber = 1000*1000;
    std::vector<Timestamp> stamps;

}

int main()
{
    Timestamp now(Timestamp::now());
    printf("%s\n",now.toString().c_str());

    passByValue(now);
    passByConstRefer(now);

    benchmark();
}