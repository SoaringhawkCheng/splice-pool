#include <map>
#include <stack>

#include "splice-pool.hpp"
#include "gtest/gtest.h"

namespace
{
    const std::vector<int> values{3, 1, 4, 1, 5, 9};

    std::vector<splicer::Node<int>> makeNodes(std::vector<int> v = values)
    {
        std::vector<splicer::Node<int>> nodes(v.size());

        for (std::size_t i(0); i < v.size(); ++i)
        {
            *nodes[i] = v[i];
        }

        return nodes;
    }

    splicer::Stack<int> makeStack(std::vector<splicer::Node<int>>& nodes)
    {
        splicer::Stack<int> stack;

        for (std::size_t i(0); i < nodes.size(); ++i)
        {
            stack.push(&nodes[i]);
        }

        return stack;
    }
}

TEST(Stack, PopEmpty)
{
    splicer::Stack<int> stack;
    splicer::Node<int>* node(nullptr);

    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.size(), 0);
    EXPECT_NO_THROW(node = stack.pop());

    EXPECT_EQ(node, nullptr);
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.size(), 0);
}

TEST(Stack, PushPopNode)
{
    splicer::Stack<int> stack;

    const int value(4);

    splicer::Node<int> node;
    *node = value;

    EXPECT_TRUE(stack.empty());

    stack.push(&node);
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 1);

    splicer::Node<int>* popped(stack.pop());
    ASSERT_TRUE(popped);
    EXPECT_EQ(**popped, value);
    EXPECT_FALSE(popped->next());
    EXPECT_TRUE(stack.empty());
}

TEST(Stack, PushBack)
{
    std::vector<splicer::Node<int>> nodes(
            makeNodes(std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }));

    splicer::Stack<int> stack;
    splicer::Stack<int> other;

    std::size_t i(0);
    for (auto& node : nodes)
    {
        if (i < nodes.size() / 2) stack.pushBack(&node);
        else other.pushBack(&node);

        ++i;
    }

    i = 0;
    EXPECT_EQ(stack.size(), 5);
    EXPECT_EQ(other.size(), 5);

    for (const int v : stack)
    {
        ASSERT_EQ(v, i);
        ++i;
    }

    for (const int v : other)
    {
        ASSERT_EQ(v, i);
        ++i;
    }

    stack.pushBack(other);

    i = 0;
    EXPECT_EQ(stack.size(), 10);
    EXPECT_EQ(other.size(), 0);
    EXPECT_TRUE(other.empty());

    for (const int v : stack)
    {
        ASSERT_EQ(v, i);
        ++i;
    }
}

TEST(Stack, Swap)
{
    std::vector<splicer::Node<int>> nodes(makeNodes());
    splicer::Stack<int> stack(makeStack(nodes));

    splicer::Stack<int> other;

    EXPECT_EQ(stack.size(), values.size());
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(other.size(), 0);
    EXPECT_TRUE(other.empty());

    stack.swap(other);

    EXPECT_EQ(stack.size(), 0);
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(other.size(), values.size());
    EXPECT_FALSE(other.empty());

    for (std::size_t i(0); i < values.size(); ++i)
    {
        ASSERT_EQ(other.pop()->val(), values[values.size() - i - 1]);
    }

    ASSERT_EQ(other.size(), 0);
    ASSERT_TRUE(other.empty());
}

TEST(Stack, PushStack)
{
    splicer::Stack<int> stack;
    splicer::Stack<int> other;

    const int value(4);

    splicer::Node<int> node;
    node.val() = value;

    other.push(&node);
    EXPECT_FALSE(other.empty());
    EXPECT_EQ(other.size(), 1);

    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.size(), 0);

    stack.push(other);
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 1);
    EXPECT_TRUE(other.empty());
    EXPECT_EQ(other.size(), 0);

    ASSERT_EQ(stack.pop()->val(), value);
    EXPECT_TRUE(stack.empty());
    EXPECT_TRUE(other.empty());
    EXPECT_EQ(stack.size(), 0);
    EXPECT_EQ(other.size(), 0);
}

TEST(Stack, PopStackEmpty)
{
    splicer::Stack<int> stack;
    splicer::Stack<int> other;

    EXPECT_NO_THROW(other = stack.popStack(1));
    EXPECT_TRUE(stack.empty());
    EXPECT_TRUE(other.empty());

    EXPECT_NO_THROW(other = stack.popStack(0));
    EXPECT_TRUE(stack.empty());
    EXPECT_TRUE(other.empty());
}

TEST(Stack, PopStackZero)
{
    std::vector<splicer::Node<int>> nodes(makeNodes());
    splicer::Stack<int> stack(makeStack(nodes));

    const std::size_t total(values.size());

    splicer::Stack<int> other(stack.popStack(0));

    ASSERT_EQ(stack.size(), total);
    ASSERT_FALSE(stack.empty());

    ASSERT_EQ(other.size(), 0);
    ASSERT_TRUE(other.empty());
}

TEST(Stack, PopStackPartial)
{
    std::vector<splicer::Node<int>> nodes(makeNodes());
    splicer::Stack<int> stack(makeStack(nodes));

    const std::size_t total(values.size());

    splicer::Stack<int> other(stack.popStack(2));

    EXPECT_EQ(stack.size(), total - 2);
    EXPECT_EQ(other.size(), 2);

    std::size_t i(total);

    while (--i < total)
    {
        const auto check(values[i]);

        if (i >= 4) ASSERT_EQ(other.pop()->val(), check);
        else ASSERT_EQ(stack.pop()->val(), check);
    }

    ASSERT_TRUE(stack.empty());
    ASSERT_TRUE(other.empty());
}

TEST(Stack, PopStackFull)
{
    std::vector<splicer::Node<int>> nodes(makeNodes());
    splicer::Stack<int> stack(makeStack(nodes));

    const std::size_t total(values.size());

    ASSERT_EQ(stack.size(), total);

    splicer::Stack<int> other(stack.popStack(total));

    EXPECT_EQ(stack.size(), 0);
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(other.size(), total);
    EXPECT_FALSE(other.empty());

    for (std::size_t i(total - 1); i < total; --i)
    {
        ASSERT_EQ(other.pop()->val(), values[i]);
    }

    EXPECT_TRUE(other.empty());
}

TEST(Stack, PopStackTooMany)
{
    std::vector<splicer::Node<int>> nodes(makeNodes());
    splicer::Stack<int> stack(makeStack(nodes));

    const std::size_t total(values.size());

    ASSERT_EQ(stack.size(), total);

    splicer::Stack<int> other(stack.popStack(total * 2));

    EXPECT_EQ(stack.size(), 0);
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(other.size(), total);
    EXPECT_FALSE(other.empty());

    for (std::size_t i(total - 1); i < total; --i)
    {
        ASSERT_EQ(other.pop()->val(), values[i]);
    }

    EXPECT_TRUE(other.empty());
}

TEST(Stack, PushPopSingle)
{
    std::stack<int> validator;
    splicer::Stack<int> stack;
    std::vector<splicer::Node<int>> nodes(makeNodes());

    auto doPush([&](std::size_t i)->void
    {
        const int value(values.at(i));
        splicer::Node<int>& node(nodes.at(i));

        ASSERT_EQ(value, node.val());

        validator.push(value);
        stack.push(&node);

        ASSERT_FALSE(validator.empty());
        ASSERT_FALSE(stack.empty());
        ASSERT_EQ(stack.size(), i + 1);
        ASSERT_EQ(stack.size(), validator.size());
    });

    auto doPop([&](std::size_t i)->void
    {
        const int value(values.at(i));

        ASSERT_EQ(value, validator.top());
        validator.pop();

        ASSERT_EQ(value, stack.pop()->val());
        ASSERT_EQ(validator.empty(), stack.empty());
    });

    ASSERT_TRUE(validator.empty());
    ASSERT_TRUE(stack.empty());

    for (std::size_t i(0); i < values.size(); ++i) doPush(i);

    ASSERT_FALSE(validator.empty());
    ASSERT_FALSE(stack.empty());

    for (std::size_t i(values.size() - 1); i < values.size(); --i) doPop(i);

    ASSERT_TRUE(validator.empty());
    ASSERT_TRUE(stack.empty());
}

TEST(Stack, PushPopOtherStack)
{
    std::stack<int> validator;
    splicer::Stack<int> stack;
    std::vector<splicer::Node<int>> nodes(makeNodes());

    auto doPush([&]()->void
    {
        splicer::Stack<int> other;

        for (auto& node : nodes)
        {
            other.push(&node);
            validator.push(node.val());
        }

        ASSERT_FALSE(other.empty());
        ASSERT_TRUE(stack.empty());

        stack.push(other);

        ASSERT_TRUE(other.empty());
        ASSERT_FALSE(stack.empty());

        ASSERT_FALSE(validator.empty());
    });

    auto doPop([&](std::size_t i)->void
    {
        const int value(values.at(i));

        ASSERT_EQ(value, validator.top());
        validator.pop();

        ASSERT_EQ(value, stack.pop()->val());
        ASSERT_EQ(validator.empty(), stack.empty());
    });

    ASSERT_TRUE(validator.empty());
    ASSERT_TRUE(stack.empty());

    doPush();

    ASSERT_FALSE(validator.empty());
    ASSERT_FALSE(stack.empty());

    for (std::size_t i(values.size() - 1); i < values.size(); --i) doPop(i);

    ASSERT_TRUE(validator.empty());
    ASSERT_TRUE(stack.empty());
}

TEST(Stack, Iterate)
{
    std::vector<splicer::Node<int>> nodes(makeNodes());
    splicer::Stack<int> stack(makeStack(nodes));

    splicer::Stack<int>::Iterator it(stack.begin());
    const splicer::Stack<int>::ConstIterator end(stack.cend());

    std::size_t i(0);

    while (it != end)
    {
        EXPECT_EQ(*it, values.at(values.size() - ++i));
        ++it;
    }

    ASSERT_EQ(i, makeNodes().size());

    it = stack.begin();
    while (it != end)
    {
        *it = 1;
        ++it;
    }

    it = stack.begin();
    while (it != end)
    {
        EXPECT_EQ(*it, 1);
        ++it;
    }
}

TEST(Stack, Range)
{
    std::vector<splicer::Node<int>> nodes(makeNodes());
    splicer::Stack<int> stack(makeStack(nodes));

    std::size_t i(0);
    for (const auto& n : stack)
    {
        EXPECT_EQ(n, values.at(values.size() - ++i));
    }

    for (auto& n : stack) n = 1;
    for (const auto& n : stack) EXPECT_EQ(n, 1);
}

TEST(Stack, RangeCopy)
{
    std::vector<splicer::Node<int>> nodes(makeNodes());
    splicer::Stack<int> stack(makeStack(nodes));

    std::size_t i(0);
    for (const auto n : stack)
    {
        EXPECT_EQ(n, values.at(values.size() - ++i));
    }

    // Not actually changing the Stack values.
    for (auto n : stack) n = 1;

    i = 0;
    for (const auto n : stack)
    {
        EXPECT_EQ(n, values.at(values.size() - ++i));
    }
}

TEST(Stack, RangeEmpty)
{
    splicer::Stack<int> stack;

    std::size_t i(0);
    for (const auto& n : stack)
    {
        EXPECT_NE(n, 12345);  // Get rid of unused variable warning.
        ++i;
    }

    EXPECT_EQ(i, 0);
}

TEST(Stack, SortedNodes)
{
    std::vector<splicer::Node<int>> nodes(20);

    for (std::size_t i(0); i < nodes.size(); ++i) *nodes[i] = i + 1;

    {
        splicer::Stack<int> stack;

        // Pushing in ascending order, so stack.head() will be 20.
        for (auto& n : nodes) stack.push(&n);

        EXPECT_EQ(stack.size(), nodes.size());
        EXPECT_TRUE(stack.sortedBy(std::greater<int>()));

        std::size_t i(20);
        for (const auto& v : stack) ASSERT_EQ(v, i--);
    }

    {
        splicer::Stack<int> stack;

        // Push with comparator std::less, so stack.head() will be 1.
        for (auto& n : nodes) stack.push(&n, std::less<int>());

        EXPECT_EQ(stack.size(), nodes.size());
        EXPECT_TRUE(stack.sortedBy(std::less<int>()));

        std::size_t i(0);
        for (const auto& v : stack) ASSERT_EQ(v, ++i);
    }

    {
        splicer::Stack<int> stack;

        // Pushing in ascending order, so stack.head() will be 20.
        for (auto& n : nodes) stack.push(&n, [](int a, int b)
        {
            return a > b;
        });

        EXPECT_EQ(stack.size(), nodes.size());
        EXPECT_TRUE(stack.sortedBy(std::greater<int>()));

        std::size_t i(20);
        for (const auto& v : stack) ASSERT_EQ(v, i--);
    }
}

TEST(Stack, SortedStacks)
{
    std::vector<splicer::Node<int>> nodes(20);

    for (std::size_t i(0); i < nodes.size(); ++i) *nodes[i] = i + 1;

    {
        splicer::Stack<int> a;
        splicer::Stack<int> b;

        // Pushing in ascending order, so .head() will be 10 and 20.
        for (auto& n : nodes)
        {
            if (*n <= 10) a.push(&n);
            else b.push(&n);
        }

        ASSERT_TRUE(a.sortedBy(std::greater<int>()));
        ASSERT_TRUE(b.sortedBy(std::greater<int>()));

        a.push(b, std::greater<int>());

        ASSERT_EQ(a.size(), nodes.size());
        ASSERT_TRUE(a.sortedBy(std::greater<int>()));

        std::size_t i(20);
        for (const auto& v : a) ASSERT_EQ(v, i--);
    }

    {
        splicer::Stack<int> a;
        splicer::Stack<int> b;

        // Push such that the nodes will interleave, with a.head() < b.head().
        for (auto& n : nodes)
        {
            if (*n % 2) a.push(&n, std::less<int>());
            else b.push(&n, std::less<int>());
        }

        ASSERT_EQ(a.size(), 10);
        ASSERT_EQ(b.size(), 10);
        ASSERT_TRUE(a.sortedBy(std::less<int>()));
        ASSERT_TRUE(b.sortedBy(std::less<int>()));

        a.push(b, std::less<int>());

        ASSERT_EQ(a.size(), nodes.size());
        ASSERT_TRUE(a.sortedBy(std::less<int>()));

        std::size_t i(0);
        for (const auto& v : a) ASSERT_EQ(v, ++i);
    }

    {
        splicer::Stack<int> a;
        splicer::Stack<int> b;

        // Same as before, but now with b.head() < a.head().
        for (auto& n : nodes)
        {
            if (*n % 2) b.push(&n, std::less<int>());
            else a.push(&n, std::less<int>());
        }

        ASSERT_EQ(a.size(), 10);
        ASSERT_EQ(b.size(), 10);
        ASSERT_TRUE(a.sortedBy(std::less<int>()));
        ASSERT_TRUE(b.sortedBy(std::less<int>()));

        a.push(b, std::less<int>());

        ASSERT_EQ(a.size(), nodes.size());
        ASSERT_TRUE(a.sortedBy(std::less<int>()));

        std::size_t i(0);
        for (const auto& v : a) ASSERT_EQ(v, ++i);
    }
}

TEST(Stack, SortedStacksEqualRange)
{
    std::vector<splicer::Node<int>> nodes(40);

    for (std::size_t i(0); i < nodes.size(); ++i) *nodes[i] = i % 20 + 1;

    splicer::Stack<int> a;
    splicer::Stack<int> b;

    for (auto& n : nodes)
    {
        if (*n % 2) a.push(&n, std::less<int>());
        else b.push(&n, std::less<int>());
    }

    ASSERT_EQ(a.size(), 20);
    ASSERT_EQ(b.size(), 20);
    ASSERT_TRUE(a.sortedBy(std::less<int>()));
    ASSERT_TRUE(b.sortedBy(std::less<int>()));

    a.push(b, std::less<int>());

    ASSERT_EQ(a.size(), nodes.size());
    ASSERT_TRUE(a.sortedBy(std::less<int>()));

    std::size_t i(0);
    for (const auto& v : a) ASSERT_EQ(v, i++ / 2 * 2 / 2 + 1);
}

TEST(Stack, SortedStacksEqualValues)
{
    std::vector<splicer::Node<int>> nodes(40);

    for (std::size_t i(0); i < nodes.size(); ++i) *nodes[i] = i / 2;

    splicer::Stack<int> a;
    splicer::Stack<int> b;

    for (auto& n : nodes)
    {
        if (*n % 2) a.push(&n, std::less<int>());
        else b.push(&n, std::less<int>());
    }

    ASSERT_EQ(a.size(), 20);
    ASSERT_EQ(b.size(), 20);
    ASSERT_TRUE(a.sortedBy(std::less<int>()));
    ASSERT_TRUE(b.sortedBy(std::less<int>()));

    a.push(b, std::less<int>());

    ASSERT_EQ(a.size(), nodes.size());
    ASSERT_TRUE(a.sortedBy(std::less<int>()));

    std::size_t i(0);
    for (const auto& v : a) ASSERT_EQ(v, i++ / 2);
}

TEST(Stack, SortedStacksEmpty)
{
    std::vector<splicer::Node<int>> nodes(makeNodes());
    std::sort(
            nodes.begin(),
            nodes.end(),
            [](const splicer::Node<int>& a, const splicer::Node<int>& b)
            {
                // Sort by >, they'll be inserted in reverse order.
                return *a > *b;
            });

    {
        splicer::Stack<int> a;
        splicer::Stack<int> b(makeStack(nodes));

        ASSERT_TRUE(b.sortedBy(std::less<int>()));

        a.push(b, std::less<int>());

        std::size_t i(0);
        for (const auto& n : a)
        {
            ASSERT_EQ(n, *nodes.at(nodes.size() - i++ - 1));
        }
    }

    {
        splicer::Stack<int> a;
        splicer::Stack<int> b(makeStack(nodes));

        ASSERT_TRUE(b.sortedBy(std::less<int>()));

        b.push(a, std::less<int>());

        std::size_t i(0);
        for (const auto& n : b)
        {
            ASSERT_EQ(n, *nodes.at(nodes.size() - i++ - 1));
        }
    }
}

