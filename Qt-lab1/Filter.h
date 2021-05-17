#pragma once
#include <QImage>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <time.h>
#include <fstream>

class Filter
{
protected:
	virtual QColor calcNewPixelColor(const QImage& img, int x, int y) const = 0;
public:
	virtual ~Filter() = default;
	virtual QImage process(const QImage& img);
};

class InvertFilter : public Filter {
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

class Kernel {
protected:
	//данные ядра
	std::unique_ptr<float[]> data;
	//радиус ядра
	std::size_t radius;
	//размер памяти ядра
	std::size_t getLen() const { return getSize() * getSize(); }
public:
	//конструктор пустого ядра
	Kernel(std::size_t radius) : radius(radius) {
		//выделение памяти
		data = std::make_unique<float[]>(getLen());
	}
	//конструктор копирования
	Kernel(const Kernel& other) : Kernel(other.radius) {
		std::copy(other.data.get(), other.data.get() + getLen(), data.get());
	}
	
	// аксессоры
	std::size_t getRadius() const { return radius; }
	std::size_t getSize() const { return 2 * radius + 1; }
	float operator[] (std::size_t id) const { return data[id]; }
	float& operator[] (std::size_t id) { return data[id]; }

	void SetKernel(float* dataK, int rad)
	{
		radius = rad;
		std::size_t len = getLen();
		if (data)
		{
			data.reset();
		}
		data = std::make_unique<float[]>(len);
		std::copy(dataK, dataK + len, data.get());
	}

};

class MatrixFilter : public Filter {
protected:
	// хранится по значению, потому что внутри Kernel содержится указатель
	Kernel mKernel;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	MatrixFilter(const Kernel& kernel) : mKernel(kernel) {};
	virtual ~MatrixFilter() = default;
};

///-------- Фильтры ---------///
class BlurKernel : public Kernel {
public:
	using Kernel::Kernel;
	BlurKernel(std::size_t radius = 2) : Kernel(radius) {
		for (std::size_t i = 0; i < getLen(); i++)
			data[i] = 1.0f / getLen();
	}
};

class BlurFilter : public MatrixFilter {
public:
	BlurFilter(std::size_t radius = 1) : MatrixFilter(BlurKernel(radius)) {}
};

class GaussianKernel : public Kernel {
public:
	//Наследование конструкторов
	using Kernel::Kernel;
	GaussianKernel(std::size_t radius = 2, float sigma = 3.f) : Kernel(radius) {
		//коэффицент нормировки ядра
		float norm = 0;
		int signed_radius = static_cast<int>(radius);
		//расчитываем ядро линейного фильтра
		for(int x = -signed_radius; x <= signed_radius; x++)
			for (int y = -signed_radius; y <= signed_radius; y++) {
				std::size_t idx = (x + radius) * getSize() + (y + radius);
				data[idx] = std::exp(-(x * x + y * y) / (sigma * sigma));
				norm += data[idx];
			}
		//нормируем ядро
		for (std::size_t i = 0; i < getLen(); i++)
			data[i] /= norm;
	}
};

class GaussianFilter : public MatrixFilter {
public:
	GaussianFilter(std::size_t radius = 2, float sigma = 3.f) : MatrixFilter(GaussianKernel(radius, sigma)) {}
};

//---- Точечные фильтры ----//

class GrayScale : public Filter {
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

class Sepia : public Filter {
protected:
	float k;
public:
	Sepia(float mk = 1) : Filter() {
		k = mk;
	}
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
	float GetK() const { return k; }
};

class Brighter : public Filter {
protected:
	float k;
public:
	Brighter(float mk = 1) : Filter() {
		k = mk;
	}
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
	float GetK() const { return k; }
};

//---- Матричные фильтры ----//

// собель
class SobelKernel : public Kernel {
public:
	using Kernel::Kernel;
	SobelKernel(std::size_t radius = 1) : Kernel(radius) {
		//int Gx[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
		//int Gy[3][3] = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1} };
		//for (int i = 0; i < 9; i++) {
		//	data[i] = sqrt(pow(*(*Gx + i), 2) + pow(*(*Gy + i), 2));
		//}

		data[0] = -1; data[1] = 0; data[2] = 1;
		data[3] = -2; data[4] = 0; data[5] = 2;
		data[6] = -1; data[7] = 0; data[8] = 1;
	}
};

class SobelFilter : public MatrixFilter {
public:
	SobelFilter(std::size_t radius = 1) : MatrixFilter(SobelKernel(radius)) {}
};

// резкость
class SharpKernel : public Kernel {
public:
	using Kernel::Kernel;
	SharpKernel(std::size_t radius = 1) : Kernel(radius) {
		data[0] = 0; data[1] = -1; data[2] = 0;
		data[3] = -1; data[4] = 5; data[5] = -1;
		data[6] = 0; data[7] = -1; data[8] = 0;
	}
};

class SharpFilter : public MatrixFilter {
public:
	SharpFilter(std::size_t radius = 1) : MatrixFilter(SharpKernel(radius)) {}
};

// ----------------- GrayWorld -----------------//
class GrayWorld : public Filter {
public:
	float avg;
	int avgR;
	int avgG;
	int avgB;

	GrayWorld() : Filter() {
		avg = 0;
		avgR = 0;
		avgG = 0;
		avgB = 0;
	}
	QImage process(const QImage& img);
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};


class LinealStretching : public Filter
{
protected:
	float maxR, maxG, maxB;
	float minR, minG, minB;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	LinealStretching(const QImage& img)
	{
		float tmpR = 0, tmpG = 0, tmpB = 0;
		maxR = 0; maxG = 0; maxB = 0;
		minR = 255; minG = 255; minB = 255;
		for (int x = 0; x < img.width(); x++)
			for (int y = 0; y < img.height(); y++)
			{
				QColor color = img.pixelColor(x, y);
				tmpR = color.red();
				tmpG = color.green();
				tmpB = color.blue();
				if (tmpR > maxR)
					maxR = tmpR;
				if (tmpG > maxG)
					maxG = tmpG;
				if (tmpB > maxB)
					maxB = tmpB;
				if (tmpR < minR)
					minR = tmpR;
				if (tmpG < minG)
					minG = tmpG;
				if (tmpB < minB)
					minB = tmpB;
			}
	}
};

// ----------------- Transfer -----------------//
class Transfer : public Filter
{
protected:
	int x1, y1;
	
public:
	Transfer(int _x1 = 50, int _y1 = 0)
	{
		x1 = _x1;
		y1 = _y1;
	}
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
	QImage process(const QImage& img);
};

// ----------------- Glass -----------------//
class Glass : public Filter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

// ----------------- dilation, erosion, opening, closing-----------------//
class DilationKernel : public Kernel
{
public:
	using Kernel::Kernel;
	DilationKernel(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
			data[i] = 1;
	}
};

class ErosionKernel : public Kernel
{
public:
	using Kernel::Kernel;
	ErosionKernel(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
			data[i] = 1;
	}
};

class OpeningKernel : public Kernel
{
public:
	using Kernel::Kernel;
	OpeningKernel(std::size_t radius = 1) : Kernel(radius)
	{

		for (int i = 0; i < getLen(); i++)
			data[i] = 1;
	}
};

class ClosingKernel : public Kernel
{
public:
	using Kernel::Kernel;
	ClosingKernel(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
			data[i] = 1;
	}
};

class Dilation : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Dilation(std::size_t radius = 1) :MatrixFilter(DilationKernel(radius)) {}
	Dilation(Kernel& ker) : MatrixFilter(ker) {}
};

class Erosion : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Erosion(std::size_t radius = 1) : MatrixFilter(ErosionKernel(radius)) {}
	Erosion(Kernel& ker) : MatrixFilter(ker) {}
};

class Opening : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Opening(std::size_t radius = 1) : MatrixFilter(OpeningKernel(radius)) {}
	Opening(Kernel& ker) :MatrixFilter(ker) {}
	QImage process(const QImage& img) const;
};

class Closing : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Closing(std::size_t radius = 1) : MatrixFilter(ClosingKernel(radius)) {}
	Closing(Kernel& ker) :MatrixFilter(ker) {}
	QImage process(const QImage& img) const;
};



// --------------- Grad ---------------//
class GradKernel : public Kernel
{
public:
	using Kernel::Kernel;
	GradKernel(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
			data[i] = 1;
	}
};

class Grad : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Grad(std::size_t radius = 1) : MatrixFilter(GradKernel(radius)) {}
	Grad(Kernel& ker) : MatrixFilter(ker) {}
	QImage process(const QImage& img) const;
};

// --------------- Median ---------------//
class MedianKernel : public Kernel
{
public:
	using Kernel::Kernel;
	MedianKernel(std::size_t radius = 1) : Kernel(radius)
	{
		for (int i = 0; i < getLen(); i++)
			data[i] = 1;
	}
};

class Median : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	Median(std::size_t radius = 1) : MatrixFilter(MedianKernel(radius)) {}
};