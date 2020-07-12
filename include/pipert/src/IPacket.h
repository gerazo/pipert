#ifndef _IPACKET_HPP_
#define _IPACKET_HPP_


namespace pipert {

template <class T>
class IPacket {
 public:
  using Time = uint64_t;

  virtual Packet<T> Clone() const = 0;

  virtual T GetData() const = 0;
  virtual const T* GetDataConstPtr() const = 0;
  virtual T* GetDataPtr() = 0;
  virtual Time GetCreatedTime() const = 0;
  virtual Time GetCurrentTime() const = 0;

  virtual void SetCreatedTime(Time created_at) = 0;

  virtual const PacketImp<T>* GetImp() const = 0;

 private:
};

}  // namespace pipert

#endif  //_PACKET_HPP_