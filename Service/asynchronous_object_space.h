#ifndef ASYNCOBJECTSPACE_H
#define ASYNCOBJECTSPACE_H

#include <QThread>

#include "i_async_wrapper.h"

class AsyncObjectSpace final {
 private:
  QThread Thread;
  std::vector<std::unique_ptr<IAsyncWrapper>> Managers;

 public:
  explicit AsyncObjectSpace();
  ~AsyncObjectSpace();

 private:
  Q_DISABLE_COPY_MOVE(AsyncObjectSpace)

 private:
  void createWrappers(void);
};

#endif  // ASYNCOBJECTSPACE_H
