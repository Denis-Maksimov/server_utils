#include "httpLayer.hpp"

server::httplayer::http::
http()
{

}

void
server::httplayer::http::
observe(server::securelayer::event const &e)
{
    std::string sl;
    if(e.request)
    {
        sl.assign((*e.request).begin(),(*e.request).end());
        std::cout<< "[msg]:\n"<<sl<<std::endl;
    }
    if(e.u_ssl)
    {    
        e.emiter->send_response("HTTP/1.0 200 OK\r\nServer: EKRServer\r\n\r\n",*e.u_ssl);
        e.emiter->send_response("Teapot\r\n",*e.u_ssl);
    }
    e.emiter->close_connection=true;
    
}



