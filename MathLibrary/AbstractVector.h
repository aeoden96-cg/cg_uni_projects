#pragma once
#include "IVector.h"
class AbstractVector : public IVector
{
public:
    virtual double get(int) override;
    virtual IVector& set(int, double) override;
    virtual int getDimension()override;
    virtual IVector& copy()override;
    virtual IVector& copyPart(int)override;
    virtual IVector& newInstance(int)override;
    virtual IVector& add(IVector&)override;
    virtual IVector& nAdd(IVector&)override;
    virtual IVector& sub(IVector&) override;
    virtual IVector& nSub(IVector&)override;
    virtual IVector& scalarMultiply(double)override;
    virtual IVector& nScalarMultiply(double)override;
    virtual double norm()override;
    virtual IVector& normnalize()override;
    virtual IVector& nNormnalize()override;
    virtual double cosine(IVector&) override;
    virtual double scalarProduct(IVector&)override;
    virtual IVector& nVectorProduct(IVector&)override;
    virtual IVector& nFromHomogeneus()override;
    virtual IMatrix& toRowMatrix(bool) override;
    virtual IMatrix& toColumnMatrix(bool)override;
    virtual double* toArray() override;

    virtual IVector& operator+(const IVector&) override;
    virtual IVector& operator-(const IVector&) override;
    virtual IVector& operator*(double) override;
    virtual int& operator[](int i) override;
    virtual double operator^(const IVector&)override;
    virtual void operator=(const IVector&) override;
};

