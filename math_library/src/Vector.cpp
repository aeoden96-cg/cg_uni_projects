#include "Vector.h"
#include <vector>
#include <sstream>
#include <fstream>
#include <istream>
#include <iostream>

using namespace std;

std::ostream& operator<<(std::ostream& strm, const Vector& a) {
	
	strm << "(" << a.elements[0] << ", " << a.elements[1] << ")" << endl;
	return strm;

}


double Vector::get(int i)  {
	return this->elements[i];
}
IVector& Vector::set(int i ,double s){
	this->elements[i] = s;
	return *this;
}
int Vector::getDimension(){
	return this->dimension;
}
IVector& Vector::copy(){

	Vector* x = new Vector(elements);
	return *x;
}
IVector& Vector::newInstance(int){
	Vector v = Vector(elements);
	return v;
}
Vector& Vector::parseSimple(std::string s){
	std::vector<double> vect;

	std::stringstream ss(s);

	for (double i; ss >> i;) {
		vect.push_back(i);
		if (ss.peek() == ' ')
			ss.ignore();
	}
	
	Vector* v = new Vector(&vect[0]);

	return *v;
}

IVector& Vector::operator+(const IVector& v)
{
	
	for (int i = 0; i < this->dimension; i++) {

		this->elements[i] += ((Vector&)v).elements[i];
	}

	return *this;
}
