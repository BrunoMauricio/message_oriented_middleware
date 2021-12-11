#include "Subscriber.hpp"


// Subscriber

template <class Message>
Subscriber<Message>::~Subscriber(){}

template <class Message>
Subscriber<Message>::Subscriber(handle_fptr fpHandler)
: m_pfSubscriptionHandle(fpHandler) {}


template <class Message>
void Subscriber<Message>::vHandle(std::shared_ptr<Message> poMessage){
    // Should the Message be pushed to the handler or inserted into the queue
    if(nullptr != m_pfSubscriptionHandle){
        m_pfSubscriptionHandle(poMessage);
    }else{
        m_oMessageQueue.push_back(poMessage);
    }
}

template <class Message>
std::shared_ptr<Message> Subscriber<Message>::poGetMessage(){
    std::shared_ptr<Message> m = nullptr;
    if(!m_oMessageQueue.empty()){
        // Retrieve message at front of the queue
        m = m_oMessageQueue.front();
        // Remove message from queue
        m_oMessageQueue.pop_front();
    }
    return m;
}

template <class Message>
bool Subscriber<Message>::bHasMessage(){
    return !m_oMessageQueue.empty();
}

template <class Message>
handle_fptr Subscriber<Message>::fpHandler(){
    return m_pfSubscriptionHandle;
}