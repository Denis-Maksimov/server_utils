
#include <cassert>

#include <stdexcept> // using std::runtime_error;


#include <strings.h>
#include <string.h>
#include <unistd.h>
#include "secureLayer.hpp"
#include <openssl/err.h>
#define assertm(exp,msg) assert((void)msg,exp)

#define BUFSIZZ 2048


server::securelayer::sslCtx:: 
sslCtx()
{
    server::securelayer::sslLib::get_instance();
    this->ctx=server::securelayer::sslCtxUPtr(SSL_CTX_new(SSLv23_method()));
}

// namespace server::securelayer
// {

void
server::securelayer::sslCtx:: 
initialize_ctx(std::filesystem::path key_file,std::filesystem::path pem_file)
{
    /* Load our keys and certificates*/
    if(!(SSL_CTX_use_certificate_file(ctx.get(),pem_file.c_str(),SSL_FILETYPE_PEM)))
    {
        std::runtime_error("Не удалось загрузить файл сертификата\n");
        throw;
        // this->interrupt[CERTIFICATE_FILE_ERR].emit((event{CERTIFICATE_FILE_ERR,.emiter=this}));
    }

    if(!(SSL_CTX_use_PrivateKey_file(ctx.get(), key_file.c_str(),SSL_FILETYPE_PEM)))
    {
        std::runtime_error("Не удалось загрузить файл ключей\n");
        throw;
        // this->interrupt[PRIVATE_KEY_FILE_ERR].emit((event{PRIVATE_KEY_FILE_ERR,.emiter=this}));
    }
}

SSL_CTX* 
server::securelayer::sslCtx:: 
get_ctx()
{
    return ctx.get();
};




static int uselect(int sock,time_t seconds_timeout)
{
    timeval to={.tv_sec=seconds_timeout,.tv_usec=0};
    fd_set refset;
    /* Clear the reference set of socket */
    FD_ZERO(&refset);
    /* Add the server socket */
    FD_SET(sock, &refset);

    int status=select(sock + 1, &refset, NULL, NULL, &to);
    return status;
}


static 
std::vector<uint8_t> recvall(SSL *ssl,int s)
{
    char buf[BUFSIZZ];
    int r = 0;
    int e;
   std::vector<uint8_t> buffer;
   

    //--recvall
    do
    {
        if(uselect(s,1)<=0)break;
        r=SSL_read(ssl,buf,BUFSIZZ); // Чтение данных
        if(r<0) // Если r < 0 то произошла ошибка
        {
            e = SSL_get_error(ssl,r);
            std::cout << "err:"<<e<<std::endl;
            break;
        }
        for (size_t i = 0; i < r; i++)
        {
            buffer.push_back(buf[i]);
        }
    }while(r==BUFSIZZ);
    

    return std::move(buffer);
    // rv=buffer;


    //     std::string sl;
    //    sl.assign(buffer.begin(),buffer.end());
    //    std::cout<<"StRiNg::"<<sl<<std::endl;

    // printf("[Длина принятого текста %d, Error:%d]%s\n", r, e, buf);
    // std::cout<<"[Длина принятого текста"<<buffer.size()<<", Error:"<<e<<"]:\n";
    // std::cout<<buf<<std::endl;
    // for (auto &i : buffer)
    // {
    //     std::cout<<i;
    // }
    // std::cout<<std::endl;
    
    
}
// };

void 
server::securelayer::ssl::
send_response(const std::vector<uint8_t>& buff,SSL *ssl)
{
    // std::string resp="HTTP/1.0 200 OK\r\nServer: EKRServer\r\n\r\nServer test page\r\n";
    // buffer.assign(resp.begin(),resp.end());
    // buffer="HTTP/1.0 200 OK\r\nServer: EKRServer\r\n\r\nServer test page\r\n";
    
    /* Запись ответа */
    auto r = SSL_write(ssl,buff.data(), buff.size());
    if(r<=0)  // Если r < 0 то произошла ошибка
    {
        printf("Write error %d\n",r);
    }
    else
    {
        printf("Write ok %d\n",r);
    }
    // return need_close;

    // return(0);
}

void 
server::securelayer::ssl::
send_response(const std::string& resp,SSL *ssl)
{
    std::vector<uint8_t> buff;
    buff.assign(resp.begin(),resp.end());
    
    /* Запись ответа */
    auto r = SSL_write(ssl,buff.data(), buff.size());
    if(r<=0)  // Если r < 0 то произошла ошибка
    {
        printf("Write error %d\n",r);
    }
    else
    {
        printf("Write ok %d\n",r);
    }
}


void 
server::securelayer::ssl::observe(server::network::event const &e)
{
    assert(e.t==server::network::DATA_AVAILABLE);

    auto mssl=sslUPtr(SSL_new(ctx.get_ctx()));
    SSL_set_fd(mssl.get(), e.sock);

    auto r=SSL_accept(mssl.get());
    if( r < 0 )
    {
        this->interrupt[ACCEPT_ERROR].emit((event{.t=ACCEPT_ERROR,.emiter=this}));
        printf("SSL accept error %d\n",r);
        // printf("SSL accept error code %d\n",SSL_get_error(mssl.get(),r) );
        auto ec=SSL_get_error(mssl.get(),r);
        std::cout<<ec<<"\n";
        
        // exit(0);

    }
    else
    {
        this->interrupt[ACCEPT_OK].emit((server::securelayer::event{.t=ACCEPT_OK,.emiter=this,.u_ssl=mssl.get()}));
        printf("SSL accept %d\n",r);

        auto readbuf=recvall(mssl.get(),e.sock);
        if(readbuf.size()>0)
        {
            this->interrupt[RDATA_READY].emit((event{.t=RDATA_READY,.emiter=this,.request=readbuf,.u_ssl=mssl.get()}));
        }
    }

    
    e.emiter->close_sock=this->close_connection;
};

server::securelayer::ssl::ssl()
{
    try
    {
        ctx.initialize_ctx("server.key","server.pem");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void 
server::securelayer::ssl::
add_event_listener(std::weak_ptr<util::observer<server::securelayer::event>> const &o,type t)
{
    interrupt[t].add_observer(o);       
};

void 
server::securelayer::ssl::
add_event_listener(std::shared_ptr<util::observer<server::securelayer::event>> &o,type t)
{
    interrupt[t].add_observer(o); 
}

