#include "test_formatsize.h"
#include "../mainwindow.h"

#include <QTest>

TestFormatSize::TestFormatSize() = default;
TestFormatSize::~TestFormatSize() = default;

void TestFormatSize::zero()
{
    QCOMPARE(MainWindow::formatSize(0), QString("0 Б"));
}

void TestFormatSize::bytes()
{
    QCOMPARE(MainWindow::formatSize(1023), QString("1023 Б"));
}

void TestFormatSize::kilobytes()
{
    QCOMPARE(MainWindow::formatSize(1024), QString("1.0 КБ"));
}

void TestFormatSize::kbFraction()
{
    QCOMPARE(MainWindow::formatSize(1536), QString("1.5 КБ"));
}

void TestFormatSize::megabytes()
{
    QCOMPARE(MainWindow::formatSize(1024 * 1024), QString("1.0 МБ"));
}

void TestFormatSize::mbFraction()
{
    QCOMPARE(MainWindow::formatSize(1536 * 1024), QString("1.5 МБ"));
}

void TestFormatSize::gigabytes()
{
    QCOMPARE(MainWindow::formatSize(1024LL * 1024 * 1024), QString("1.00 ГБ"));
}

void TestFormatSize::gbFraction()
{
    QCOMPARE(MainWindow::formatSize(1536LL * 1024 * 1024), QString("1.50 ГБ"));
}
