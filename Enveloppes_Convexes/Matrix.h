#pragma once
#include <iostream>
#include <vector>
#include "Vertex.h"

using namespace std;

struct Matrix
{
	size_t row, col;
	vector<double> data;

	Matrix();
	Matrix(size_t rows, size_t cols);
	Matrix(vector<double>& const d, size_t rows, size_t cols);
	Matrix(Matrix& const m);
	~Matrix();

	size_t getRow() const {
		return row;
	};
	size_t getCol() const {
		return col;
	};
	void setRow(size_t const rows);
	void setCol(size_t const cols);
	vector<double> getData(Matrix& const m) {
		return m.data;
	};

	static Matrix Translate(float x, float y, float z = 0);
	static Matrix RotateZ(float angle);
	static Matrix RotateX(float angle);
	static Matrix RotateY(float angle);
	static Matrix Scale(float scale);
	static Matrix Ortho(float left, float right, float bottom, float top, float znear, float zfar);
	static Matrix Perspective(float FoV, float aspectRatio, float n, float f);
	static Matrix LookAt(Vec3 position, Vec3 target, Vec3 up);

	vector<double> identity(Matrix& const m);
	void swap(Matrix& m);

	double& operator()(size_t i, size_t j);
	double operator()(size_t i, size_t j) const;
	Matrix& operator*=(Matrix& const m);
	friend Matrix operator*(Matrix& const m, Matrix& const m2);
	Matrix& operator+=(Matrix& const m);
	friend Matrix operator+(Matrix& const m, Matrix& const m2);
	Matrix operator-=(Matrix& const m);
	friend Matrix operator-(Matrix& const m, Matrix& const m2);
	Matrix& operator=(Matrix& const m);
	
	Vertex operator*(Vertex v);
};

ostream& operator<<(ostream& os, Matrix& r);
