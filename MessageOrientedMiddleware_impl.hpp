#include "MessageOrientedMiddleware.hpp"

#include <iostream>
#include <vector>

#include "Subscriber.hpp"
#include "Signal.hpp"

using namespace std;

// https://stackoverflow.com/a/46931770
// Simple string splitter by delimiter
vector<string> split (string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

//                  MessageOrientedMiddleware

template <class Message>
MessageOrientedMiddleware<Message>::~MessageOrientedMiddleware(){
    // Start cascade delete from root
    delete poRootSignal;
}


template <class Message>
MessageOrientedMiddleware<Message>::MessageOrientedMiddleware(){
    // Create root signal
    poRootSignal = new Signal<Message>(nullptr, "root");
}

template <class Message>
Signal<Message>* MessageOrientedMiddleware<Message>::poGetSignal(std::string oSignalGUID){
    Signal<Message>* poSignal;
    // Go over all signals and find the target LUID
    poSignal = poRootSignal;
    for(std::string oSignalLUID : split(oSignalGUID, ".")){
        poSignal = poSignal->poGetChild(oSignalLUID);
        if(nullptr == poSignal){
            return nullptr;
        }
    }

    return poSignal;
}

template <class Message>
Signal<Message>* MessageOrientedMiddleware<Message>::poSetSignal(std::string oSignalGUID){
    Signal<Message>* poSignal;
    // Iterate over all signals
    poSignal = poRootSignal;
    for(std::string oSignalLUID : split(oSignalGUID, ".")){
        // SetChild ignores existing children and just returns the appropriate one (or creates it)
        poSignal = poSignal->poSetChild(oSignalLUID);
    }
    
    return poSignal;
}

template <class Message>
std::shared_ptr<Subscriber<Message>> MessageOrientedMiddleware<Message>::poSubscribe(std::string oSignalGUID, void (*pfHandler)(std::shared_ptr<Message>)){
    // Retrieve the target signal
    Signal<Message>* poSignal = poGetSignal(oSignalGUID);
    if(nullptr != poSignal){
        // Subscribe and return the generated subscriber
        return poSignal->poSubscribe(pfHandler);
    }
    // In case there is no signal, returns nullptr
    return nullptr;
}


template <class Message>
void MessageOrientedMiddleware<Message>::vPublish(std::string oSignalGUID, Message* poMessage){
    // Retrieve the target signal
    Signal<Message>* poSignal = poGetSignal(oSignalGUID);
    // Either the message or the signal is null
    if(nullptr == poMessage || nullptr == poSignal){
        if(nullptr != poMessage){
            // The pointer is the responsible of this method, if it's not possible to get a signal
            // need to release the memory
            delete poMessage;
        }
        return;
    }
    // Everythings' ok, send message
    poSignal->vPublish(poMessage);
}

