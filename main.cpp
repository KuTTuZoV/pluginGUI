#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PluginGUI w("G:/photoEditor/plugins/build-median-Desktop_Qt_5_12_6_MinGW_32_bit-Debug/median.dll");

    QFile pluginDescription("pluginDescription.xml");
    pluginDescription.open(QIODevice::ReadOnly);
    w.readXML(pluginDescription.readAll());
    pluginDescription.close();


    w.setGeometry(0,0,0,0);

    w.show();
    return a.exec();
}
