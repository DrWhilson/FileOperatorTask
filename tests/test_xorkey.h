#ifndef TEST_XORKEY_H
#define TEST_XORKEY_H

#include <QObject>

class TestXorKey : public QObject
{
    Q_OBJECT

public:
    TestXorKey();
    ~TestXorKey() override;

private slots:
    void fullHex();
    void allZeros();
    void shortHex();
    void empty();
    void lowercase();
};

#endif
