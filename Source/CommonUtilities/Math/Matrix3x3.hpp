#pragma once
#include "Vector3.hpp"
#include "Matrix4x4.hpp"
namespace CommonUtilities
{
	template <class T>
	class Matrix3x3
	{
	public:

		// Creates the identity matrix.
		Matrix3x3<T>();

		// Copy constructor
		Matrix3x3<T>(const Matrix3x3<T>&) = default;

		// Copies the top left 3x3 part of the Matrix4x4.
		Matrix3x3<T>(const Matrix4x4<T>& aMatrix);

		// () operator for accessing element (row, column) for read/write or read, respectively.
		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;

		// Static functions for creating rotation matrices.
		static Matrix3x3<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundZ(T aAngleInRadians);

		// Static function for creating a transpose of a matrix.
		static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose);

		// Operator Overloading
		const Matrix3x3<T> operator+(const Matrix3x3<T>& aMatrix);
		const Matrix3x3<T> operator-(const Matrix3x3<T>& aMatrix);
		const Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix);
		const Matrix3x3<T>& operator=(const Matrix3x3<T>& aMatrix);
		const bool operator==(const Matrix3x3<T>& aMatrix) const;
		void operator+=(const Matrix3x3<T>& aMatrix);
		void operator-=(const Matrix3x3<T>& aMatrix);
		void operator*=(const Matrix3x3<T>& aMatrix);

	private:

		T myMatrix[3][3];
	};
	template<class T>
	inline Matrix3x3<T>::Matrix3x3()
	{
		for (unsigned int row = 0; row < 3; row++)
		{
			for (unsigned int col = 0; col < 3; col++)
			{
				if (row == col)
				{
					myMatrix[row][col] = static_cast<T>(1);
				}
				else
				{
					myMatrix[row][col] = T();
				}
			}
		}
	}
	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
	{
		for (unsigned int row = 0; row < 3; row++)
		{
			for (unsigned int col = 0; col < 3; col++)
			{
				myMatrix[row][col] = aMatrix(row + 1, col + 1);
			}
		}
	}
	template<class T>
	inline T& Matrix3x3<T>::operator()(const int aRow, const int aColumn)
	{
		return myMatrix[aRow - 1][aColumn - 1];
	}
	template<class T>
	inline const T& Matrix3x3<T>::operator()(const int aRow, const int aColumn) const
	{
		return myMatrix[aRow - 1][aColumn - 1];
	}
	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix3x3 matrix;

		matrix(1, 1) = 1;
		matrix(1, 2) = 0;
		matrix(1, 3) = 0;

		matrix(2, 1) = 0;
		matrix(2, 2) = cos(aAngleInRadians);
		matrix(2, 3) = sin(aAngleInRadians);

		matrix(3, 1) = 0;
		matrix(3, 2) = -sin(aAngleInRadians);
		matrix(3, 3) = cos(aAngleInRadians);

		return matrix;
	}
	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix3x3 matrix;

		matrix(1, 1) = cos(aAngleInRadians);
		matrix(1, 2) = 0;
		matrix(1, 3) = -sin(aAngleInRadians);

		matrix(2, 1) = 0;
		matrix(2, 2) = 1;
		matrix(2, 3) = 0;

		matrix(3, 1) = sin(aAngleInRadians);
		matrix(3, 2) = 0;
		matrix(3, 3) = cos(aAngleInRadians);

		return matrix;
	}
	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix3x3 matrix;

		matrix(1, 1) = cos(aAngleInRadians);
		matrix(1, 2) = sin(aAngleInRadians);
		matrix(1, 3) = 0;

		matrix(2, 1) = -sin(aAngleInRadians);
		matrix(2, 2) = cos(aAngleInRadians);
		matrix(2, 3) = 0;

		matrix(3, 1) = 0;
		matrix(3, 2) = 0;
		matrix(3, 3) = 1;

		return matrix;
	}
	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Transpose(const Matrix3x3<T>& aMatrixToTranspose)
	{
		Matrix3x3 matrix;

		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int col = 1; col < 4; col++)
			{
				matrix(row, col) = aMatrixToTranspose(col, row);
			}
		}

		return matrix;
	}
	template<class T>
	inline const Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3<T>& aMatrix)
	{
		Matrix3x3 matrix(*this);
		matrix += aMatrix;
		return matrix;
	}
	template<class T>
	inline const Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3<T>& aMatrix)
	{
		Matrix3x3 matrix(*this);
		matrix -= aMatrix;
		return matrix;
	}
	template<class T>
	inline const Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3<T>& aMatrix)
	{
		Matrix3x3 matrix(*this);
		matrix *= aMatrix;
		return matrix;
	}
	template<class T>
	inline const Matrix3x3<T>& Matrix3x3<T>::operator=(const Matrix3x3<T>& aMatrix)
	{
		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int col = 1; col < 4; col++)
			{
				(*this)(row, col) = aMatrix(row, col);
			}
		}
		return *this;
	}
	template<class T>
	inline const bool Matrix3x3<T>::operator==(const Matrix3x3<T>& aMatrix) const
	{
		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int col = 1; col < 4; col++)
			{
				if ((*this)(row, col) != aMatrix(row, col))
				{
					return false;
				}
			}
		}
		return true;
	}
	template<class T>
	inline void Matrix3x3<T>::operator+=(const Matrix3x3<T>& aMatrix)
	{
		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int col = 1; col < 4; col++)
			{
				(*this)(row, col) += aMatrix(row, col);
			}
		}
	}
	template<class T>
	inline void Matrix3x3<T>::operator-=(const Matrix3x3<T>& aMatrix)
	{
		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int col = 1; col < 4; col++)
			{
				(*this)(row, col) -= aMatrix(row, col);
			}
		}
	}
	template<class T>
	inline void Matrix3x3<T>::operator*=(const Matrix3x3<T>& aMatrix)
	{
		// Matrismultiplikation
		Matrix3x3<T> matrix;

		for (unsigned int row = 1; row < 4; row++)
		{
			for (unsigned int col = 1; col < 4; col++)
			{
				matrix(row, col) = 0;

				for (unsigned int i = 1; i < 4; i++)
				{
					matrix(row, col) += (*this)(row, i) * aMatrix(i, col);
				}
			}
		}

		(*this) = matrix;
	}

	// Vector3 * Matrix3x3 overloading
	template<class T>
	Vector3<T> operator*(const Vector3<T>& aLhsVector3, const Matrix3x3<T>& aRhsMatrix)
	{
		Vector3<T> vector;

		vector.x =
			  aLhsVector3.x * aRhsMatrix(1, 1)
			+ aLhsVector3.y * aRhsMatrix(2, 1)
			+ aLhsVector3.z * aRhsMatrix(3, 1);
		vector.y =
			  aLhsVector3.x * aRhsMatrix(1, 2)
			+ aLhsVector3.y * aRhsMatrix(2, 2)
			+ aLhsVector3.z * aRhsMatrix(3, 2);
		vector.z =
			  aLhsVector3.x * aRhsMatrix(1, 3)
			+ aLhsVector3.y * aRhsMatrix(2, 3)
			+ aLhsVector3.z * aRhsMatrix(3, 3);

		return vector;
	}
}