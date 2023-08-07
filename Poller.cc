#include "Poller.h"
#include "Channel.h"

namespace mymuduo
{

Poller::Poller(EventLoop* loop):ownerLoop_(loop){}

bool Poller::hasChannel(Channel *channel) const
{
    // 在ChannelMap里找epollfd，ChannelMap是在poller里面
    auto it = channels_.find(channel->fd());
    // 不等于表示存在
    return it != channels_.end() && it->second == channel;
}

}