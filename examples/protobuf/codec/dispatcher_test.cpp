#include "dispatcher.h"

#include "query.pb.h"

#include <iostream>

using std::cout;
using std::endl;

typedef boost::shared_ptr<Engine::Query> QueryPtr;
typedef boost::shared_ptr<Engine::Answer> AnswerPtr;

void test_down_pointer_cast()
{
    ::boost::shared_ptr<google::protobuf::Message> msg(new Engine::Query);
    ::boost::shared_ptr<Engine::Query> query(down_pointer_cast<Engine::Query>(msg));
    assert(msg && query);
    if (!query)
    {
        abort();
    }
}

void onQuery(const TcpConnectionPtr&,
             const QueryPtr& message,
             Timestamp)
{
    cout << "onQuery: " << message->GetTypeName() << endl;
}

void onAnswer(const TcpConnectionPtr&,
              const AnswerPtr& message,
              Timestamp)
{
    cout << "onAnswer: " << message->GetTypeName() << endl;
}

void onUnknownMessageType(const TcpConnectionPtr&,
                          const MessagePtr& message,
                          Timestamp)
{
    cout << "onUnknownMessageType: " << message->GetTypeName() << endl;
}

int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    test_down_pointer_cast();

    ProtobufDispatcher dispatcher(onUnknownMessageType);
    dispatcher.registerMessageCallback<Engine::Query>(onQuery);
    dispatcher.registerMessageCallback<Engine::Answer>(onAnswer);

    TcpConnectionPtr conn;
    Timestamp t;

    boost::shared_ptr<Engine::Query> query(new Engine::Query);
    boost::shared_ptr<Engine::Answer> answer(new Engine::Answer);
    boost::shared_ptr<Engine::Empty> empty(new Engine::Empty);
    dispatcher.onProtobufMessage(conn, query, t);
    dispatcher.onProtobufMessage(conn, answer, t);
    dispatcher.onProtobufMessage(conn, empty, t);

    google::protobuf::ShutdownProtobufLibrary();
}

