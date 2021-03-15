#include <QtCore/QCoreApplication>
#include <QImage>
#include <string>
#include "Filter.h"
#include <iostream>

int main(int argc, char *argv[])
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
    //img.save("Images/Source.png");

    InvertFilter invert;
    //invert.process(img).save("Images/Invert.png");

    BlurFilter blur;
   // blur.process(img).save("Images/Blur.png");

    GaussianFilter gaus;
    //gaus.process(img).save("Images/Gauss.png");

    GrayScale gs;
    //gs.process(img).save("Images/GrayScale.png");

    Sepia sep(10);
    //sep.process(img).save("Images/Sepia.png");

    Brighter br(50);
    br.process(img).save("Images/Brighter.png");

    puts("done");
    return a.exec();
}
