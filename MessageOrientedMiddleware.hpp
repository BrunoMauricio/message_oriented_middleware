#ifndef MOM_MOM
#define MOM_MOM

#include <functional>
#include <memory>
#include <string>
#include <cstdint>

template <class Message>
class Signal;

template <class Message>
class Subscriber;

/*
MessageOrientedMiddleware interface
*/

// Function object to hold the Message handlers
template <class Message>
using _handle_fptr = std::function<void(std::shared_ptr<Message>)>;

// handle_fptr is only supposed to be used inside Message templated classes/functions of MOM
#define handle_fptr _handle_fptr<Message>


template <class Message>
class MessageOrientedMiddleware{
    public:
    Signal<Message>* poRootSignal;
    
    // Start the middleware
    MessageOrientedMiddleware();
    ~MessageOrientedMiddleware();

    /**
     * @brief Retrieve a signal
     * @param oSignalGUID The target child signals' GUID
     * @return either a pointer to the found signal, or nullptr
     */
    Signal<Message>* poGetSignal(std::string oSignalGUID);
    
    /**
     * @brief Create a signal and return it
     * @param oSignalGUID The target child signals' GUID
     * @return a pointer to the found signal
     */
    Signal<Message>* poSetSignal(std::string oSignalGUID);

    /**
     * @brief Subscribe a simple handler (or nullptr to use the returned subscriber object instead) to a signal
     * @param oSignalGUID The signals' GUID
     * @param pfHandler If any, a pointer to the function responsible for handling the Message
     * @return the resulting subscriber object
     */
    std::shared_ptr<Subscriber<Message>> poSubscribe(std::string oSignalGUID, void (*pfHandler)(std::shared_ptr<Message>) = nullptr);

    /**
     * @brief Subscribe an objects' method handler to a signal
     * Implementation is here on purpose so that the compiler has all information necessary
     * @param oSignalGUID The signals' GUID
     * @param pfHandler The method handler
     * @param poObjectReference The object reference required to launch the appropriate method handler
     * @return the resulting subscriber object
     */
    template <class T>
    std::shared_ptr<Subscriber<Message>> poSubscribe(std::string oSignalGUID, void (T::*pfHandler)(std::shared_ptr<Message>), T* poObjectReference){
        // Retrieve target signal
        Signal<Message>* poSignal = poGetSignal(oSignalGUID);
        if(nullptr != poSignal){
            // Bind function pointer and object reference into std::function via std::bind
            return poSignal->poSubscribe(std::bind(pfHandler, poObjectReference, std::placeholders::_1));
        }
        return nullptr;
    }
    
    /**
     * @brief Publish a Message to all subscribers in a signal
     * @param oSignalGUID The signals' GUID
     * @param poMessage The Message to send
     * @return the resulting subscriber object
     */
    void vPublish(std::string oSignalGUID, Message* poMessage);
};

#include "MessageOrientedMiddleware_impl.hpp"

#endif