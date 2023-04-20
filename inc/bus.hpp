#ifndef BUS
#define BUS
#include <vector>
#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "ibus.hpp"

/// @brief Класс myBus представляет шину сообщений, 
/// которая связывает различные компоненты между собой и 
/// позволяет им обмениваться сообщениями. Он реализует 
/// интерфейс iBus, который определяет метод, позволяющий 
/// отправлять сообщения на шину, а также метод, с помощью 
/// которого компоненты могут подписываться на сообщения в 
/// определенном "комнате" (это может быть, например, название 
/// определенного канала передачи сообщений). myBus хранит 
/// список всех компонентов, подписанных на сообщения в 
/// каждой комнате, а также очередь, в которую добавляются 
/// сообщения, поступающие в шину. Класс также реализует 
/// метод worker_(), который запускает поток, который обрабатывает 
/// очередь сообщений и отправляет их на соответствующие компоненты.
class myBus:public iBus
{
private:
    using Room=std::vector<std::weak_ptr<iComponent*>>;

    /// @brief вектор для хранения слабых указателей на все компоненты, подключенные к шине.
    std::vector<std::weak_ptr<iComponent*>> components_;

    /// @brief хэш-таблица для хранения списков слабых указателей на компоненты, подключенных к каждой комнате данных на шине.
    std::unordered_map<std::string,Room> rooms;

    /// @brief интерфейс для потока.
    std::thread thread_;

    /// @brief интерфейс мьютекса для потока.
    std::mutex mutex_;

    /// @brief интерфейс уведомления для потока.
    std::condition_variable cond_;

    /// @brief флаг остановки потока.
    bool stop_ = false;
    
    /// @brief - "queue_" - очередь кортежей из трёх строк, каждый из которых представляет комнату, сообщение и имя компонента-отправителя.
    std::queue<std::tuple<const std::string ,const std::string , const std::string >> queue_;

    //--------------------------------
    
    /// @brief * @brief - "SendOutMsgToRoomComponents" - отправляет сообщение из указанной комнаты на все компоненты этой комнаты, кроме той, что отправила сообщение.
    /// @param msg 
    /// @param room 
    /// @param from_componentName 
    void SendOutMsgToRoomComponents(const std::string& msg,const std::string& room,const std::string& from_componentName);

    /// @brief sendMsg_ - отправляет заданное сообщение в указанную комнату и также выполняет широковещательную передачу сообщения всем связанным компонентам в других комнатах.
    /// @param room 
    /// @param msg 
    /// @param componentName 
    void sendMsg_(const std::string& room,const std::string& msg, const std::string& componentName);

    /// @brief Функция работы потока, которая обрабатывает сообщения в очереди.
    void worker_();
    
public:
    
    /// @brief Конструктор
    myBus();

    /// @brief Деструктор
    ~myBus();
    
    /// @brief Функция отправки сообщения в очередь.
    /// @param room 
    /// @param msg 
    /// @param componentName 
    void sendMsg(const std::string& room, const std::string& msg, const std::string& componentName) override;
    
    //=====================================    
    /// @brief Шаблонная функция подписки компонента на комнату.
    /// @tparam F 
    /// @param room 
    /// @param component 
    /// @param cb 
    template<typename F>
    void subscribe(const std::string& room,std::weak_ptr<iComponent*> component,F cb)
    {
        if(auto ptr=component.lock())
        {
            (*(ptr))->setCallback(room,cb);
            rooms[room].push_back(component);
        }
    }
    //=====================================
    
    /// @brief Шаблонная функция подписки компонента на все сообщения.
    /// @tparam F - тип лямбда функции
    /// @param component 
    /// @param cb - лямбда функция обрадного вызова
    template<typename F>
    void subscribe_broadcast(std::weak_ptr<iComponent*> component,F cb)
    {
        if(auto ptr=component.lock())
        {
            (*(ptr))->setCallback("::broadcast::",cb);
            rooms["::broadcast::"].push_back(component);
        }
    }
    //=====================================

    /// @brief функция отписки компонента от комнаты.
    /// @param room 
    /// @param component 
    void unsubscribe(const std::string& room, std::weak_ptr<iComponent*> component);
};



/// @brief Класс myComponent представляет компонент, который может 
/// подписаться на определенные сообщения и реагировать на них с 
/// помощью определенной функции обратного вызова. Класс реализует 
/// интерфейс iComponent, который определяет методы, позволяющие 
/// компонентам отправлять сообщения на шину, задавать функции
/// обратного вызова и получать имя компонента.
class myComponent : public iComponent
{
public:
    myComponent(const std::string& name);
};


#endif /* BUS */
