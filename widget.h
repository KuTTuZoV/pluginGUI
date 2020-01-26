#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QList>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QXmlStreamReader>
#include <QDomDocument>
#include <QDomElement>
#include <QSpacerItem>
#include <QPixmap>
#include <QBuffer>
#include <QLibrary>

#include "../bw/bwPlugin/usefull.h"

#define QLABEL          0
#define QTEXTEDIT       1
#define QSLIDER         2
#define QPUSHBUTTON     3
#define QRADIOBUTTON    4
#define QCHECKBOX       5

#define PLUGIN_NAME 0
#define PARAMETERS  1

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE


class PluginGUI : public QWidget
{
    Q_OBJECT

    typedef void ( *filter )(unsigned char * image, unsigned char * result,  int width, int height, float value);
    typedef QByteArray ( *createPicture)(unsigned char * imageData, int fileSize, int imageWidth, int imageHeight);

public:
    void addElement(QString elementType, QString elementName, QString parameters);
    int getElementNumber(QString elementName);
    void readXML(QString xml);
    void applyFilter();
    void getPicture(char * picture);
    void setPicture(QByteArray data);
    void valueChanged(int value);

    PluginGUI(QString dllPath, QWidget *parent = nullptr);
    ~PluginGUI();

private:
    Ui::Widget *ui;

    QString dllPath;
    QVBoxLayout basicLayout;
    QLabel test;
    QList<QWidget*> elementList;
    QPixmap pixmap;
    QLabel * picture;
    unsigned char * original;
    BMPheader * header;
    char * padding;


    filter applyFilter_;
    createPicture createPicture_;

signals:
    void filtered(QByteArray image);

};
#endif // WIDGET_H
