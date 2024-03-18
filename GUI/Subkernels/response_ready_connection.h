#ifndef RESPONSEREADYCONNECTION_H
#define RESPONSEREADYCONNECTION_H

#include "sql_query_values.h"

class ResponseReadyConnection
{
 public:
  explicit ResponseReadyConnection() = default;
  virtual ~ResponseReadyConnection() = default;

 private:
  virtual void connectSignal_ResponseReady(void) = 0;
};

#endif  // RESPONSEREADYCONNECTION_H
