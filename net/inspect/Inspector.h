

#ifndef MUDUO_NET_INSPECT_INSPECTOR_H
#define MUDUO_NET_INSPECT_INSPECTOR_H

#include "../../base/Mutex.h"
#include "../../net/http/HttpRequest.h"
#include "../../net/http/HttpServer.h"

#include <map>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>



class ProcessInspector;
class PerformanceInspector;
class SystemInspector;

// An internal inspector of the running process, usually a singleton.
// Better to run in a separated thread, as some method may block for seconds
class Inspector : boost::noncopyable
{
 public:
  typedef std::vector<string> ArgList;
  typedef boost::function<string (HttpRequest::Method, const ArgList& args)> Callback;
  Inspector(EventLoop* loop,
            const InetAddress& httpAddr,
            const string& name);
  ~Inspector();

  /// Add a Callback for handling the special uri : /mudule/command
  void add(const string& module,
           const string& command,
           const Callback& cb,
           const string& help);
  void remove(const string& module, const string& command);

 private:
  typedef std::map<string, Callback> CommandList;
  typedef std::map<string, string> HelpList;

  void start();
  void onRequest(const HttpRequest& req, HttpResponse* resp);

  HttpServer server_;
  boost::scoped_ptr<ProcessInspector> processInspector_;
  boost::scoped_ptr<PerformanceInspector> performanceInspector_;
  boost::scoped_ptr<SystemInspector> systemInspector_;
  MutexLock mutex_;
  std::map<string, CommandList> modules_;
  std::map<string, HelpList> helps_;
};



#endif  // MUDUO_NET_INSPECT_INSPECTOR_H
