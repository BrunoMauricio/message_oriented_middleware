#include "Signal.hpp"

#include <iostream>

//                  Signal


template <class Message>
Signal<Message>::Signal(Signal<Message>* poParentSignal, std::string oChildSignalLUID)
:m_poParentSignal(poParentSignal), m_oSignalLUID(oChildSignalLUID)
{}

template <class Message>
Signal<Message>::~Signal()
{
    // For each signal child
    auto itChildSignal = m_poChildrenSignalVector.begin();
    while (itChildSignal != m_poChildrenSignalVector.end()){
        // Trigger child destructor
        delete *itChildSignal;
        // Remove child from children vector
        itChildSignal = m_poChildrenSignalVector.erase(itChildSignal);
    }
}

template <class Message>
std::string Signal<Message>::oGetLUID(){
    return m_oSignalLUID;
}

template <class Message>
Signal<Message>* Signal<Message>::poGetChild(std::string oChildSignalLUID){
    // Cycle over children
    for(Signal<Message>* poChildSignal : m_poChildrenSignalVector){
        // Find corresponding child LUID
        if(poChildSignal->m_oSignalLUID == oChildSignalLUID){
            return poChildSignal;
        }
    }
    return nullptr;
}

template <class Message>
Signal<Message>* Signal<Message>::poSetChild(std::string oChildSignalLUID){
    // Find child, in case it exxists
    Signal<Message>* poSignal = poGetChild(oChildSignalLUID);
    if(nullptr == poSignal){
        // Create new child signal
        poSignal = new Signal(this, oChildSignalLUID);
        // Add to children signal vector
        m_poChildrenSignalVector.push_back(poSignal);
    }
    return poSignal;
}

template <class Message>
void Signal<Message>::vPublish(Message* poMessage){
    // Setup shared pointer for new Message
    // EXPLICITLY give the pointer ownership to the shared_ptr
    std::shared_ptr<Message> poSharedMessage{poMessage};

    // Iterate over all subscribers
    auto itSubscriber = m_oSubscriberVector.begin();
    while (itSubscriber != m_oSubscriberVector.end()){
        
        // Holder for subscriber shared pointer
        std::shared_ptr<Subscriber<Message>>& sub = *itSubscriber;

        // A returned subscriber object should have been stored (aka, there is no handler) and
        // The subscriber is only present in the subscriber vector (count == 1)
        if(sub->m_pfSubscriptionHandle == nullptr && sub.use_count() == 1){
            // Release subscriber shared pointer
            itSubscriber = m_oSubscriberVector.erase(itSubscriber);
            continue;
        }

        sub->vHandle(poSharedMessage);
        
        // Next Subscriber
        ++itSubscriber;
    }
}

template <class Message>
std::shared_ptr<Subscriber<Message>> Signal<Message>::poSubscribe(handle_fptr fpHandler){
    // Create a new subscriber with the appropriate handler
    std::shared_ptr<Subscriber<Message>> poSubscriber (new Subscriber(fpHandler));
    // Store subscriber in this signal
    m_oSubscriberVector.push_back(poSubscriber);
    // Return subscriber
    return poSubscriber;
}


template <class Message>
std::string Signal<Message>::oGetGUID(){
    // On root
    if(nullptr == m_poParentSignal){
        // Just return LUID
        return m_oSignalLUID;
    }
    // Return parents' GUID + this signals' LUID
    return m_poParentSignal->oGetGUID() + "." + m_oSignalLUID;
}
