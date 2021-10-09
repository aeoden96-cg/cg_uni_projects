
#include <iostream>
#include< fstream >
#include<istream>
#include <string>

#include "Vector.h"

using namespace std;

int main()
{
    std::cout << "Hello World!\n";

    double a[] = { 10, 20, 30, 40 };

    Vector v(a);

    cout << v;

    Vector b = (Vector&)v.copy();

    b.set(0, 25);

    cout << v;
    cout << b;

    Vector z = Vector::parseSimple("1 2 3");

    cout << z;


    cout <<  (Vector&)(v + b);

    



}

