#pragma once

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

template <typename T, typename ContainerType = std::deque<T>>
class ConcurentQueue
{
public:
    using size_type = typename ContainerType::size_type;

public:
    ConcurentQueue();
    ConcurentQueue(const ConcurentQueue&);
    ConcurentQueue(ConcurentQueue&&);

    ConcurentQueue& operator=(const ConcurentQueue&) = delete;
    ConcurentQueue& operator=(const ConcurentQueue&&) = delete;

    void push(const T&);
    void push(T&&);

    template <typename... Args>
    void emplace(Args&&...);

    std::shared_ptr<T> tryPop();
    bool tryPop(T&);

    std::shared_ptr<T> waitAndPop();
    void waitAndPop(T&);

    bool empty() const;   ///< Might be incorrect at use time
    size_type size() const;  ///< Might be incorrect at use time

private:
    std::queue<T, ContainerType> m_container;
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
};

template <typename T, typename ContainerType>
ConcurentQueue<T, ContainerType>::ConcurentQueue()
{
}

template <typename T, typename ContainerType>
ConcurentQueue<T, ContainerType>::ConcurentQueue(const ConcurentQueue& other)
{
    std::lock_guard<std::mutex> lock{other.m_mutex};

    m_container = other.m_container;
}

template <typename T, typename ContainerType>
ConcurentQueue<T, ContainerType>::ConcurentQueue(ConcurentQueue&& other)
{
    std::lock_guard<std::mutex> lock{other.m_mutex};

    m_container = std::move(other.m_container);
    m_mutex = std::move(other.m_mutex);
    m_condition = std::move(other.m_condition);
}

template <typename T, typename ContainerType>
void ConcurentQueue<T, ContainerType>::push(const T& value)
{
    std::lock_guard<std::mutex> lock{m_mutex};

    m_container.push(value);

    m_condition.notify_one();
}

template <typename T, typename ContainerType>
void ConcurentQueue<T, ContainerType>::push(T&& value)
{
    std::lock_guard<std::mutex> lock{m_mutex};

    m_container.push(std::move(value));

    m_condition.notify_one();
}

template <typename T, typename ContainerType>
template <typename... Args>
void ConcurentQueue<T, ContainerType>::emplace(Args&&... args)
{
    std::lock_guard<std::mutex> lock{m_mutex};

    m_container.emplace(std::forward<Args>(args)...);

    m_condition.notify_one();
}

template <typename T, typename ContainerType>
std::shared_ptr<T> ConcurentQueue<T, ContainerType>::tryPop()
{
    std::lock_guard<std::mutex> lock{m_mutex};

    std::shared_ptr<T> result{};

    if (m_container.empty())
        return result;

    result = std::make_shared<T>(m_container.front());
    m_container.pop();

    return result;
}

template <typename T, typename ContainerType>
bool ConcurentQueue<T, ContainerType>::tryPop(T& value)
{
    std::lock_guard<std::mutex> lock{m_mutex};

    if (m_container.empty())
        return false;

    value = m_container.front();
    m_container.pop();

    return true;
}

template <typename T, typename ContainerType>
std::shared_ptr<T> ConcurentQueue<T, ContainerType>::waitAndPop()
{
    std::unique_lock<std::mutex> lock{m_mutex};

    m_condition.wait(lock, [this]() {
        return !m_container.empty();
    });

    auto result = std::make_shared<T>(m_container.front());
    m_container.pop();

    return result;
}

template <typename T, typename ContainerType>
void ConcurentQueue<T, ContainerType>::waitAndPop(T& value)
{
    std::unique_lock<std::mutex> lock{m_mutex};

    m_condition.wait(lock, [this]() {
        return !m_container.empty();
    });

    value = m_container.front();
    m_container.pop();
}

template <typename T, typename ContainerType>
bool ConcurentQueue<T, ContainerType>::empty() const
{
    std::unique_lock<std::mutex> lock{m_mutex};

    return m_container.empty();
}

template <typename T, typename ContainerType>
typename ConcurentQueue<T, ContainerType>::size_type ConcurentQueue<T, ContainerType>::size() const
{
    std::unique_lock<std::mutex> lock{m_mutex};

    return m_container.size();
}
