#include "dispatcher_lite.h"

#include <examples/protobuf/codec/query.pb.h>

#include <iostream>

using std::cout;
using std::endl;

void onUnknownMessageType(const TcpConnectionPtr&,const MessagePtr& message,Timestamp)
{
    cout << "onUnknownMessageType: " << message->GetTypeName() << endl;
}

void onQuery(const TcpConnectionPtr&,const MessagePtr& message,Timestamp)
{
    cout << "onQuery: " << message->GetTypeName() << endl;
    boost::shared_ptr<Engine::Query> query = down_pointer_cast<Engine::Query>(message);
    assert(query != NULL);
}

void onAnswer(const TcpConnectionPtr&, const MessagePtr& message, Timestamp)
{
    cout << "onAnswer: " << message->GetTypeName() << endl;
    boost::shared_ptr<Engine::Answer> answer = down_pointer_cast<Engine::Answer>(message);
    assert(answer != NULL);
}

int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    ProtobufDispatcherLite dispatcher(onUnknownMessageType);
    dispatcher.registerMessageCallback(Engine::Query::descriptor(), onQuery);
    dispatcher.registerMessageCallback(Engine::Answer::descriptor(), onAnswer);

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

