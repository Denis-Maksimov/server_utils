#include "bus.hpp"
#include <iostream>
/// @brief * @brief - "SendOutMsgToRoomComponents" - отправляет сообщение из указанной комнаты на все компоненты этой комнаты, кроме той, что отправила сообщение.
/// @param msg
/// @param room
/// @param from_componentName
void myBus::SendOutMsgToRoomComponents(const std::string &msg, const std::string &room, const std::string &from_componentName)
{
    for (auto &component : rooms[room])
    {
        if (auto ptr = component.lock())
        {
            // if ((*ptr)->getName()!=componentName)
            if ((*ptr)->getName().compare(from_componentName) != 0)
            {
                (*ptr)->recvMsg(room, msg);
            }
        }
    }
}

/// @brief sendMsg_ - отправляет заданное сообщение в указанную комнату и также выполняет широковещательную передачу сообщения всем связанным компонентам в других комнатах.
/// @param room
/// @param msg
/// @param componentName
void myBus::sendMsg_(const std::string &room, const std::string &msg, const std::string &componentName)
{
    std::cout << "Bus: msg " << msg << " on room " << room << " sent by " << componentName << std::endl;
    if (!room.compare("::broadcast::"))
    {
        std::cout << "Bus: broadcast msg " << std::endl;

        // For all components from all rooms
        for (auto &room_i : rooms)
        {
            SendOutMsgToRoomComponents(msg, room_i.first, componentName);
        }
        return;
    }

    SendOutMsgToRoomComponents(msg, room, componentName);
    SendOutMsgToRoomComponents(msg, "::broadcast::", componentName);
}

/// @brief - "queue_" - очередь кортежей из трёх строк, каждый из которых представляет комнату, сообщение и имя компонента-отправителя.
std::queue<std::tuple<const std::string, const std::string, const std::string>> queue_;

//=====================================
myBus::myBus()
{
    // создание потока, в котором производится отправка сообщений.
    thread_ = std::thread(&myBus::worker_, this);
}
//=====================================
myBus::~myBus()
{
    stop_ = true;
    cond_.notify_all();
    if (thread_.joinable())
    {
        thread_.join();
    }
}
//=====================================

/// @brief Функция отправки сообщения в очередь.
/// @param room
/// @param msg
/// @param componentName
void myBus::sendMsg(const std::string &room, const std::string &msg, const std::string &componentName)
{

    std::unique_lock<std::mutex> lock(mutex_);
    queue_.push({room, msg, componentName});
    cond_.notify_all();
}
//=====================================

/// @brief Функция работы потока, которая обрабатывает сообщения в очереди.
void myBus::worker_()
{
    std::tuple<std::string, std::string, std::string> tuple;
    while (!stop_)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]
                   { return stop_ || !queue_.empty(); });

        if (!queue_.empty())
        {
            // std::cout<<queue_.size()<<std::endl;
            tuple = queue_.front();
            lock.unlock();
            sendMsg_(std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple));
            lock.lock();
            queue_.pop();
            // cnter--;
        }
    }
}
//=====================================

void myBus::unsubscribe(const std::string &room, std::weak_ptr<iComponent *> component)
{
    if (auto ptr = component.lock())
    {
        auto &room_components = rooms[room];
        auto it = std::find_if(room_components.begin(), room_components.end(), [&](const auto &cmp)
                               { return cmp.lock() == ptr; });
        if (it != room_components.end())
        {
            room_components.erase(it);
        }
    }
}

myComponent::myComponent(const std::string &name) : iComponent(name) {}

//////////////////////////////////////////////////////////////////////////
