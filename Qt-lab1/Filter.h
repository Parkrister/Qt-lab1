#pragma once
#include <QImage>

class Filter
{
protected:
	virtual QColor calcNewPixelColor(const QImage& img, int x, int y) const = 0;
public:
	virtual ~Filter() = default;
	virtual QImage process(const QImage& img) const;
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

class SobelKernel : public Kernel {
	using Kernel::Kernel;
	float Gx = 0, Gy = 0;
	SobelKernel(std::size_t radius = 1) : Kernel(radius) {
		int signed_radius = static_cast<int>(radius);
		for (int x = -signed_radius; x <= signed_radius; x++)
			for (int y = -signed_radius; y <= signed_radius; y++) {
				std::size_t idx = (x + radius) * getSize() + (y + radius);
				switch (idx)
				{
				case 0:
					Gx -= data[idx];
					Gy -= data[idx];
					break;
				case 1:
					Gx -= 2 * data[idx];
					break;
				case 2:
					Gx -= data[idx];
					Gy += data[idx];
					break;
				case 3:
					Gy -= 2 * data[idx];
					break;
				case 4:
					break;
				case 5:
					Gy += 2 * data[idx];
					break;
				case 6:
					Gx += data[idx];
					Gy -= data[idx];
					break;
				case 7:
					Gx += 2 * data[idx];
					break;
				case 8:
					Gx += data[idx];
					Gy += data[idx];
					break;
				default:
					break;
				}
			}
	}
};