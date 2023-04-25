#include "network.hpp"

namespace server::common
{
void (*ThreadDeleter)(std::thread *t) =	[](std::thread* t)	{t->join();	delete t; std::cout <<"end T\n";};
};

#if defined(_WIN32)
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#endif

#if defined(_WIN32)
/* Already set in modbus-tcp.h but it seems order matters in VS2005 */
# include <winsock2.h>
# include <ws2tcpip.h>
# define SHUT_RDWR 2
# define close closesocket
# define strdup _strdup
#else
# include <sys/socket.h>
# include <sys/ioctl.h>

#if defined(__OpenBSD__) || (defined(__FreeBSD__) && __FreeBSD__ < 5)
# define OS_BSD
# include <netinet/in_systm.h>
#endif

# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/tcp.h>
# include <arpa/inet.h>
# include <netdb.h>
#endif

#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <cstdlib>

    void server::network::TCPServer::run()
    {
                
        this->interrupt[RUNING].emit((event{RUNING}));
        running=true;
        server::common::ThreadPtr Loop_thread(new std::thread(&TCPServer::loop, this), server::common::ThreadDeleter);
        tp.push(std::move(Loop_thread));
    }

    void server::network::TCPServer::stop()
    {
        this->interrupt[NEED_STOP].emit((event{NEED_STOP}));
        mtx.lock();
        running=false;
        mtx.unlock();
    }


    bool server::network::TCPServer::this_debug::operator==(const this_debug &r) const 
    { 
        return this == &r;
    }
    void server::network::TCPServer::this_debug::observe(subject_t const & data)
    {
        switch (data.t)
        {
            case RUNING:
            {
                std::cout << "Server is running" << std::endl;
                break;
            }
            case NEED_STOP:
            {
                std::cout << "Server is terminate" << std::endl;
                break;
            }
            default:
            {
                std::cout << "Event["<<data.t<<"]" << std::endl;
                break;
            }
        }
    }

    server::network::TCPServer::TCPServer(/* args */):
    dbg(std::make_shared<this_debug>())
    {
        for (size_t i = 0; i < N_EVENTS; i++)
        {
            this->add_event_listener(dbg, type(i));
        }
        
    }
    
    server::network::TCPServer::~TCPServer()
    {
    }
    
    void server::network::TCPServer::add_event_listener(std::weak_ptr<util::observer<server::network::event>> const &o,type t)
    {
        interrupt[t].add_observer(o);       
    };

    void server::network::TCPServer::add_event_listener(std::shared_ptr<util::observer<server::network::event>> &o,type t)
    {
        interrupt[t].add_observer(o); 
    }

    
    void server::network::TCPServer::loop()
    {
        //[1]--------------------------------
        timeval to=refto;
        fd_set refset;
        fd_set rdset;
        int master_socket;
        int rc;
        int status;

        bool live=true;
        // uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
        // listen()
        auto server_socket = this->tcp_listen(1);
        if (server_socket == -1) 
        {

            // fprintf(stderr, "Unable to listen TCP connection\n");
            // this->emit((event{LISTEN_ERR}));
            this->interrupt[ERR_LISTEN].emit((event{ERR_LISTEN}));
            return;
            // modbus_free(ctx);
            // return -1;
        }

        //[3]--------------------------------
        /* Clear the reference set of socket */
        FD_ZERO(&refset);
        /* Add the server socket */
        FD_SET(server_socket, &refset);
        /* Keep track of the max file descriptor */
        auto fdmax = server_socket;
        //[4]--------------------------------
        while (live) 
        {
            //--------------------------------
            mtx.lock();
            live=running;
            to=refto;
            mtx.unlock();
            //--------------------------------
            if(!live) break;

            rdset = refset;

            status=select(fdmax + 1, &rdset, NULL, NULL, &to);

            //--(1)------------------------------
            if ( status == -1) 
            {
                // perror("Server select() failure.");
                this->interrupt[ERR_SELECT].emit((event{ERR_SELECT,.emiter=this}));
                std::this_thread::sleep_for((std::chrono::milliseconds)10);
                continue;
                // close_sigint(1);
            }

            //--(2)------------------------------
            if (status == 0)
            {
                this->interrupt[TIMEOUT_SELECT].emit((event{TIMEOUT_SELECT,.emiter=this}));
            }

            //--(3)------------------------------
            /* Run through the existing connections looking for data to be
            * read */
            for (master_socket = 0; master_socket <= fdmax; master_socket++) 
            {

                //--(3.1)------------------------------
                if (!FD_ISSET(master_socket, &rdset)) 
                {
                    continue;
                }

                //--(3.2)------------------------------
                if (master_socket == server_socket) 
                {
                    /* A client is asking a new connection */
                    socklen_t addrlen;
                    struct sockaddr_in clientaddr={0};
                    int newfd;

                    /* Handle new connections */
                    addrlen = sizeof(clientaddr);
                    // memset(&clientaddr, 0, sizeof(clientaddr));
                    
                    this->interrupt[NEW_CONNECTION].emit((server::network::event{NEW_CONNECTION,.emiter=this}));

                    newfd = accept(server_socket, (struct sockaddr *) &clientaddr, &addrlen);
                    //--(3.2.1)------------------------------
                    if (newfd == -1) 
                    {
                        this->interrupt[ERR_ACCEPT].emit((server::network::event{ERR_ACCEPT,.emiter=this}));                   
                    } else {
                    //--(3.2.2)------------------------------
                        FD_SET(newfd, &refset);
                        if (newfd > fdmax) 
                        {
                            /* Keep track of the maximum */
                            fdmax = newfd;
                        }
                        printf("New connection from %s:%d on socket %d\n",
                            inet_ntoa(clientaddr.sin_addr),
                            clientaddr.sin_port,
                            newfd);
                        this->interrupt[ACCEPT_SUCCESS].emit((server::network::event{ACCEPT_SUCCESS,.emiter=this}));
                    }
                } else {
                //--(3.3)------------------------------
                    // modbus_set_socket(ctx.get(), master_socket);
                    msocket=master_socket;
                    
                    this->interrupt[DATA_AVAILABLE].emit((server::network::event{DATA_AVAILABLE,.sock=msocket,.emiter=this}));
                    // rc = modbus_receive(ctx.get(), query);
                    // if (rc > 0) 
                    // {
                    //     //--(3.3.1)------------------------------
                    //     modbus_reply(ctx.get(), query, rc, map.get());
                    //     this->emit((event{REQUEST_REPLY}));
                        
                    // }//--(3.3.1)
                    // else if (rc == -1) 
                    if (this->close_sock)
                    {
                        this->close_sock=false;
                        //--(3.3.2)------------------------------   
                        /* This example server in ended on connection closing or
                        * any errors. */
                        printf("Connection closed on socket %d\n", master_socket);
                        // this->emit((event{CONNECTION_CLOSED}));

                        close(master_socket);

                        /* Remove from reference set */
                        FD_CLR(master_socket, &refset);
                        if (master_socket == fdmax) 
                        {
                            fdmax--;
                        }
                    }//--(3.3.2)
                }//--(3.3)
            }//--(3)
        }//--while forewer
    }//--void loop();











#ifdef OS_WIN32
static int _modbus_tcp_init_win32(void)
{
    /* Initialise Windows Socket API */
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr,
                "WSAStartup() returned error code %d\n",
                (unsigned int) GetLastError());
        errno = EIO;
        return -1;
    }
    return 0;
}
#endif



/* Listens for any request from one or many modbus masters in TCP */
int server::network::TCPServer::tcp_listen(int nb_connection)
{
    int new_s;
    int enable;
    int flags;
    struct sockaddr_in addr={0};

    int rc;


#ifdef OS_WIN32
// #error TODO
    if (_modbus_tcp_init_win32() == -1) {
        return -1;
    }
#endif

    flags = SOCK_STREAM;

#ifdef SOCK_CLOEXEC
    flags |= SOCK_CLOEXEC;
#endif

    new_s = socket(PF_INET, flags, IPPROTO_TCP);
    if (new_s == -1) {
        this->interrupt[ERR_SOCKET].emit((event{ERR_SOCKET}));
        return -1;
    }

    enable = 1;
    if (setsockopt(new_s, SOL_SOCKET, SO_REUSEADDR, (char *) &enable, sizeof(enable)) ==
        -1) {
        this->interrupt[ERR_SETSOCKOPT].emit((event{ERR_SETSOCKOPT}));
        close(new_s);
        return -1;
    }

    // memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    /* If the modbus port is < to 1024, we need the setuid root. */
    addr.sin_port = htons(this->port);
    if (this->ip.c_str()[0] == '0') 
    {
        /* Listen any addresses */
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        /* Listen only specified IP address */
        rc = inet_pton(addr.sin_family, this->ip.c_str(), &(addr.sin_addr));
        if (rc <= 0) 
        {
            // if (ctx->debug) {
            //     fprintf(stderr, "Invalid IP address: %s\n", ctx_tcp->ip);
            // }
            this->interrupt[ERR_INVALID_IP].emit((event{ERR_INVALID_IP}));

            //------------------------
            close(new_s);
            // std::runtime_error("invalid ip!");
            return -1;
            //------------------------
        }
    }

    if (bind(new_s, (struct sockaddr *) &addr, sizeof(addr)) == -1) 
    {
        this->interrupt[ERR_BIND].emit((event{ERR_BIND}));
        close(new_s);
        return -1;
    }

    if (listen(new_s, nb_connection) == -1) 
    {
        this->interrupt[ERR_LISTEN].emit((event{ERR_LISTEN}));
        close(new_s);
        return -1;
    }

    return new_s;
}


// int main(int argc, char const *argv[])
// {
//     server::network::TCPServer s;
//     s.run();
//     s.add_event_listener(std::make_shared<util::observer<server::network::event>>(),
//                         server::network::DATA_AVAILABLE);

//     std::this_thread::sleep_for(std::chrono::milliseconds(5000));
//     s.stop();
//     return 0;
// }


