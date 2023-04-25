#ifndef HTTPLAYER
#define HTTPLAYER

#include "secureLayer.hpp"

namespace server::httplayer
{

    class http: public util::observer<server::securelayer::event>
    {
    public:
        http();
        virtual ~http() {}
        void observe(server::securelayer::event const &e) override;
    };

}





#endif /* HTTPLAYER */
