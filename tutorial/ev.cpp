#include "ev_client.hpp"
#include <random>
#include <thread>
#include <unistd.h>
#include <atomic>
#include <mutex>
using redis_client = aredis::ev_client<std::mutex>;
std::atomic_bool stop{false};


static void stdin_cb (EV_P_ ev_io *w, int revents)
{
    
    puts ("stdin ready");
    char buf[100];
    auto size = read(0, buf, 100);
    if(strncmp("stop", buf, 4)==0)
    {
        stop=true;
        auto cli = (redis_client *)stream->data;
        // for one-shot events, one must manually stop the watcher
        // with its corresponding stop function.
        ev_io_stop (EV_A_ w);
        cli->close();
        // this causes all nested ev_run's to stop iterating
        // ev_break (EV_A_ EVBREAK_ALL);
    }
    

    
}

int main(int argc, char** argv) {
    struct ev_loop *loop = EV_DEFAULT;
    std::random_device rd;  // 将用于为随机数引擎获得种子
    std::mt19937 gen(rd()); // 以播种标准 mersenne_twister_engine
    std::uniform_int_distribution<> dis(1, 6);
    
    redis_client cli;
    cli.connect(loop, "127.0.0.1", 6379);
    ev_io stdin_watcher;
     // initialise an io watcher, then start it
     // this one will watch for stdin to become readable
    ev_io_init (&stdin_watcher, stdin_cb, /*STDIN_FILENO*/ 0, EV_READ);
    
    stdin_watcher.data=&cli;

    ev_io_start (loop, &stdin_watcher);

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

    ev_run (loop, 0);
    t.join();
    tt.join();
}