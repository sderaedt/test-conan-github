#include <QApplication>
#include <QPushButton>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "helloworld_" + QLocale(locale).name();
        
        qDebug() << "Checking translation: " << baseName;
        if (translator.load(":/i18n/" + baseName)) {
            qDebug() << "Loaded: " << baseName;
            app.installTranslator(&translator);
            break;
        }
    }

    QPushButton hello(QPushButton::tr("Hello world!"));
    hello.resize(100, 30);

    hello.show();
    return app.exec();
}
