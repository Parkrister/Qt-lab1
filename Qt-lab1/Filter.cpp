#include "Filter.h"

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

// ----------------- GrayWorld ---------------------//
QImage GrayWorld::process(const QImage& img) {
	//QImage result(img);
	int Size = img.width() * img.height();
	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++) {
			avgR += img.pixelColor(x, y).red();
			avgG += img.pixelColor(x, y).green();
			avgB += img.pixelColor(x, y).blue();
		}
	avgR /= Size;
	avgG /= Size;
	avgB /= Size;
	avg = (avgR + avgG + avgB) / 3;

	/*
	for (int x = 0; x < img.width() / 2; x++)
		for (int y = 0; y < img.height(); y++) {
			QColor color = calcNewPixelColor(img, x, y);
			result.setPixelColor(x, y, color);
		}
	*/
	return Filter::process(img);
}

QColor GrayWorld::calcNewPixelColor(const QImage& img, int x, int y) const {
	QColor color = img.pixelColor(x, y);
	color.setRgb((int)clamp((color.red() * avg / avgR), 255.f, 0.f),
		(int)(clamp(color.green() * avg / avgG, 255.f, 0.f)),
		(int)(clamp(color.blue() * avg / avgB, 255.f, 0.f)));

	return color;
}

// ----------------- Transfer -----------------//
QColor Transfer::calcNewPixelColor(const QImage& img, int x, int y) const {
	QColor color;
	if (x + x1 < img.width() && y + y1 < img.height())
		color = img.pixelColor(x + x1, y + y1);
	else
		color.setRgb(0, 0, 0);
	return color;
}

QImage Transfer::process(const QImage& img) {
	QImage result(img);

	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++) {
			QColor color = Transfer::calcNewPixelColor(img, x, y);
			result.setPixelColor(x, y, color);
		}
	return result;
}

// ----------------- Glass -----------------//
QColor Glass::calcNewPixelColor(const QImage& img, int x, int y) const {
	QColor color;
	double r1 = (rand() % 11) / 10;
	double r2 = (rand() % 11) / 10;
	color = img.pixelColor(clamp(x + (r1 - 0.5) * 10, double(img.width() - 1), 0.0), 
							clamp(y + (r2 - 0.5) * 10, double(img.height() - 1), 0.0));
	return color;
}

// ----------------- LinealStretching -----------------//
QColor LinealStretching::calcNewPixelColor(const QImage& img, int x, int y) const {
	QColor color = img.pixelColor(x, y);
	color.setRgb(clamp(((color.red() - minR) * 255 / (maxR - minR)), 255.f, 0.f), clamp(((color.green() - minG) * 255 / (maxG - minG)), 255.f, 0.f), clamp(((color.blue() - minB) * 255 / (maxB - minB)), 255.f, 0.f));
	return color;
}

QColor Dilation::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 0, tmpR = 0;
	float returnG = 0, tmpG = 0;
	float returnB = 0, tmpB = 0;
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;
			QColor color = img.pixelColor(clamp(x + j, img.width() - 1, 0), clamp(y + i, img.height() - 1, 0));
			tmpR = color.red() * mKernel[idx];
			tmpG = color.green() * mKernel[idx];
			tmpB = color.blue() * mKernel[idx];
			if (tmpR > returnR)
				returnR = tmpR;
			if (tmpG > returnG)
				returnG = tmpG;
			if (tmpB > returnB)
				returnB = tmpB;
		}
	return QColor(clamp(returnR, 255.f, 0.f), clamp(returnG, 255.f, 0.f), clamp(returnB, 255.f, 0.f));
}

QColor Erosion::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 255, tmpR = 0;
	float returnG = 255, tmpG = 0;
	float returnB = 255, tmpB = 0;
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;
			QColor color = img.pixelColor(clamp(x + j, img.width() - 1, 0), clamp(y + i, img.height() - 1, 0));
			tmpR = color.red() * mKernel[idx];
			tmpG = color.green() * mKernel[idx];
			tmpB = color.blue() * mKernel[idx];
			if (tmpR < returnR)
				returnR = tmpR;
			if (tmpG < returnG)
				returnG = tmpG;
			if (tmpB < returnB)
				returnB = tmpB;
		}
	return QColor(clamp(returnR, 255.f, 0.f), clamp(returnG, 255.f, 0.f), clamp(returnB, 255.f, 0.f));
}

QColor Opening::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor a;
	return a;
}

QColor Closing::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor a;
	return a;
}

QImage Opening::process(const QImage& img) const
{
	QImage result;
	int rad = mKernel.getRadius();
	Dilation dil(rad);
	result = dil.process(img);
	Erosion eros(rad);
	result = eros.process(result);
	return result;
}

QImage Closing::process(const QImage& img) const
{
	QImage result;
	int rad = mKernel.getRadius();
	Erosion eros(rad);
	result = eros.process(img);
	Dilation dil(rad);
	result = dil.process(result);
	return result;
}

// ---------------- Grad ----------------- //
QImage Grad::process(const QImage& img) const
{
	QImage tmp1, tmp2, result(img);
	int rad = mKernel.getRadius();
	Dilation dil(rad);
	tmp1 = dil.process(img);
	Erosion eros(rad);
	tmp2 = eros.process(img);

	for (int x = 0; x < tmp1.width(); x++)
		for (int y = 0; y < tmp2.height(); y++)
		{
			QColor color1 = tmp1.pixelColor(x, y);
			QColor color2 = tmp2.pixelColor(x, y);
			QColor res;
			res.setRgb(clamp(color1.red() - color2.red(), 255, 0), clamp(color1.green() - color2.green(), 255, 0), clamp(color1.blue() - color2.blue(), 255, 0));
			result.setPixelColor(x, y, res);
		}

	return result;
}

QColor Grad::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor a;
	return a;
}

//----------- Median ---------------//
QColor Median::calcNewPixelColor(const QImage& img, int x, int y) const
{
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	int* masR, * masG, * masB;
	masR = new int[size * size];
	masG = new int[size * size];
	masB = new int[size * size];

	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;
			QColor color = img.pixelColor(clamp(x + j, img.width() - 1, 0), clamp(y + i, img.height() - 1, 0));
			masR[idx] = color.red();
			masG[idx] = color.green();
			masB[idx] = color.blue();
		}

	std::sort(masR, masR + size * size);
	std::sort(masG, masG + size * size);
	std::sort(masB, masB + size * size);

	QColor col;
	col.setRgb(clamp(masR[size * size / 2], 255, 0), clamp(masG[size * size / 2], 255, 0), clamp(masB[size * size / 2], 255, 0));

	return col;
}