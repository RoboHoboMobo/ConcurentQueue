#pragma once

#include <queue>

template <typename T>
struct TestContainer
{
    using size_type = size_t;
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;

    static std::deque<T> data;

    TestContainer() = default;
    TestContainer(const TestContainer&) = default;
    TestContainer(TestContainer&&) = default;

    static void clear();

    T& front();
    const T& front() const;

    T& back();
    const T& back() const;

    void push_back(const T&);
    void push_back(T&&);

    template <typename... Args>
    void emplace_back(Args&&...);

    void pop_front();
    void pop_back();

    bool empty() const;
    size_t size() const;
};

template <typename T>
std::deque<T> TestContainer<T>::data = {};

template <typename T>
void TestContainer<T>::clear()
{
    data.clear();
}

template <typename T>
T& TestContainer<T>::front()
{
    return data.front();
}

template <typename T>
const T& TestContainer<T>::front() const
{
    return data.front();
}

template <typename T>
T& TestContainer<T>::back()
{
    return data.back();
}

template <typename T>
const T& TestContainer<T>::back() const
{
    return data.back();
}

template <typename T>
void TestContainer<T>::push_back(const T& value)
{
    data.push_back(value);
}

template <typename T>
void TestContainer<T>::push_back(T&& value)
{
    data.push_back(std::move(value));
}

template <typename T>
template <typename... Args>
void TestContainer<T>::emplace_back(Args&&... args)
{
    data.emplace_back(std::forward<Args>(args)...);
}

template <typename T>
void TestContainer<T>::pop_front()
{
    data.pop_front();
}

template <typename T>
void TestContainer<T>::pop_back()
{
    data.pop_back();
}

template <typename T>
bool TestContainer<T>::empty() const
{
    return data.empty();
}

template <typename T>
size_t TestContainer<T>::size() const
{
    return data.size();
}
