/**
 * 一个IO线程，多个计算thread的模式
 */


#include "sudoku.h"

#include "../../base/Atomic.h"
#include "../../base/Logging.h"
#include "../../base/Thread.h"
#include "../../base/ThreadPool.h"
#include "../../net/EventLoop.h"
#include "../../net/InetAddress.h"
#include "../../net/TcpServer.h"
#include "../../net/Buffer.h"

#include <boost/bind.hpp>

#include <utility>

#include <stdio.h>
#include <unistd.h>


class SudokuServer
{
public:
    SudokuServer(EventLoop* loop, const InetAddress& listenAddr, int numThreads)
            : server_(loop, listenAddr, "SudokuServer"),  // socket bind
              numThreads_(numThreads),
              startTime_(Timestamp::now())
    {
        server_.setConnectionCallback(boost::bind(&SudokuServer::onConnection, this, _1));
        server_.setMessageCallback(boost::bind(&SudokuServer::onMessage, this, _1, _2, _3));
    }

    void start()
    {
        LOG_INFO << "starting " << numThreads_ << " threads.";
        threadPool_.start(numThreads_);                      //使用固定大小线程池   启动工作线程
        server_.start();                                     //listen
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_TRACE << conn->peerAddress().toIpPort() << " -> "
                  << conn->localAddress().toIpPort() << " is "
                  << (conn->connected() ? "UP" : "DOWN");
    }
    /**
     * 处理协议格式，并调用 solveSudoku() 求解问题
     * @param conn
     * @param buf
     */
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
    {
        LOG_DEBUG << conn->name();
        size_t len = buf->readableBytes();
        while (len >= kCells + 2)
        {
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                string request(buf->peek(), crlf);
                buf->retrieveUntil(crlf + 2);
                len = buf->readableBytes();
                if (!processRequest(conn, request))// call solveSudoku() and then will 投放到工作线程池中执行
                {
                    conn->send("Bad Request!\r\n");
                    conn->shutdown();
                    break;
                }
            }
            else if (len > 100) // id + ":" + kCells + "\r\n"
            {
                conn->send("Id too long!\r\n");
                conn->shutdown();
                break;
            }
            else
            {
                break;
            }
        }
    }

    bool processRequest(const TcpConnectionPtr& conn, const string& request)
    {
        string id;
        string puzzle;
        bool goodRequest = true;

        string::const_iterator colon = find(request.begin(), request.end(), ':');
        if (colon != request.end())
        {
            id.assign(request.begin(), colon);
            puzzle.assign(colon+1, request.end());
        }
        else
        {
            puzzle = request;
        }

        if (puzzle.size() == implicit_cast<size_t>(kCells))
        {
            threadPool_.run(boost::bind(&solve, conn, puzzle, id));//投放到工作线程池中执行
        }
        else
        {
            goodRequest = false;
        }
        return goodRequest;
    }

    static void solve(const TcpConnectionPtr& conn, const string& puzzle, const string& id)
    {
        LOG_DEBUG << conn->name();
        string result = solveSudoku(puzzle);
        if (id.empty())
        {
            conn->send(result+"\r\n");
        }
        else
        {
            conn->send(id+":"+result+"\r\n");
        }
    }

    TcpServer server_;
    ThreadPool threadPool_; //使用固定大小线程池 threadPool.threads_
    int numThreads_;
    Timestamp startTime_;
};



int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
    int numThreads = 0;
    if (argc > 1)
    {
        numThreads = atoi(argv[1]);
    }
    EventLoop loop; //主线程的loop
    InetAddress listenAddr(9981);
    SudokuServer server(&loop, listenAddr, numThreads);

    server.start(); // 1.TcpServer::start()启动工作线程   2.server_.start(); 主线程启动监听

    loop.loop();    // 启动主线程(也就是控制线程)
}

