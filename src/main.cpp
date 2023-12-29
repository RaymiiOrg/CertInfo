/*
 * Copyright (c) 2023 Remy van Elst https://raymii.org
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */



#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QDebug>
#include "src/ca/caconcurrentgatherer.h"
#include "src/domainsources/browserhistorydb.h"
#include "src/domainsources/domainslisttextfile.h"


void destroy(QObject*);


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

    QQmlApplicationEngine* engine = new QQmlApplicationEngine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine->load(url);

    int appExecResult = app.exec();

    destroy(engine);

    return appExecResult;
}

void destroy(QObject* object)
{
    QEventLoop eventloop;
    QObject::connect(object, &QObject::destroyed, &eventloop, &QEventLoop::quit);
    object->deleteLater();
    eventloop.exec();
}
