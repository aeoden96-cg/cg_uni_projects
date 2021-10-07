#include "AbstractMatrix.h"
#include "AbstractVector.h"

int AbstractMatrix::getRowsCount()
{
	return 0;
}

int AbstractMatrix::getColsCount()
{
	return 0;
}

double AbstractMatrix::get(int, int)
{
	return 0.0;
}

IMatrix& AbstractMatrix::set(int, int, double)
{
	// TODO: insert return statement here
	return *this;
}

IMatrix& AbstractMatrix::copy()
{
	// TODO: insert return statement here
	return *this;
}

IMatrix& AbstractMatrix::newInstance(int, int)
{
	// TODO: insert return statement here
	return *this;
}

IMatrix& AbstractMatrix::nTranspose(bool)
{
	// TODO: insert return statement here
	return *this;
}

IMatrix& AbstractMatrix::add(IMatrix&)
{
	// TODO: insert return statement here
	return *this;
}

IMatrix& AbstractMatrix::nAdd(IMatrix&)
{
	// TODO: insert return statement here
	return *this;
}

IMatrix& AbstractMatrix::sub(IMatrix&)
{
	// TODO: insert return statement here
	return *this;
}

IMatrix& AbstractMatrix::nSub(IMatrix&)
{
	// TODO: insert return statement here
	return *this;
}

IMatrix& AbstractMatrix::nMultiply(IMatrix&)
{
	// TODO: insert return statement here
	return *this;
}

double AbstractMatrix::determinant()
{
	return 0.0;
}

IMatrix& AbstractMatrix::subMatrix(int, int, bool)
{
	// TODO: insert return statement here
	return *this;
}

IMatrix& AbstractMatrix::nInvert()
{
	// TODO: insert return statement here
	return *this;
}

double** AbstractMatrix::toArray()
{
	return nullptr;
}

IVector& AbstractMatrix::toVector(bool)
{
	// TODO: insert return statement here
	AbstractVector v = AbstractVector();
	return v;
}
