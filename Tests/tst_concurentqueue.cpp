#include <QtTest>

#include "ConcurentQueue.h"
#include "TestContainer.h"

struct TestStruct;

class ConcurentQueueTest : public QObject
{
    Q_OBJECT

public:
    ConcurentQueueTest();
    ~ConcurentQueueTest();

private slots:
    void testEmplace();
    void tryPopReturnSharedPtrIfEmpty();
    void tryPopReferenceIfEmpty();
    void testPushConstLRef();
    void testPushRvalValRef();
    void testEmpty();
    void testSize();
    void testWaitAndPopReturnSharedPtr();
    void testWaitAndPopWithReference();
};

ConcurentQueueTest::ConcurentQueueTest()
{
}

ConcurentQueueTest::~ConcurentQueueTest()
{
}

void ConcurentQueueTest::testEmplace()
{
    ConcurentQueue<int> cq0;
    cq0.emplace(1);
    cq0.emplace(2);
    cq0.emplace(3);

    QCOMPARE(cq0.size(), 3u);

    ConcurentQueue<TestStruct> cq1;
    cq1.emplace(1, 2.0, "test");

    QCOMPARE(cq1.size(), 1u);
}

void ConcurentQueueTest::tryPopReturnSharedPtrIfEmpty()
{
    ConcurentQueue<int> cq;
    auto result = cq.tryPop();

    QVERIFY(!result.get());
}

void ConcurentQueueTest::tryPopReferenceIfEmpty()
{
    ConcurentQueue<int> cq;

    int value{123};

    const bool result = cq.tryPop(value);

    QVERIFY(!result);
    QCOMPARE(value, 123);
}

void ConcurentQueueTest::testPushConstLRef()
{
    TestContainer<int>::clear();

    ConcurentQueue<int, TestContainer<int>> cq;

    std::vector<int> v{3, 2, 1};

    cq.push(v.at(0));
    cq.push(v.at(1));
    cq.push(v.at(2));

    auto result = TestContainer<int>::data;

    QCOMPARE(result.size(), 3u);
    QCOMPARE(result, std::deque<int>(v.begin(), v.begin() + 3));
}

void ConcurentQueueTest::testPushRvalValRef()
{
    TestContainer<int>::clear();

    ConcurentQueue<int, TestContainer<int>> cq;

    cq.push(3);
    cq.push(2);
    cq.push(1);

    auto result = TestContainer<int>::data;

    QCOMPARE(result.size(), 3u);
    QCOMPARE(result.at(0), 3);
    QCOMPARE(result.at(1), 2);
    QCOMPARE(result.at(2), 1);
}

void ConcurentQueueTest::testEmpty()
{
    TestContainer<int>::clear();
    TestContainer<int> tc;

    ConcurentQueue<int, TestContainer<int>> cq;

    QVERIFY(cq.empty());

    cq.push(42);

    QVERIFY(!cq.empty());
    QCOMPARE(tc.size(), 1u);
}

void ConcurentQueueTest::testSize()
{
    TestContainer<int>::clear();
    TestContainer<int> tc;

    ConcurentQueue<int, TestContainer<int>> cq;

    QCOMPARE(cq.size(), 0u);

    cq.push(42);

    QCOMPARE(cq.size(), 1u);
    QCOMPARE(tc.size(), 1u);

    cq.push(3);
    cq.push(2);
    cq.push(1);

    QCOMPARE(cq.size(), 4u);
    QCOMPARE(tc.size(), 4u);
}

void ConcurentQueueTest::testWaitAndPopReturnSharedPtr()
{
    ConcurentQueue<int> cq;

    auto f = static_cast<std::shared_ptr<int>(ConcurentQueue<int>::*)()>(
                &ConcurentQueue<int>::waitAndPop);

    std::future<std::shared_ptr<int>> result =
            std::async(std::launch::async, f, std::ref(cq));

    cq.push(42);

    auto value = result.get();

    QVERIFY(value);
    QCOMPARE(*value, 42);
}

void ConcurentQueueTest::testWaitAndPopWithReference()
{
    ConcurentQueue<int> cq;

    auto f = static_cast<void(ConcurentQueue<int>::*)(int&)>(
                &ConcurentQueue<int>::waitAndPop);

    int result{};

    auto future = std::async(std::launch::async, f, std::ref(cq), std::ref(result));

    cq.push(42);

    future.get();

    QCOMPARE(result, 42);
}

struct TestStruct
{
    TestStruct(int i, double d, std::string s) : i{i}, d{d}, s{s} {}

    int i;
    double d;
    std::string s;
};

QTEST_APPLESS_MAIN(ConcurentQueueTest)

#include "tst_concurentqueue.moc"
