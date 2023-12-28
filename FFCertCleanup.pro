QT += quick sql

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS = -Wno-deprecated-declarations

SOURCES += \
        src/ca/caconcurrentgatherer.cpp \
        src/domainsources/browserhistorydb.cpp \
        src/listmodel/caissuerlistmodel.cpp \
        src/ca/caprocessor.cpp \
        src/listmodel/domaincountlistmodel.cpp \
        src/domainsources/domainslisttextfile.cpp \
        src/main.cpp

CONFIG += c++17

RESOURCES += src/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/ca/caconcurrentgatherer.h \
    src/ca/certificate.h \
    src/domainsources/browserhistorydb.h \
    src/listmodel/caissuerlistmodel.h \
    src/ca/caprocessor.h \
    src/listmodel/domaincountlistmodel.h \
    src/domainsources/domainslisttextfile.h \
    src/listmodel/genericlistmodel.h \
    src/listmodel/qabstractlistmodelwithrowcountsignal.h

include(src/thirdparty/SortFilterProxyModel/SortFilterProxyModel.pri)
