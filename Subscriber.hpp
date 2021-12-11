#ifndef MOM_SUBSCRIBER
#define MOM_SUBSCRIBER

#include <cstdint>
#include <cstring>
#include <memory>
#include <deque>
#include <unistd.h>

#include "MessageOrientedMiddleware.hpp"

template <class Message>
class Signal;

/*
A Subscriber to a Signal
Can either be a function handler, called with each publish call
to the respective Signal (push notifications)
Or a queue that holds the relevant Messages (pull notifications)
*/
template <class Message>
class Subscriber{
    friend class Signal<Message>;

    // The queue for the messages
    std::deque<std::shared_ptr<Message>> m_oMessageQueue;

    // The function pointer for the messages
    handle_fptr m_pfSubscriptionHandle;
    
    public:

    /**
     * @brief Setup the subscriber
     * @param fpHandler The handler for a function that will handle the received Message shared pointers
     * @return The subscriber, which should only be used if the fpHandler == nullptr ()
     * as the queue is only filled if that is the case
     */
    Subscriber(handle_fptr fpHandler = nullptr);

    ~Subscriber();


    /**
     * @brief Launches the function handler, in case it exists, with the received Message
     * @param poMessage The shared pointer for the published Message
     */
    void vHandle(std::shared_ptr<Message> poMessage);

    /**
     * @brief In case the handler is nullptr, the queue will begin to be filled.
     * This function returns the last Message inside the queue
     * @return The last Message in the queue
     */
    std::shared_ptr<Message> poGetMessage();

    /**
     * @brief Whether there are Message to be read from the queue
     * @return True if there are Messages to be read from the queue
     */
    bool bHasMessage();

    /**
     * @brief Getter for the existing function pointer
     * @return The stored function pointer to handle the messages
     */
    handle_fptr fpHandler();
};

#include "Subscriber_impl.hpp"

#endif