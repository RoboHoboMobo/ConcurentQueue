QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

prefix = $$PWD/../Sources

INCLUDEPATH += $$prefix

SOURCES += tst_concurentqueue.cpp

HEADERS += $$prefix/ConcurentQueue.h \
    TestContainer.h
