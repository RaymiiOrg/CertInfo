QT += quick sql widgets quickcontrols2

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS = -Wno-deprecated-declarations

VERSION = 2024.01.1.0     # major.minor.patch.build
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
QMAKE_TARGET_PRODUCT = "CertInfo"
QMAKE_TARGET_COMPANY = "Sparkling Network"
QMAKE_TARGET_DESCRIPTION = "Which root certificates should you trust? by Remy van Elst, https://raymii.org"
QMAKE_TARGET_COPYRIGHT = "Remy van Elst - Licensed under the GNU GPLv3"

RC_ICONS = certinfo.ico

HEADERS += \
    src/ca/caconcurrentgatherer.h \
    src/ca/certificate.h \
    src/domainsources/browserhistorydb.h \
    src/listmodel/caissuerlistmodel.h \
    src/ca/caprocessor.h \
    src/listmodel/domaincountlistmodel.h \
    src/domainsources/domainslisttextfile.h \
    src/listmodel/genericlistmodel.h \
    src/listmodel/qabstractlistmodelwithrowcountsignal.h \
    src/versioncheck/versioncheck.h

SOURCES += \
        src/ca/caconcurrentgatherer.cpp \
        src/domainsources/browserhistorydb.cpp \
        src/listmodel/caissuerlistmodel.cpp \
        src/ca/caprocessor.cpp \
        src/listmodel/domaincountlistmodel.cpp \
        src/domainsources/domainslisttextfile.cpp \
        src/main.cpp \
        src/versioncheck/versioncheck.cpp

CONFIG += c++17

RESOURCES += src/qml.qrc 

contains(QT_MAJOR_VERSION, 6) {
    RESOURCES += src/qt6.qrc
}

contains(QT_MAJOR_VERSION, 5) {
    RESOURCES += src/qt5.qrc
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


include(src/thirdparty/SortFilterProxyModel/SortFilterProxyModel.pri)

DISTFILES += \
    certinfo.ico



