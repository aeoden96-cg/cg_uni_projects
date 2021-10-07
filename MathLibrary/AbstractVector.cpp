#include "AbstractVector.h"
#include "AbstractMatrix.h"

 double AbstractVector::get(int){return 0.0;}
 IVector& AbstractVector::set(int, double) { return *this; }
 int AbstractVector::getDimension() { return 0; }
 IVector& AbstractVector::copy() { return *this; }
 IVector& AbstractVector::copyPart(int) { return *this; }
 IVector& AbstractVector::newInstance(int) { return *this; }
 IVector& AbstractVector::add(IVector&) { return *this; }
 IVector& AbstractVector::nAdd(IVector&) { return *this; }
 IVector& AbstractVector::sub(IVector&) { return *this; }
 IVector& AbstractVector::nSub(IVector&) { return *this; }
 IVector& AbstractVector::scalarMultiply(double) { return *this; }
 IVector& AbstractVector::nScalarMultiply(double) { return *this; }
 double AbstractVector::norm() { return 0; }
 IVector& AbstractVector::normnalize() { return *this; }
 IVector& AbstractVector::nNormnalize() { return *this; }
 double AbstractVector::cosine(IVector&) { return 0; }
 double AbstractVector::scalarProduct(IVector&) { return 0; }
 IVector& AbstractVector::nVectorProduct(IVector&) { return *this; }
 IVector& AbstractVector::nFromHomogeneus() { return *this; }
 IMatrix& AbstractVector::toRowMatrix(bool) { 
	 AbstractMatrix v = AbstractMatrix();
	 return v;
 }
 IMatrix& AbstractVector::toColumnMatrix(bool) {
	 AbstractMatrix v = AbstractMatrix();
	 return v; 
 }

 double* AbstractVector::toArray()
 {
	 return nullptr;
 }

 IVector& AbstractVector::operator+(const IVector&)
 {
	 // TODO: insert return statement here
	 return *this;
 }

 IVector& AbstractVector::operator-(const IVector&)
 {
	 // TODO: insert return statement here
	 return *this;
 }

 IVector& AbstractVector::operator*(double)
 {
	 // TODO: insert return statement here
	 return *this;
 }

 int& AbstractVector::operator[](int i)
 {
	 // TODO: insert return statement here
	 return ++i;
 }

 double AbstractVector::operator^(const IVector&)
 {
	 return 0.0;
	 return 0;
 }

 void AbstractVector::operator=(const IVector&)
 {
 }
