#include "uv_client.hpp"
#include <random>
#include <thread>
#include <unistd.h>
#include <atomic>
#include <mutex>
using redis_client = aredis::uv_client<std::mutex>;
std::atomic_bool stop{false};
void _alloc_cb(uv_handle_t* handle,
                            size_t suggested_size,
                            uv_buf_t* buf)
{
    *buf = uv_buf_init((char*) malloc(100), 100);
}
void _read_cb(uv_stream_t* stream,
                           ssize_t nread,
                           const uv_buf_t* buf)
{
    if (nread < 0){

    } else if (nread > 0) {
        if(strncmp("stop", buf->base, 4)==0)
        {
            stop=true;
            auto cli = (redis_client *)stream->data;
            uv_read_stop(stream);
            cli->close();
            // uv_stop(stream->loop);
        }
    }

    // OK to free buffer as write_data copies it.
    if (buf->base)
        free(buf->base);
    
}

int main(int argc, char** argv) {
    uv_loop_t* loop = uv_default_loop();
    std::random_device rd;  // 将用于为随机数引擎获得种子
    std::mt19937 gen(rd()); // 以播种标准 mersenne_twister_engine
    std::uniform_int_distribution<> dis(1, 6);
    
    redis_client cli;
    cli.connect(*loop, "127.0.0.1", 6379);
    uv_tty_t tty;
    uv_tty_init(loop, &tty, 0, 1);
    tty.data=&cli;

    uv_read_start((uv_stream_t* )&tty,_alloc_cb,_read_cb);


    std::thread t([&]{

        while(!stop){
            cli.post([](redisReply * reply){ 
                printf("SET (binary API): %s\n", reply->str);

            }, "SET %d %ld", dis(gen), time(NULL));
            usleep(50);
        }
        printf("thread t stoped!");
    });

    std::thread tt([&]{

        while(!stop){
            auto key = dis(gen);
            cli.post([key](redisReply * reply){ 
                printf("GET %d: %s\n", key, reply->str);

            }, "GET %d", key);
            usleep(30);
        }
        printf("thread tt stoped!");
    });

    uv_run(loop, UV_RUN_DEFAULT);
    t.join();
    tt.join();
}