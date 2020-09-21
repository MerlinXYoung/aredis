#ifndef __AREDIS_EV_CLIENT_HPP_
#define __AREDIS_EV_CLIENT_HPP_
#include "client.hpp"
#include <hiredis/adapters/libev.h>
namespace aredis{

template<class _Mutex>
class ev_client:public detail::client<ev_client<_Mutex>, _Mutex>
{
public:
    using super = detail::client<ev_client<_Mutex>, _Mutex>;
    ev_client():super(){
    }

    bool connect(ev_loop& loop, const char* host, int port){
        if(super::connect(host,port))
            return attach(loop, redisLibevAttach);
    }
    bool connect(ev_loop& loop, const char* path){
        if(super::connect(path))
            return attach(loop, redisLibevAttach);
    }
    bool connect(ev_loop& loop, const redisOptions& options){
        if(super::connect(options))
            return attach(loop, redisLibevAttach);
    }
    template<bool IsReuse=true>
    bool connect(ev_loop& loop, const char* host, int port,const char *source_addr){
        if(super::connect(host, port, source_addr))
            return attach(loop, redisLibevAttach);
    }

};

};

#endif//__AREDIS_EV_CLIENT_HPP_