#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PluginGUI w("G:/photoEditor/plugins/build-gaus-Desktop_Qt_5_12_6_MinGW_32_bit-Debug/debug/gaus.dll");

    QFile image("F:/lena512color.bmp");

    image.open(QIODevice::ReadOnly);

    QByteArray data = image.readAll();

    w.setPicture(data);

    QFile pluginDescription("pluginDescription.xml");
    pluginDescription.open(QIODevice::ReadOnly);
    //w.readXML(pluginDescription.readAll());
    pluginDescription.close();


    w.setGeometry(0,0,0,0);

    w.show();
    return a.exec();
}
