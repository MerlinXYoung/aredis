#ifndef __AREDIS_EVENT_CLIENT_HPP_
#define __AREDIS_EVENT_CLIENT_HPP_
#include "client.hpp"
#include <hiredis/adapters/libevent.h>
namespace aredis{

template<class _Mutex>
class event_client:public detail::client<event_client<_Mutex>, _Mutex>
{
public:
    using super = detail::client<event_client<_Mutex>, _Mutex>;
    using self = event_client<_Mutex>;
    event_client():super(){

    }

    bool connect(event_base& loop, const char* host, int port){
        if(super::connect(host,port))
            return attach(loop_, redisLibeventAttach);
    }
    bool connect(event_base& loop, const char* path){
        if(super::connect(path))
            return attach(loop_, redisLibeventAttach);
    }
    bool connect(event_base& loop, const redisOptions& options){
        if(super::connect(options))
            return attach(loop_, redisLibeventAttach);
    }
    template<bool IsReuse=true>
    bool connect(event_base& loop, const char* host, int port,const char *source_addr){
        if(super::connect(host, port, source_addr))
            return attach(loop_, redisLibeventAttach);
    }
};

};

#endif//__AREDIS_EVENT_CLIENT_HPP_