#include "widget.h"
#include "ui_widget.h"


int PluginGUI::getElementNumber(QString elementName)
{
    if(elementName == "QLabel")         return QLABEL;
    if(elementName == "QTextEdit")      return QTEXTEDIT;
    if(elementName == "QSlider")        return QSLIDER;
    if(elementName == "QPushButton")    return QPUSHBUTTON;
    if(elementName == "QRadioButton")   return QRADIOBUTTON;
    if(elementName == "QCheckBox")      return QCHECKBOX;
}

void PluginGUI::readXML(QString xml)
{
    QDomDocument doc("mydocument");
    doc.setContent(xml);

    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomElement docElem = doc.documentElement();

    QDomElement name = doc.childNodes().at(0).childNodes().at(PLUGIN_NAME).toElement();
    QString pluginName = name.text();

    this->setWindowTitle(pluginName);

    QDomNode parameter = doc.childNodes().at(0).childNodes().at(PARAMETERS).firstChild();

    while(!parameter.isNull()) {
        QDomElement e = parameter.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            QString parameterName = e.tagName();
            qDebug(parameterName.toUtf8().data());

            QString parameterElement  = e.childNodes().at(0).toElement().tagName();
            QString parameterDescript = e.childNodes().at(0).toElement().text();

            this->addElement(parameterElement, parameterName, parameterDescript);
        }
        parameter = parameter.nextSibling();
    }

    QSlider* slider = this->findChild<QSlider*>("slider");

    connect(slider, &QSlider::valueChanged, this, &PluginGUI::valueChanged);

    QSpacerItem * spacer = new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Expanding);
    this->basicLayout.addSpacerItem(spacer);
}

void PluginGUI::applyFilter()
{
    bool res = false;
    unsigned char * resImage = new unsigned char[512*512*3];

    QSlider* slider = this->findChild<QSlider*>("slider");

    float value_ = slider->value() / 100.0;
    applyFilter_(original, resImage, 512 * 3, 512, value_);

    QByteArray data = createPicture_(resImage, 786486, 512, 512);

    res = pixmap.loadFromData(data);
    picture->setPixmap(pixmap);

    filtered(data);
}

void PluginGUI::getPicture(char *picture)
{
    QLibrary lib(dllPath);

    if(!lib.load()) {
        int a = 5;
    }

    typedef QByteArray ( *createPicture)(unsigned char * imageData, int fileSize, int imageWidth, int imageHeight);
        createPicture createPicture_ = (createPicture) lib.resolve("createPicture");
        if(createPicture_) {}

    QByteArray data = createPicture_(original, 786486, 512, 512);

    memcpy(picture, data.data(), data.count());
}

void PluginGUI::setPicture(QByteArray data)
{
    bool res = pixmap.loadFromData(data);

    picture->setPixmap(pixmap);

    header = (BMPheader *)data.data();
    original = new unsigned char[512*512*3];

    memcpy(original, data.data() + header->bfOffBits, 512*512*3);
}

void PluginGUI::valueChanged(int value)
{
    bool res = false;
    unsigned char * resImage = new unsigned char[512*512*3];

    QSlider* slider = this->findChild<QSlider*>("slider");

    float value_ = slider->value() / 100.0;
    applyFilter_(original, resImage, 512 * 3, 512, value_);

    QByteArray data = createPicture_(resImage, 786486, 512, 512);

    res = pixmap.loadFromData(data);
    picture->setPixmap(pixmap);
}


void PluginGUI::addElement(QString elementType, QString elementName, QString parameters)
{
    switch (getElementNumber(elementType)) {
        case QLABEL: {
            QLabel * temp = new QLabel(elementName);
            //QStringList params = parameters.split(";");
            elementList.append(temp);
            this->basicLayout.addWidget(temp);
        }break;

        case QTEXTEDIT: {
            QTextEdit * temp = new QTextEdit(elementName);
            elementList.append(temp);
            this->basicLayout.addWidget(temp);
        }break;

        case QSLIDER: {
            QLabel  * tempLabel = new QLabel(elementName);
            elementList.append(tempLabel);

            QSlider * temp = new QSlider(Qt::Orientation::Horizontal);

            temp->setMinimum(parameters.split(";")[1].toInt());
            temp->setMaximum(parameters.split(";")[2].toInt());

            temp->setObjectName("slider");
            elementList.append(temp);

            this->basicLayout.addWidget(tempLabel);
            this->basicLayout.addWidget(temp);
        }break;

        case QPUSHBUTTON: {
            QPushButton * temp = new QPushButton(elementName);
            elementList.append(temp);
            connect(temp, &QPushButton::clicked, this, &PluginGUI::applyFilter);
            this->basicLayout.addWidget(temp);
        }break;

        case QRADIOBUTTON: {
            QRadioButton * temp = new QRadioButton(elementName);
            elementList.append(temp);
            this->basicLayout.addWidget(temp);
        }break;

        case QCHECKBOX: {
            QCheckBox * temp = new QCheckBox(elementName);
            elementList.append(temp);
            this->basicLayout.addWidget(temp);
        }break;
    }
    //this->basicLayout.addWidget(element);
}

PluginGUI::PluginGUI(QString dllPath, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setLayout(&this->basicLayout);
    this->dllPath = dllPath;
    picture = new QLabel("");

    QLibrary lib(dllPath);
    QFile pluginDescription("pluginDescription.xml");

    this->basicLayout.addWidget(picture);

    if(!lib.load()) {
        this->setWindowTitle("Библиотека не загружена");
        return;
    }

    if(!pluginDescription.open(QIODevice::ReadOnly)) {
        this->setWindowTitle(this->windowTitle() + " Файл с описанием на загружен");
        return;
    }

    readXML(pluginDescription.readAll());

    applyFilter_ = (filter) lib.resolve("filter");
    createPicture_ = (createPicture) lib.resolve("createPicture");
}

PluginGUI::~PluginGUI()
{
    delete ui;
}

