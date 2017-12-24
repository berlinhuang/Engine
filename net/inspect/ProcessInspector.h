
#ifndef MUDUO_NET_INSPECT_PROCESSINSPECTOR_H
#define MUDUO_NET_INSPECT_PROCESSINSPECTOR_H

#include "Inspector.h"
#include <boost/noncopyable.hpp>



class ProcessInspector : boost::noncopyable
{
 public:
  void registerCommands(Inspector* ins);

  static string overview(HttpRequest::Method, const Inspector::ArgList&);
  static string pid(HttpRequest::Method, const Inspector::ArgList&);
  static string procStatus(HttpRequest::Method, const Inspector::ArgList&);
  static string openedFiles(HttpRequest::Method, const Inspector::ArgList&);
  static string threads(HttpRequest::Method, const Inspector::ArgList&);

  static string username_;
};



#endif  // MUDUO_NET_INSPECT_PROCESSINSPECTOR_H
