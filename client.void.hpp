#ifndef __AREDIS_CLIENT_VOID_HPP__
#define __AREDIS_CLIENT_VOID_HPP__
namespace aredis{
namespace detail{
template<class derived_t>
class client<derived_t, void>:public detail::object_t<derived_t>{
public:   
    using mutex_type = void;
    using lock_guard_type = void;

    client()=default;
#ifdef __AREDIS_SSL__
    static void init_ssl()
    { redisInitOpenSSL();}
    bool init_ssl(const char *cert, const char *certKey, const char *caCert){ 
        redisSSLContextError ssl_error;
        ssl_.reset(redisCreateSSLContext(caCert, NULL,
                cert, certKey, NULL, &ssl_error));
        if (!ssl_) {
            printf("Error: %s\n", redisSSLContextGetError(ssl_error));
            return false;
        }
        return true;
    }
#endif//__AREDIS_SSL__
    derived_t& bind_connected(std::function<void(derived_t&, int status)> handle)
    {
        on_connected_ = std::move(handle);
        return *this;
    }

    derived_t& bind_disconnected(std::function<void(derived_t&, int status)> handle)
    {
        on_disconnected_ = std::move(handle);
        return *this;
    }
protected:
    bool connect(const char* host, int port){
        if(ctx_)
            return false;
        ctx_.reset(redisAsyncConnect(host, port));
        if(!ctx_)
            return false;
        return after_connect(std::nothrow);
    }
    bool connect(const char* path){
        if(ctx_)
            return false;
        ctx_.reset(redisAsyncConnectUnix(path));
        if(!ctx_)
            return false;
        return after_connect(std::nothrow);
    }
    bool connect(const redisOptions& options){
        if(ctx_)
            return false;
        ctx_.reset(redisAsyncConnectWithOptions(&options));
        if(!ctx_)
            return false;
        return after_connect(std::nothrow);
    }
#if __cplusplus >= 201703L
    template<bool IsReuse=true>
    bool connect(const char* host, int port,const char *source_addr){
        if(ctx_)
            return false;
        if constexpr (IsReuse)
        {
            ctx_.reset(redisAsyncConnectBindWithReuse(host, port, source_addr));
        }
        else
        {
            ctx_.reset(redisAsyncConnectBind(host, port, source_addr));
        }
        if(!ctx_)
            return false;
        return after_connect(std::nothrow);
    }
#else  
    bool connect(const char* host, int port,const char *source_addr, bool isResuse=true){
        if(ctx_)
            return false;
        if (isResuse)
        {
            ctx_.reset(redisAsyncConnectBindWithReuse(host, port, source_addr));
        }
        else
        {
            ctx_.reset(redisAsyncConnectBind(host, port, source_addr));
        }
        if(!ctx_)
            return false;
        return after_connect(std::nothrow);
    }
#endif
    template<class loop_t>
    inline void attach(loop_t& loop, int(*attach_func)(redisAsyncContext *, loop_t*)  )
    {
        if(0!=attach_func(ctx_.get(),&loop))
            throw std::runtime_error("aredis::client attach_func error!");
    }

    template<class loop_t>
    inline bool attach(loop_t& loop, int(*attach_func)(redisAsyncContext *, loop_t*), const std::nothrow_t&)
    {
        return 0==attach_func(ctx_.get(),&loop);
    }
public:
    template<class... Args>
    int post(std::function<void(redisReply *)> _callback, const char* fmt, const Args&... args)
    {
        auto holdor = std::make_unique<std::function<void(redisReply *)>>(std::move(_callback));
        auto r = redisAsyncCommand(ctx_.get(), client::_redis_cb, holdor.get(), fmt, args...);
        if(0 == r){ 
            holdor.release();
        }
        return r;
    }
#if __cplusplus >= 201703L
    int post(std::function<void(redisReply *)> _callback,  std::string_view str)
    {
        auto holdor = std::make_unique<std::function<void(redisReply *)>>(std::move(_callback));
        auto r = redisAsyncFormattedCommand(ctx_.get(), client::_redis_cb, holdor.get(), str.data(), str.size());
        if(0 == r){ 
            holdor.release();
        }
        return r;
    }
#endif
    inline void set_timeout(timeval tv){ 
        assert(ctx_.get());
        redisAsyncSetTimeout(ctx_.get(), tv);
    }
    inline void set_timeout(size_t microseconds){
        set_timeout({0, (std::int64_t)microseconds});
    }
    void close() {
        redisAsyncDisconnect(ctx_.get());
    }
public:   
    static void _redis_cb(redisAsyncContext *c, void *r, void *privdata)
    {
        redisReply *reply = (redisReply *)r;
        std::unique_ptr<std::function<void(redisReply *)>> _callback(static_cast<std::function<void(redisReply *)>*>(privdata));
        auto& callback = *(_callback.get());
        callback(reply);
    }
    static void _redis_connect_cb(const struct redisAsyncContext* c, int status)
    {
        if (status != REDIS_OK) {
            printf("Error: %s\n", c->errstr);
            return;
        }
        printf("Connected...\n");
        derived_t* derived = (derived_t*)c->data;
        if(derived->on_connected_)
            derived->on_connected_(*derived, status);
    }
    static void _redis_disconnect_cb(const struct redisAsyncContext* c, int status)
    {
        if (status != REDIS_OK) {
            printf("Error: %s\n", c->errstr);
            return;
        }
        printf("Disconnected...\n");
        derived_t* derived = (derived_t*)c->data;
        if(derived->on_disconnected_)
            derived->on_disconnected_(*derived, status);
    }

private:

    void after_connect()
    {
        ctx_->data = this;
#ifdef __AREDIS_SSL__
        if(ssl_)
        {
            if (redisInitiateSSLWithContext(&ctx_->c, ssl_.get()) != REDIS_OK) {
                throw std::runtime_error("aredis::client redisInitiateSSLWithContext error!");
            }
        }
#endif//__AREDIS_SSL__
        // if(0!=attach_func(ctx_.get(),&loop))
        //     throw std::runtime_error("aredis::client attach_func error!");
		if(0!=redisAsyncSetConnectCallback(ctx_.get(),client::_redis_connect_cb))
            throw std::runtime_error("aredis::client redisAsyncSetConnectCallback error!");
		if(0!=redisAsyncSetDisconnectCallback(ctx_.get(),client::_redis_disconnect_cb))
            throw std::runtime_error("aredis::client redisAsyncSetDisconnectCallback error!");
    }
    bool after_connect(const std::nothrow_t&)
    {
        ctx_->data = this;
#ifdef __AREDIS_SSL__
        if(ssl_)
        {
            if (redisInitiateSSLWithContext(&c->c, ssl_.get()) != REDIS_OK) 
                return false;
        }
#endif//__AREDIS_SSL__
        // if(0!=redisLibuvAttach(ctx_.get(),&loop_))
        //     return false;
		if(0!=redisAsyncSetConnectCallback(ctx_.get(),client::_redis_connect_cb))
            return false;
		if(0!=redisAsyncSetDisconnectCallback(ctx_.get(),client::_redis_disconnect_cb))
            return false;
        return true;
    }
private:  
    std::unique_ptr<redisAsyncContext, void (*)(redisAsyncContext *)> 
        ctx_{nullptr, [](redisAsyncContext * ac){
            if(ac)
                redisAsyncFree(ac);
        }}; 
#ifdef __AREDIS_SSL__
    std::unique_ptr<redisSSLContext, void (*)(redisSSLContext* )> 
        ssl_ctx_{nullptr, [](redisSSLContext* ssl){
            if(ssl)
                redisFreeSSLContext(ssl);
        }};
#endif//__AREDIS_SSL__
    std::function<void(derived_t&, int status)> on_connected_;
    std::function<void(derived_t&, int status)> on_disconnected_;

};
}
}

#endif//__AREDIS_CLIENT_VOID_HPP__

