include(../SortedSequence.pri)

QT       += testlib
QT       -= gui

TARGET = tst_sortedsequencetest
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += SORTEDSEQUENCE_ENABLE_QHASH_WRAPPER
DEFINES += SORTEDSEQUENCE_ENABLE_STD_HASH_WRAPPER


SOURCES += tst_sortedsequencetest.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
