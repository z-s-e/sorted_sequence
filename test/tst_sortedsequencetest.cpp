#include <QString>
#include <QtTest>
#include <QDebug>

#include <QList>
#include <QVarLengthArray>
#include <unordered_map>
#include <vector>

#include "sorted_sequence.h"

template class sorted_sequence::adaptor< QList<int> >;
template class sorted_sequence::adaptor< QVector<QString> >;
template class sorted_sequence::adaptor< QVarLengthArray<float> >;
template class sorted_sequence::adaptor< std::vector<unsigned> >;


template< typename T >
struct MyCompare {
    bool inverted = false;

    bool operator()(const T& x, const T& y) const
    { return inverted ? x > y : x < y; }
};

template< typename T >
bool operator==(const MyCompare<T>& c1, const MyCompare<T>& c2)
{
    return c1.inverted == c2.inverted;
}

template< typename T >
bool operator!=(const MyCompare<T>& c1, const MyCompare<T>& c2)
{
    return ! (c1 == c2);
}


template class sorted_sequence::adaptor< QList<int>, MyCompare<int> >;

using TestAdaptor = sorted_sequence::adaptor< QList<int> >;
using TestAdaptor2 = sorted_sequence::adaptor< QList<int>, MyCompare<int> >;

using TestLambdaCmp = sorted_sequence::lambda_compare<int>;
template class sorted_sequence::lambda_compare<int>;

using TestAdaptor3 = sorted_sequence::adaptor< std::vector<int>, TestLambdaCmp >;


class SortedSequenceTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testCase1();
    void testCase2();
    void testCase3();
    void testCase4();
    void testCase5();
    void testCase6();
    void testCase7();
};


void SortedSequenceTest::testCase1()
{
    TestAdaptor ta(QList<int>({3, 2, 1}));
    QCOMPARE(ta.container(), QList<int>({1, 2, 3}));
}

void SortedSequenceTest::testCase2()
{
    TestAdaptor ta;
    ta << 3;
    QCOMPARE(ta.insertPosition(2), 0);
    ta << 2;
    QCOMPARE(ta.insertPosition(1), 0);
    ta << 1;
    QCOMPARE(ta.insertPosition(4), 3);

    QCOMPARE(ta.container(), QList<int>({1, 2, 3}));
}

void SortedSequenceTest::testCase3()
{
    TestAdaptor ta(QList<int>({3, 2, 1}));

    for( int i = 0; i < 5; ++i ) {
        const int p1 = ta.insertPosition(i);
        const int p2 = ta.insertPosition(i, sorted_sequence::InsertFirst);
        for( int j = -3; j < 5; ++j ) {
            QVERIFY(ta.insertPosition(i, sorted_sequence::InsertLast, j) == p1);
            QVERIFY(ta.insertPosition(i, sorted_sequence::InsertFirst, j) == p2);
        }
    }
}

void SortedSequenceTest::testCase4()
{
    TestAdaptor ta;
    ta.insert(3);
    ta.insert(1);
    ta.insert(4);
    ta.insert(2);
    ta.insert(5);
    QCOMPARE(ta.insertPosition(3), 3);
    QCOMPARE(ta.insertPosition(3, sorted_sequence::InsertFirst), 2);
    ta.insert(3);

    QCOMPARE(ta.container(), QList<int>({1, 2, 3, 3, 4, 5}));
}

void SortedSequenceTest::testCase5()
{
    TestAdaptor ta(QList<int>({1, 2, 3}));
    ta.change(0, 55);
    QCOMPARE(ta.container(), QList<int>({2, 3, 55}));
    ta.change(1, 44);
    QCOMPARE(ta.container(), QList<int>({2, 44, 55}));
    ta.change(2, -1);
    QCOMPARE(ta.container(), QList<int>({-1, 2, 44}));
}

void SortedSequenceTest::testCase6()
{
    TestAdaptor2 ta(QList<int>({3, 2, 1}));
    QCOMPARE(ta.container(), QList<int>({1, 2, 3}));

    MyCompare<int> c;
    c.inverted = true;
    QHash<int, int> map{ {0, -1}, {1, -1}, {2, -1} };

    ta.setCompareOperatorGetReorderMap(c, &map);

    for( auto it = map.cbegin(), end = map.cend(); it != end; ++it ) {
        QVERIFY( it.key() == (ta.size() - *it - 1)) ;
    }

    QCOMPARE(ta.container(), QList<int>({3, 2, 1}));
}

void SortedSequenceTest::testCase7()
{
    auto f = [] (const int& x, const int& y) -> bool {
        if( x % 2 == 0 && y % 2 == 0 )
            return x > y;
        else if (x % 2 == 0)
            return true;
        else if (y % 2 == 0)
            return false;
        return x < y;
    };
    TestAdaptor3 ta( TestLambdaCmp{f} );

    ta << std::vector<int>({ 8, 6, 7, 5, 3, 4});

    QCOMPARE(ta.container(), std::vector<int>({8, 6, 4, 3, 5, 7}));

    std::unordered_map<std::ptrdiff_t, std::ptrdiff_t> map{ {0, -1}, {3, -1}, {4, -1} };
    ta.setCompareOperatorGetReorderMap(TestLambdaCmp([] (const int& x, const int& y) { return x < y; }),
                                       &map);
    QCOMPARE(ta.container(), std::vector<int>({3, 4, 5, 6, 7, 8}));
    QCOMPARE(map[0], 5l);
    QCOMPARE(map[3], 0l);
    QCOMPARE(map[4], 2l);
    QCOMPARE(map.size(), 3ul);

    TestAdaptor3 ta2(ta.compareOperator());
    QCOMPARE(ta.compareOperator(), ta2.compareOperator());
}


QTEST_APPLESS_MAIN(SortedSequenceTest)

#include "tst_sortedsequencetest.moc"
