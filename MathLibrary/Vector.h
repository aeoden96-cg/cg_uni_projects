#include "AbstractVector.h"
#include <string>
using namespace std;

class Vector : public AbstractVector {
public:
    double* elements;
    int dimension;
    bool readOnly;
    

    friend ostream& operator<<(ostream& os, const Vector&);

    Vector(double* s){
        elements = new double[sizeof(s)];

        for (int i(0); i < sizeof(s); i++) {
            this->elements[i] = s[i];
        }
        this->elements = elements;

        this->dimension= sizeof(s);
        this->readOnly=false;
    }
    Vector(bool a,bool b, double s[]){
        this->elements = s;
        this->dimension=2;
        this->readOnly=true;
    }

    ~Vector() {
        delete this->elements;
    }


    double get(int) override;
    IVector& set(int,double) override;
    int getDimension() override;
    IVector& copy() override;
    IVector& newInstance(int sa) override;

    static Vector& parseSimple(std::string  s);

    IVector& operator+(const IVector&) override;
  

};
