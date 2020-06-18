#ifndef _JOIN_CHANNEL_HPP_
#define _JOIN_CHANNEL_HPP_

#include <memory>
#include "JoinChannelImp.h"

namespace pipert {

template <class T>
class Packet;

template <class T1, class T2>
class JoinChannel {
 public:
  /*Channel(const std::function<void(Packet<T>)>& callback, int bufferSize)
          :	imp_(new ChannelImp<T>(callback, bufferSize))
  {
  }*/

  JoinChannel(JoinChannelImp<T1, T2>* imp);

  JoinChannelImp<T1, T2>* GetChannelImp();

  void WriteFst(Packet<T1> packet);

  void WriteSnd(Packet<T2> packet);

  void Execute();

  const std::string& GetName() const;

  int GetSize() const;

 protected:
  std::shared_ptr<JoinChannelImp<T1, T2>> imp_;
};

template <class T1, class T2>
JoinChannel<T1, T2>::JoinChannel(JoinChannelImp<T1, T2>* imp) : imp_(imp) {}

template <class T1, class T2>
JoinChannelImp<T1, T2>* JoinChannel<T1, T2>::GetChannelImp() {
  return imp_.get();
}

template <class T1, class T2>
void JoinChannel<T1, T2>::WriteFst(Packet<T1> packet) {
  imp_->WriteFst(packet);
}

template <class T1, class T2>
void JoinChannel<T1, T2>::WriteSnd(Packet<T2> packet) {
  imp_->WriteSnd(packet);
}

template <class T1, class T2>
void JoinChannel<T1, T2>::Execute() {
  imp_->Execute();
}

template <class T1, class T2>
const std::string& JoinChannel<T1, T2>::GetName() const {
  return imp_->GetName();
}

template <class T1, class T2>
int JoinChannel<T1, T2>::GetSize() const {
  return imp_->GetSize();
}

}  // namespace pipert

#endif  //_JOIN_CHANNEL_HPP_
