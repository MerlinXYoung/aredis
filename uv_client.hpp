#ifndef __AREDIS_UV_CLIENT_HPP_
#define __AREDIS_UV_CLIENT_HPP_
#include "client.hpp"
#include <hiredis/adapters/libuv.h>
namespace aredis{

template<class _Mutex>
class uv_client:public detail::client<uv_client<_Mutex>, _Mutex>
{
public:
    using super = detail::client<uv_client<_Mutex>, _Mutex>;
    using self = uv_client<_Mutex>;
    // uv_client():super(){
    // }
    uv_client()= default;
    bool connect(uv_loop_t& loop, const char* host, int port){
        if(super::connect(host,port))
            return this->attach(loop, redisLibuvAttach, std::nothrow);
            // return 0==redisLibuvAttach(this->ctx_.get(), &loop);
    }
    bool connect(uv_loop_t& loop, const char* path){
        if(super::connect(path))
            return this->attach(loop, redisLibuvAttach, std::nothrow);
    }
    bool connect(uv_loop_t& loop, const redisOptions& options){
        if(super::connect(options))
            return this->attach(loop, redisLibuvAttach, std::nothrow);
    }
    template<bool IsReuse=true>
    bool connect(uv_loop_t& loop, const char* host, int port,const char *source_addr){
        if(super::connect(host, port, source_addr))
            return this->attach(loop, redisLibuvAttach, std::nothrow);
    }

};

};

#endif//__AREDIS_UV_CLIENT_HPP_