#pragma once
#ifndef NETWORK
#define NETWORK



#include <mutex>
#include <memory>
#include <stdexcept> // using std::runtime_error;

// #include <iostream>
#include "iobserver.hpp"


#include "common.hpp"



/****************************************************
* @name server::network
* @author Maksimov Denis (dn.maksimow@gmail.com)
* @brief Сеансовый уровень сервера. ЗАмысел в том, чтобы ловить сигнал DATA_AVAILABLE
* в следующем шаге нужно упаковать в пакет через recv, а дальше будем посмотреть
* @details description
****************************************************/
namespace server::network
{
    //--------------------------------------------------------------------------------------
    enum type
    {
        RUNING=0,
        NEED_STOP,
        TIMEOUT_SELECT,
        NEW_CONNECTION,
        ACCEPT_SUCCESS,
        DATA_AVAILABLE, //<--с сокета нужно принять данные, запихать в пакет и обработать

        ERR_INVALID_IP,
        ERR_BIND,
        ERR_LISTEN,
        ERR_SOCKET,
        ERR_SETSOCKOPT,
        ERR_SELECT,
        ERR_ACCEPT,
        //------
        N_EVENTS
    };
    //--------------------------------------------------------------------------------------
    
    typedef struct event_t event;
    //--------------------------------------------------------------------------------------

    class TCPServer
    {

    public:
        using event_listener_nacked=util::observer<server::network::event>;
        using event_listener=std::weak_ptr<util::observer<server::network::event>>;
        using event_listener_shared=std::shared_ptr<util::observer<server::network::event>>;
    private:
        util::observable<event> interrupt[N_EVENTS];
        server::common::ThreadPool tp;
        std::mutex mtx;
        timeval refto={.tv_sec=1,.tv_usec=3000};
        bool running=false;
        

        std::string ip="0.0.0.0";
        int port=8082;
        int msocket;


        void loop();
        //--------------------------------------------------------------------------------------
        class this_debug : public event_listener_nacked
        {
            public:
            virtual ~this_debug(){};
            bool operator==(const this_debug &r) const;
            void observe( subject_t const & data) override;
        };
        //--------------------------------------------------------------------------------------

        event_listener_shared dbg;
        int tcp_listen(int nb_connection);
    public:
        bool close_sock=false;
        
        TCPServer(/* args */);
        ~TCPServer();
        
        void run();
        void stop();
        void add_event_listener(std::shared_ptr<util::observer<server::network::event>> &o,type t);
        void add_event_listener(std::weak_ptr<util::observer<server::network::event>> const &o,type t);
        
    };


    struct event_t
    {
        type t;
        int sock;
        TCPServer* emiter;
    };
    //--------------------------------------------------------------------------------------


}; // namespace server




#endif /* NETWORK */
