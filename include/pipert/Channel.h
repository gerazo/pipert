#ifndef _CHANNEL_HPP_
#define _CHANNEL_HPP_

#include <memory>
#include "ChannelImp.h"

namespace pipert {

template <class T>
class Packet;

template <class T>
class Channel {
 public:
  /*Channel(const std::function<void(Packet<T>)>& callback, int bufferSize)
          :	imp_(new ChannelImp<T>(callback, bufferSize))
  {
  }*/

  Channel(ChannelImp<T>* imp);

  ChannelImp<T>* GetChannelImp();

  void Write(Packet<T> packet);

  void Execute();

  const std::string& GetName() const;

  int GetSize() const;

 protected:
  std::shared_ptr<ChannelImp<T>> imp_;
};

template <class T>
Channel<T>::Channel(ChannelImp<T>* imp) : imp_(imp) {}

template <class T>
ChannelImp<T>* Channel<T>::GetChannelImp() {
  return imp_.get();
}

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

#endif  //_CHANNEL_HPP_
