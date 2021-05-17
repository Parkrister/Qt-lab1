#include <QtCore/QCoreApplication>
#include <QImage>
#include <string>
#include "Filter.h"
#include <iostream>

int main(int argc, char* argv[])
{
    srand(time(0));
    QCoreApplication a(argc, argv);
    std::string s;
    QImage img;

    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-p") && (i + 1 < argc)) {
            s = argv[i + 1];
        }
    }

    std::cout << s << std::endl;

    // ----- MatKernel ------ //
    char size[80];
    std::ifstream ifs("KernelM.txt");
    ifs.getline(size, 3, '\n');
    int sizei = std::atoi(size);
    Kernel MatKernel(sizei);
    std::unique_ptr<float[]> tmp = std::make_unique<float[]>(sizei * sizei);

    for (int i = 0; i < sizei * sizei; i++)
    {
        ifs.getline(size, 2, '\n');
        tmp[i] = std::atoi(size);
    }

    MatKernel.SetKernel(tmp.get(), sizei / 2);
    //--------------------------//

    img.load(QString(s.c_str()));
    //img.save("Images/Source.png");

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
    //sobel.process(img).save("Images_2/Sobel.png");

    GrayWorld grayWorld;
   //grayWorld.GrayWorld::process(img).save("Images_2/GrayWorld.png");

    Transfer transfer;
    //transfer.process(img).save("Images_2/Transfer.png");

    Glass glass;
    //glass.process(img).save("Images_2/Glass.png");

    LinealStretching linStr(img);
    //linStr.process(img).save("Images_2/LinealStretching.png");

    Dilation dilation;
    //dilation.process(img).save("Images_2/Dilation.png");

    Erosion erosion;
    //erosion.process(img).save("Images_2/Erosion.png");

    Opening opening;
    //opening.process(img).save("Images_2/Opening.png");

    Closing closing;
    //closing.process(img).save("Images_2/Closing.png");

    Grad grad;
    //grad.process(img).save("Images_2/Grad.png");

    Median median;
    //median.process(img).save("Images_2/Median.png");


    puts("done");
    return 0;
    return a.exec();
}