#include <string>
#include <iostream>
#include <QImage>
#include "Filter.h"



using namespace std;
void main(int argc, char* argv[]) {
	string s;
	QImage img;
	for (int i = 0; i < argc; ++i) {
		if (!strcmp(argv[i], "-p") && (i + 1 < argc)) {
			s = argv[i + 1];
		}
	}
	img.load(QString(s.c_str()));
	img.save("C:/Images/Source.png");
	InvertFilter invert;
	invert.process(img).save("C:/Images/Invert.png");

	BlurFilter blur;
	blur.process(img).save("C:/Images/Blur.png");

	GaussianFilter Gaussian;
	Gaussian.process(img).save("C:/Images/Gaussian.png");

	GrayScaleFilter grayScale;
	grayScale.process(img).save("C:/Images/GrayScale.png");

	SepiaFilter sepia;
	sepia.process(img).save("C:/Images/Sepia.png");

	BrightFilter bright;
	bright.process(img).save("C:/Images/Bright.png");

	DefaultFilter sobel;
	sobel.processSobel(img).save("C:/Images/Sobel.png");

	Contr1Filter contr1;
	contr1.process(img).save("C:/Images/Contr1.png");

	DefaultFilter ideal;
	ideal.processIdeal(img).save("C:/Images/IdealMirror.png");

	DefaultFilter gist;
	gist.processGist(img).save("C:/Images/Gist.png");

	DefaultFilter erosion;
	erosion.processErosion(img).save("C:/Images/Erosion.png");

	DefaultFilter dilation;
	dilation.processDilation(img).save("C:/Images/Dialtion.png");

	DefaultFilter grad;
	grad.processGrad(img).save("C:/Images/Grad.png");

	DefaultFilter closing;
	closing.processClosing(img).save("C:/Images/Closing.png");

	DefaultFilter opening;
	opening.processOpening(img).save("C:/Images/Opening.png");

	PerenosFilter perenos;
	perenos.process(img).save("C:/Images/Perenos.png");

	WaveFilter wave;
	wave.process(img).save("C:/Images/Wave.png");

	Contr2Filter contr2;
	contr2.process(img).save("C:/Images/Contr2.png");

	MotionBlurFilter MotionBlur;
	MotionBlur.process(img).save("C:/Images/MotionBlur.png");

	MedianFilter Median;
	Median.process(img).save("C:/Images/Median.png");


}