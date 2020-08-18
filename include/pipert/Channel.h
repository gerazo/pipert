#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <memory>
#include "ChannelImp.h"

namespace pipert {

template <class T>
class Packet;

//Channel should be created by just Scheduler (friend?)

template <class T>
class Channel {
 public:
  Channel(Channel<T>* imp);

  void Write(Packet<T> packet);

  void Execute();

  const std::string& GetName() const;

  int GetSize() const;

 protected:
  std::shared_ptr<ChannelImp<T>> imp_;
};

template <class T>
Channel<T>::Channel(Channel<T>* imp) : imp_(imp) {}

template <class T>
void Channel<T>::Write(Packet<T> packet) {
  imp_->Write(packet);
}

template <class T>
void Channel<T>::Execute() {
  imp_->Execute;
}

template <class T>
const std::string& Channel<T>::GetName() const {
  return imp_->GetName();
}

template <class T>
int Channel<T>::GetSize() const {
  return imp_->GetSize();
}

}  // namespace pipert

#endif  //_CHANNEL_H_
