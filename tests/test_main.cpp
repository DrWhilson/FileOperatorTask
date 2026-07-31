#include <QApplication>
#include <QTest>

#include "test_scanfiles.h"
#include "test_formatsize.h"
#include "test_xorkey.h"
#include "test_processor.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    int status = 0;

    TestScanFiles t1;
    status |= QTest::qExec(&t1, argc, argv);

    TestFormatSize t2;
    status |= QTest::qExec(&t2, argc, argv);

    TestXorKey t3;
    status |= QTest::qExec(&t3, argc, argv);

    TestFileProcessor t4;
    status |= QTest::qExec(&t4, argc, argv);

    return status;
}
