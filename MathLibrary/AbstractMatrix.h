#pragma once
#include "IMatrix.h"
class AbstractMatrix :
    public IMatrix
{
    // Inherited via IMatrix
    virtual int getRowsCount() override;
    virtual int getColsCount() override;
    virtual double get(int, int) override;
    virtual IMatrix& set(int, int, double) override;
    virtual IMatrix& copy() override;
    virtual IMatrix& newInstance(int, int) override;
    virtual IMatrix& nTranspose(bool) override;
    virtual IMatrix& add(IMatrix&) override;
    virtual IMatrix& nAdd(IMatrix&) override;
    virtual IMatrix& sub(IMatrix&) override;
    virtual IMatrix& nSub(IMatrix&) override;
    virtual IMatrix& nMultiply(IMatrix&) override;
    virtual double determinant() override;
    virtual IMatrix& subMatrix(int, int, bool) override;
    virtual IMatrix& nInvert() override;
    virtual double** toArray() override;
    virtual IVector& toVector(bool) override;
};

