#include "Filter.h"

#define pi (3.14)

template <class T>
T clamp(T value, T max, T min) {
	if (value > max)
		return max;
	if (value < min)
		return min;
	return value;
}

QImage Filter::process(const QImage& img) {
	QImage result(img);

	for (int x = 0; x < img.width()/2; x++) 
		for (int y = 0; y < img.height(); y++) {
			QColor color = calcNewPixelColor(img, x, y);
			result.setPixelColor(x, y, color);
		}
	return result;
}

QColor InvertFilter::calcNewPixelColor(const QImage& img, int x, int y) const {
	QColor color = img.pixelColor(x, y);
	color.setRgb(255 - color.red(), 255 - color.green(), 255 - color.blue());
	return color;
}

QColor MatrixFilter::calcNewPixelColor(const QImage& img, int x, int y) const {
	float returnR = 0;
	float returnG = 0;
	float returnB = 0;
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	for(int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++) {
			int idx = (i + radius) * size + j + radius;
			QColor color = img.pixelColor(clamp(x + j, img.width() - 1, 0),
				clamp(y + 1, img.height() - 1, 0));

			returnR += color.red() * mKernel[idx];
			returnG += color.green() * mKernel[idx];
			returnB += color.blue() * mKernel[idx];
		}
	return QColor(clamp(returnR, 255.f, 0.f),
		clamp(returnG, 255.f, 0.f),
		clamp(returnB, 255.f, 0.f));
}

//////////
QColor GrayScale::calcNewPixelColor(const QImage& img, int x, int y) const {
	QColor color = img.pixelColor(x, y);
	float intensity = 0.299 * color.red() + 0.587 * color.green() + 0.144 * color.blue();
	color.setRgb(intensity, intensity, intensity);
	return color;
}

QColor Sepia::calcNewPixelColor(const QImage& img, int x, int y) const {
	QColor color = img.pixelColor(x, y);
	float k = GetK();
	float intensity = 0.299 * color.red() + 0.587 * color.green() + 0.144 * color.blue();
	color.setRgb(clamp(intensity + 2 * k, 255.f, 0.f),
		clamp(intensity + 0.5f * k, 255.f, 0.f),
		clamp(intensity - 1 * k, 255.f, 0.f));
	return color;
}

QColor Brighter::calcNewPixelColor(const QImage& img, int x, int y) const {
	QColor color = img.pixelColor(x, y);
	float k = GetK();
	color.setRgb(clamp(color.red() + k, 255.f, 0.f),
		clamp(color.green() + k, 255.f, 0.f),
		clamp(color.blue() + k, 255.f, 0.f));
	return color;
}

QImage GrayWorld::process(const QImage& img) {
	QImage result(img);
	int Size = img.width() * img.height();
	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++) {
			avgR += img.pixelColor(x, y).red();
			avgG += img.pixelColor(x, y).green();
			avgB += img.pixelColor(x, y).blue();
		}
	avgR = avgR / Size;
	avgG = avgG / Size;
	avgB = avgB / Size;
	avg = (avgR + avgG + avgB) / 3;

	for (int x = 0; x < img.width() / 2; x++)
		for (int y = 0; y < img.height(); y++) {
			QColor color = calcNewPixelColor(img, x, y);
			result.setPixelColor(x, y, color);
		}
	std::cout << avgR << ' ' << avgG << ' ' << avgB << '\n';
	return result;
}

QColor GrayWorld::calcNewPixelColor(const QImage& img, int x, int y) const {
	QColor color = img.pixelColor(x, y);
	color.setRgb((int)clamp((color.red()*avg/avgR), 255.f, 0.f),
		(int)(clamp(color.green()*avg/avgG, 255.f, 0.f)),
		(int)(color.blue()*avg/avgB, 255.f, 0.f));
	return color;
}

// линейное растяжение гистограммы
QImage AutoLevels::process(const QImage& img) {
	QImage result(img);
	int Size = img.width() * img.height();
	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++) {

			if (img.pixelColor(x, t).red() > maxR)
				maxR = img.pixelColor(x, t).red();
			if (img.pixelColor(x, t).red() < minR)
				minR = img.pixelColor(x, t).red();

			if (img.pixelColor(x, t).green() > maxG)
				maxG = img.pixelColor(x, t).green();
			if (img.pixelColor(x, t).green() < minG)
				minG = img.pixelColor(x, t).green();

			if (img.pixelColor(x, t).blue() > maxB)
				maxB = img.pixelColor(x, t).blue();
			if (img.pixelColor(x, t).blue() < minB)
				minB = img.pixelColor(x, t).blue();
		}

	for (int x = 0; x < img.width() / 2; x++)
		for (int y = 0; y < img.height(); y++) {
			QColor color = calcNewPixelColor(img, x, y);
			result.setPixelColor(x, y, color);
		}
	return result;
}

QColor AutoLevels::calcNewPixelColor(const QImage& img, int x, int y) const {
	QColor color = img.pixelColor(x, y);
	color.setRgb(clamp((color.red() - minR) * 255 / (maxR - minR), 255.f, 0.f),
		clamp((color.green() - minG) * 255 / (maxG - minG), 255.f, 0.f),
		clamp((color.blue() - minB) * 255 / (maxB - minB), 255.f, 0.f));
	return color;
}

// transfer
QColor Transfer::calcNewPixelColor(const QImage& img, int x, int y) const {
	QColor color;
	if (x > img.width() - 50) {
		color.setRgb(0, 0, 0);
		return color;
	}
	color = img.pixelColor(x + 50, y);
	return color;
}

// Wave
QColor Wave2::calcNewPixelColor(const QImage& img, int x, int y) const {
	int k = x + 20 * sin(2 * pi * x / 30);
	QColor color = img.pixelColor(k, y);
	return color;
}

