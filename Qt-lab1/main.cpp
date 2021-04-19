#include <QtCore/QCoreApplication>
#include <QImage>
#include <string>
#include "Filter.h"
#include <iostream>

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    std::string s;
    QImage img;

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-p") && (i + 1 < argc)) {
            s = argv[i + 1];
        }
    }

    std::cout << s << std::endl;

    img.load(QString(s.c_str()));
    img.save("Images/Source.png");

    InvertFilter invert;
    //invert.process(img).save("Images_2/Invert.png");

    BlurFilter blur;
     //blur.process(img).save("Images_2/Blur.png");

    GaussianFilter gaus;
    //gaus.process(img).save("Images_2/Gauss.png");

    GrayScale gs;
    //gs.process(img).save("Images_2/GrayScale.png");

    Sepia sep(10);
    //sep.process(img).save("Images_2/Sepia.png");

    Brighter br(50);
    //br.process(img).save("Images_2/Brighter.png");

    SharpFilter sh;
    //sh.process(img).save("Images_2/Sharpness.png");

    SobelFilter sobel;
    sobel.process(img).save("Images_2/Sobel.png");

    GrayWorld grayWorld;
    grayWorld.GrayWorld::process(img).save("Images_2/GrayWorld.png");

    puts("done");
    return 0;
    return a.exec();
}