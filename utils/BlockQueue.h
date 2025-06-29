#ifndef __BLOCKQUEUE_H__
#define __BLOCKQUEUE_H__

#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <assert.h>
#include <chrono>

template <typename T>
class BlockQueue
{
public:
    explicit BlockQueue(size_t maxCapacity = 1024);
    ~BlockQueue();

    /**
    /*@brief 入队
    /*
    /*@param item 入队元素
    */
    void push(T &item);

    /**
    /*@brief 入队批量元素
    /*
    /*@param items 入队元素
    */
    void push_batch(std::vector<T> &items);

    /**
    /*@brief 出队
    /*
    /*@param item 出队元素
    */
    bool pop(T &item);

   /**
   /*@brief 出队
   /*
   /*@param item 
   /*@param timeout 超时时间
   /*@return true 
   /*@return false 
    */
    bool pop(T &item, int timeout);

    /**
    /*@brief 出队批量元素
    /*
    /*@param items 出队元素
    /*@param maxCount 最大出队数量
    /*@return size_t 实际出队数量
     */
    size_t pop_batch(std::vector<T> &items, size_t maxCount);

    void close();

    void flush();

    void clear();

    bool empty();

    bool full();
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    bool m_isClose;
    size_t m_capacity;
    std::condition_variable m_consumer;
    std::condition_variable m_producer;
};

#endif /* __BLOCKQUEUE_H__ */

template <typename T>
inline BlockQueue<T>::BlockQueue(size_t maxCapacity)
{
    assert(maxCapacity > 0);
    m_capacity = maxCapacity;
    m_isClose = false;
}

template <typename T>
inline BlockQueue<T>::~BlockQueue()
{
    close();
}

template <typename T>
inline void BlockQueue<T>::push(T &item)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_producer.wait(lock, [this]() { return !full(); });

    m_queue.push(item);
    m_consumer.notify_one();
}

template <typename T>
inline void BlockQueue<T>::push_batch(std::vector<T> &items)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_producer.wait(lock, [this, &items]{
        return m_queue.size() + items.size() <= m_capacity;
    });

    for (auto &item : items)
    {
        m_queue.push(item);
    }

    m_consumer.notify_all();
}

template <typename T>
inline bool BlockQueue<T>::pop(T &item)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_consumer.wait(lock, [this]() { return m_isClose || !empty(); });

    item = std::move(m_queue.front());
    m_queue.pop();

    m_producer.notify_one();
    return true;
}

template <typename T>
inline bool BlockQueue<T>::pop(T &item, int timeout)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_consumer.wait(lock, [this]() { return m_isClose || !empty(); });

    if (m_isClose && empty()) {
        return false; // 队列关闭且为空，无法获取元素
    }
    item = std::move(m_queue.front());
    m_queue.pop();
    m_producer.notify_one();
    return true;
}

template <typename T>
inline size_t BlockQueue<T>::pop_batch(std::vector<T> &items, size_t maxCount)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    size_t availableItems = std::min(maxCount, m_queue.size());

    items.clear();
    items.reserve(availableItems);

    for (size_t i =0; i < availableItems; ++i)
    {
        items.push_back(std::move(m_queue.front()));
        m_queue.pop();
    }

    if (availableItems > 0)
    {
        m_producer.notify_all();
    }
    return availableItems;
}

template <typename T>
inline void BlockQueue<T>::close()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::queue<T> emptyQueue;
        std::swap(m_queue, emptyQueue);
        m_isClose = true;
    }

    m_producer.notify_all();
    m_consumer.notify_all();

}
template <typename T>
inline void BlockQueue<T>::flush()
{
    m_consumer.notify_all();
}
template <typename T>
inline void BlockQueue<T>::clear()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::queue<T> emptyQueue;
    std::swap(m_queue, emptyQueue);
}
template <typename T>
inline bool BlockQueue<T>::empty()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
}
template <typename T>
inline bool BlockQueue<T>::full()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.size() >= m_capacity;
}
