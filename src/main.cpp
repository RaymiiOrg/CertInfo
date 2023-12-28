#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>

#include "src/ca/caconcurrentgatherer.h"
#include "src/domainsources/browserhistorydb.h"
#include "src/domainsources/domainslisttextfile.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    app.setOrganizationName("Sparkling Network");
    app.setOrganizationDomain("raymii.org");
    app.setApplicationName("FF Cert Cleanup");
    app.setWindowIcon(QIcon(":/certinfo.png"));

    qmlRegisterType<BrowserHistoryDb>("org.raymii.BrowserHistoryDB", 1, 0, "BrowserHistoryDB");
    qmlRegisterType<DomainsListTextFile>("org.raymii.DomainsListTextFile", 1, 0, "DomainsListTextFile");
    qmlRegisterType<CAConcurrentGatherer>("org.raymii.CAConcurrentGatherer", 1, 0, "CAConcurrentGatherer");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
