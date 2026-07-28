#include <QTest>

#include "test_scanfiles.h"

int main(int argc, char *argv[])
{
    int status = 0;

    TestScanFiles t1;
    status |= QTest::qExec(&t1, argc, argv);

    return status;
}
