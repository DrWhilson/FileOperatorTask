#ifndef TEST_FORMATSIZE_H
#define TEST_FORMATSIZE_H

#include <QObject>

class TestFormatSize : public QObject
{
    Q_OBJECT

public:
    TestFormatSize();
    ~TestFormatSize() override;

private slots:
    void zero();
    void bytes();
    void kilobytes();
    void kbFraction();
    void megabytes();
    void mbFraction();
    void gigabytes();
    void gbFraction();
};

#endif
