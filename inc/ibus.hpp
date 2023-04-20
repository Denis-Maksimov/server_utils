#ifndef IBUS
#define IBUS

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

/// @brief Интерфейс для обмена сообщениями между компонентами
class iBus
{
private:
    /// @brief Указатель на объект класса iBus
    std::shared_ptr<iBus*> bus_;

public:
    /// @brief Отправить сообщение в шину
    /// @param room Название комнаты, в которую отправляется сообщение
    /// @param msg Текст сообщения
    /// @param componentName Название компонента, который отправляет сообщение
    virtual void sendMsg(const std::string& room,const std::string& msg, const std::string& componentName)
    {};

    /// @brief Конструктор класса iBus
    iBus()
    {
        bus_=std::make_shared<iBus*>(this);
    }
    
    /// @brief Получить слабый указатель на объект класса iBus
    /// @return Слабый указатель на объект класса iBus
    std::weak_ptr<iBus*> getPtr() const
    {
        return bus_;
    }

    /// @brief Получить слабый указатель на объект класса iBus
    /// @return Слабый указатель на объект класса iBus
    std::weak_ptr<iBus *> operator& () const
    {
        return bus_;
    }
};

/// @brief Интерфейс компонента
class iComponent
{
protected:
    std::string name_; ///< Название компонента
    std::weak_ptr<iBus*> bus_; ///< Слабый указатель на объект класса iBus
    std::shared_ptr<iComponent*> ptr_; ///< Сильный указатель на текущий объект класса iComponent
    std::unordered_map<
    std::string,
    std::function<void(const std::string& msg)>> callback_s; ///< Мапа из функций-обработчиков для каждой комнаты сообщений

public:
    /// @brief Установить функцию-обработчик для заданной комнаты сообщений
    /// @tparam F Тип функции-обработчика
    /// @param id Название комнаты сообщений
    /// @param f Функция-обработчик
    template<typename F>
    void setCallback(std::string id,F&& f)
    {
        callback_s[id]=std::forward<F>(f);
    }

    /// @brief Конструктор класса iComponent
    /// @param name Название компонента
    iComponent(const std::string& name):
    name_(name),
    ptr_(std::make_shared<iComponent*>(this))
    {}

    /// @brief Обработчик сообщений
    /// @param room Название комнаты сообщений
    /// @param msg Текст сообщения
    virtual void recvMsg(const std::string& room,const std::string& msg)
    {
        callback_s[room](msg);
    };

    /// @brief Установить слабый указатель на объект класса iBus
    /// @param bus Слабый указатель на объект класса iBus
    void setBus(std::weak_ptr<iBus*> bus)
    {
        bus_=bus;
    }

    /// @brief Отправить сообщение заданного текста в заданную комнату
    /// @param room Название комнаты
    /// @param msg Текст сообщения
    void sendMsg(const std::string& room,const std::string& msg)
    {
        if (auto ptr=bus_.lock())
        {
            (*ptr)->sendMsg(room,msg,name_);
        }
    }

    /// @brief Отправить сообщение заданного текста в заданную комнату
    /// @param bus Слабый указатель на объект класса iBus
    /// @param room Название комнаты
    /// @param msg Текст сообщения
    void sendMsg(std::weak_ptr<iBus *> bus,const std::string& room,const std::string& msg)
    {
        if (auto ptr=bus.lock())
        {
            (*ptr)->sendMsg(room,msg,name_);
        }
    }

    /// @brief Отправить сообщение заданного текста в заданную комнату
    /// @param bus Объект класса iBus
    /// @param room Название комнаты
    /// @param msg Текст сообщения
    void sendMsg(iBus& bus,const std::string& room,const std::string& msg)
    {
        bus.sendMsg(room,msg,name_);
    }

    /// @brief Получить название компонента
    /// @return Название компонента
    std::string getName() const
    {
        return name_;
    }

    /// @brief Получить слабый указатель на текущий объект класса iComponent
    /// @return Слабый указатель на текущий объект класса iComponent
    std::weak_ptr<iComponent*> getPtr() const
    {
        return ptr_;
    }

    /// @brief Получить слабый указатель на текущий объект класса iComponent
    /// @return Слабый указатель на текущий объект класса iComponent
    std::weak_ptr<iComponent *> operator& () const
    {
        return ptr_;
    }
};

#endif /* IBUS */