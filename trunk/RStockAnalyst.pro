TARGET = RStockAnalyst
TEMPLATE = app
DESTDIR = "../bin"

CONFIG += qt debug_and_release
CONFIG(debug,debug|release) {
	CONFIG += console
} else {
	DEFINES += QT_NO_DEBUG_OUTPUT
	CONFIG -= console
}

QT += core \
    gui \
	xml \
	script
	

PRECOMPILED_HEADER = StdAfx.h


HEADERS += StdAfx.h MainWindow.h \
			BaseWidget.h KLineWidget.h \
			MarketTrendWidget.h ColorBlockWidget.h \
			ColorManager.h BlockColorSettingDlg.h StockInfoWidget.h \
			SplashDlg.h CoordXBaseWidget.h BColorBlockWidget.h \
			resource.h BaseBlockWidget.h KeyWizard.h

SOURCES += main.cpp MainWindow.cpp \
			BaseWidget.cpp KLineWidget.cpp \
			MarketTrendWidget.cpp ColorBlockWidget.cpp \
			ColorManager.cpp BlockColorSettingDlg.cpp StockInfoWidget.cpp \
			SplashDlg.cpp CoordXBaseWidget.cpp BColorBlockWidget.cpp \
			BaseBlockWidget.cpp KeyWizard.cpp


RESOURCES += RStockAnalyst.qrc
RC_FILE = RStockAnalyst.rc

INCLUDEPATH += "../include/"
LIBS += -L"../bin/"
LIBS += -llua52 -lRStockData