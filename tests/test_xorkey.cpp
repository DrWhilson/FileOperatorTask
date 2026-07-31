#include "test_xorkey.h"
#include "../settingsdialog.h"

#include <QTest>

TestXorKey::TestXorKey() = default;
TestXorKey::~TestXorKey() = default;

void TestXorKey::fullHex()
{
    SettingsDialog dlg("", "overwrite", "once", 5,
                       QByteArray::fromHex("1234567890ABCDEF"), false);
    QCOMPARE(dlg.xorKey(), QByteArray::fromHex("1234567890ABCDEF"));
}

void TestXorKey::allZeros()
{
    SettingsDialog dlg("", "overwrite", "once", 5, QByteArray(8, '\0'), false);
    QCOMPARE(dlg.xorKey(), QByteArray(8, '\0'));
}

void TestXorKey::shortHex()
{
    SettingsDialog dlg("", "overwrite", "once", 5, QByteArray::fromHex("FF"), false);
    QCOMPARE(dlg.xorKey(), QByteArray::fromHex("00000000000000FF"));
}

void TestXorKey::empty()
{
    SettingsDialog dlg("", "overwrite", "once", 5, QByteArray(), false);
    QCOMPARE(dlg.xorKey(), QByteArray(8, '\0'));
}

void TestXorKey::lowercase()
{
    SettingsDialog dlg("", "overwrite", "once", 5,
                       QByteArray::fromHex("abcdef0123456789"), false);
    QCOMPARE(dlg.xorKey(), QByteArray::fromHex("abcdef0123456789"));
}
