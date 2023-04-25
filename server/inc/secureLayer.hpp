#ifndef SECURELAYER
#define SECURELAYER
#include "network.hpp"
#include <openssl/ssl.h>
#include <filesystem>
#include <vector>
#include <optional>

namespace server::securelayer
{
/****************************************************
* @name sslCtxDeleteFunctor
* @author Maksimov Denis (dn.maksimow@gmail.com)
* @brief интерфейс для освобождения памяти unique_ptr
* @details description
****************************************************/
struct sslCtxDeleter
{
    void
    operator()(SSL_CTX* instance)
    {
        SSL_CTX_free(instance);
        // delete instance;           
    }
};
struct sslDeleter
{
    void
    operator()(SSL* instance)
    {
        SSL_free(instance);  
    }
};

using sslCtxUPtr=std::unique_ptr<SSL_CTX,sslCtxDeleter>;
using sslUPtr=std::unique_ptr<SSL,sslDeleter>;
//-------------------------------------------------
/****************************************************
* @name sslLib
* @author Maksimov Denis (dn.maksimow@gmail.com)
* @brief
* @details description СИНГЛТОН
****************************************************/
// extern BIO *bio_err;

class sslLib
{
private:
    BIO *bio_err=0;
    sslLib(){
        /* Глобальная инициализация алгоритмов OpenSSL, без неё не обходится не один пример по OpenSSL */

        if(!bio_err)
        {
        SSL_library_init();
        SSL_load_error_strings();

        /* An error write context */
        bio_err=BIO_new_fp(stderr,BIO_NOCLOSE);

        
        }else
        {
            std::runtime_error("bioErr\n");
        }

    };
    ~sslLib(){
        BIO_free(bio_err);
    };
    sslLib(const sslLib& other)=delete;
    sslLib(sslLib&& other)=delete;
public:
    static sslLib& get_instance()
    {
        static sslLib sslLibInstance{};
        return sslLibInstance;
    }
    
};
//-------------------------------------------------
    

class sslCtx
{
private:
    sslCtxUPtr ctx;
    // util::observable<event> interrupt[N_EVENTS];
public:
    sslCtx();
    ~sslCtx()=default;

    void initialize_ctx(std::filesystem::path key_file,std::filesystem::path pem_file);
    SSL_CTX* get_ctx();;

};




enum type
    {
        RUNING=0,
        // CERTIFICATE_FILE_ERR,
        // PRIVATE_KEY_FILE_ERR,
        ACCEPT_ERROR,
        ACCEPT_OK,
        RDATA_READY,    //Принят пакет данных
        //------
        N_EVENTS
    };
    typedef struct event_t event;

class ssl: public util::observer<server::network::event>
{
private:
    sslCtx ctx;
    util::observable<event> interrupt[N_EVENTS];
    // timeval refto;
    
public:
    bool close_connection;
    ssl();
    virtual ~ssl() {}
    void send_response(const std::vector<uint8_t>& buff,SSL *ssl);
    void send_response(const std::string& buff,SSL *ssl);
    void observe(server::network::event const &e) override;

    void add_event_listener(std::shared_ptr<util::observer<server::securelayer::event>> &o,type t);
    void add_event_listener(std::weak_ptr<util::observer<server::securelayer::event>> const &o,type t);
};


struct event_t
{
    server::securelayer::type t;        //Тип прерывания
    server::securelayer::ssl* emiter;   //Хэндл звонящего
    std::optional<std::vector<uint8_t>> request;
    std::optional<SSL*> u_ssl;                         //TODO для сенд респонзе 
    
};


};



#endif /* SECURELAYER */

