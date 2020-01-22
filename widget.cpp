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

    picture = new QLabel("");

    QString file = "F:/lena512color.bmp";

    pixmap.load(file);
    picture->setPixmap(pixmap);


    QFile image("F:/lena512color.bmp");
    image.open(QIODevice::ReadOnly);
    //Чтение заголовка файла
    QByteArray data = image.read(sizeof(BMPheader));

    header = (BMPheader *)data.data();
    original = new unsigned char[header->biSizeImage];

    //Сохранение значений ширины и высоты записаных в заголовке
    int imgWidth = header->biWidth;
    int imgHeight = header->biHeight;

    int bytesCount = imgWidth * imgHeight * header->biBitCount / 8;

    padding = new char[header->bfOffBits - sizeof(BMPheader)];
    image.read(padding, header->bfOffBits - sizeof(BMPheader));
    image.read((char*)original, header->biSizeImage);

    image.close();

    this->basicLayout.addWidget(picture);

    QSpacerItem * spacer = new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Expanding);
    this->basicLayout.addSpacerItem(spacer);
}

void PluginGUI::applyFilter()
{
    bool res = false;
    unsigned char * resImage = new unsigned char[512*512*3];

    QSlider* slider = this->findChild<QSlider*>("slider");

    QLibrary lib(dllPath);

    if(!lib.load()) {
        int a = 5;
    }

    typedef void ( *filter )(unsigned char * image, unsigned char * result,  int width, int height, float value);
        filter applyFilter = (filter) lib.resolve("filter");
        if( applyFilter ) {}

    applyFilter(original, resImage, 512 * 3, 512, 5);

    typedef QByteArray ( *createPicture)(unsigned char * imageData, int fileSize, int imageWidth, int imageHeight);
        createPicture createPicture_ = (createPicture) lib.resolve("createPicture");
        if(createPicture_) {}

    QByteArray data = createPicture_(resImage, 786486, 512, 512);

    res = pixmap.loadFromData(data);
    res = pixmap.save("G:/temp.bmp");
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

            temp->setMinimum(0);
            temp->setMaximum(100);

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
}

PluginGUI::~PluginGUI()
{
    delete ui;
}

