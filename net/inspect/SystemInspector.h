
#ifndef MUDUO_NET_INSPECT_SYSTEMINSPECTOR_H
#define MUDUO_NET_INSPECT_SYSTEMINSPECTOR_H

#include "Inspector.h"



class SystemInspector : boost::noncopyable
{
 public:
  void registerCommands(Inspector* ins);

  static string overview(HttpRequest::Method, const Inspector::ArgList&);
  static string loadavg(HttpRequest::Method, const Inspector::ArgList&);
  static string version(HttpRequest::Method, const Inspector::ArgList&);
  static string cpuinfo(HttpRequest::Method, const Inspector::ArgList&);
  static string meminfo(HttpRequest::Method, const Inspector::ArgList&);
  static string stat(HttpRequest::Method, const Inspector::ArgList&);
};



#endif  // MUDUO_NET_INSPECT_SYSTEMINSPECTOR_H
