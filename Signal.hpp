#ifndef MOM_SIGNAL
#define MOM_SIGNAL

#include <vector>
#include <string>

#include "Subscriber.hpp"
#include "MessageOrientedMiddleware.hpp"

/*
A data structure tree (one parent, multiple children)
Each Signal is a node, and is uniquely identified by its' full path
*/
template <class Message>
class Signal{
    // The local signal identifier (Locally Unique ID)
    std::string m_oSignalLUID;
    
    // The parent signal, only nullptr for the root
    struct Signal<Message>* m_poParentSignal = nullptr;

    // The signal children for this signal
    std::vector<Signal<Message>*> m_poChildrenSignalVector;

    // All subscribers in this signal
    std::vector<std::shared_ptr<Subscriber<Message>>> m_oSubscriberVector;
    
    public:

    /**
     * @brief Tries to find the child with the received LUID
     * @param oChildSignalLUID The target child signals' LUID
     * @return either a pointer to the found signal, or nullptr
     */
    Signal<Message>* poGetChild(std::string oChildSignalLUID);

    /**
     * @brief Creates a child in case it doesn't exist already
     * @param oChildSignalLUID The target child signals' LUID
     * @return a pointer to the created signal (if already existing, finds and returns the signal)
     */
    Signal<Message>* poSetChild(std::string oChildSignalLUID);
    

    /**
     * @brief Returns the Locally Unique ID for this signal
     * @return a string representation of the LUID
     */
    std::string oGetLUID();

    /**
     * @brief Creates a signal
     * @param poParentSignal A pointer to this signals' pointer
     * @param oChildSignalLUID This signals' LUID
     * @return a pointer to the created signal (if already existing, finds and returns the signal)
     */
    Signal(Signal<Message>* poParentSignal, std::string oChildSignalLUID);
    ~Signal();

    /**
     * @brief Publishes the Message to the Subscribers of this Signal
     * @param poMessage The Message that is wrapped in a shared_ptr and sent to the subscribers
     */
    void vPublish(Message* poMessage);

    /**
     * @brief Performs a subscription to this signal
     * @param fpHandler If any, the function pointer that handles the Messages
     * @return the appropriate subscriber object (if fpHandler == nullptr, the return value can be ignored)
     */
    std::shared_ptr<Subscriber<Message>> poSubscribe(handle_fptr fpHandler = nullptr);

    /**
     * @brief Returns the full Globally Unique ID for this signal by calling the oGetLUID of it's parent
     * @return a string representation of the GUID
     */
    std::string oGetGUID();

};

#include "Signal_impl.hpp"

#endif