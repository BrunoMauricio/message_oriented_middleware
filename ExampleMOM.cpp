#include "MessageOrientedMiddleware.hpp"

// Example message object
struct ActualMessage{
    uint8_t* message;
    uint32_t message_size;
    int a = 0;

    ActualMessage(
    uint8_t* _message,
    uint32_t _message_size){
        message = _message;
        message_size = _message_size;
    }

    ~ActualMessage(){
        printf("PAYLOAD BEING CLEARED %d %p\n", a, this);
    }

    /*
    Deleting any sort of copy or move operation is useful to prevent lost memory, but unnecessary
    since MOM uses shared_ptr internally
    */
    ActualMessage(){}; // so we cannot accidentally delete it via pointers
    ActualMessage(const ActualMessage&) = delete; // no copies
    ActualMessage& operator=(const ActualMessage&) = delete; // no self-assignments
    ActualMessage(ActualMessage&&) = delete; // WHY?
    ActualMessage& operator=(ActualMessage&&) = delete; // WHY?

};

// Example 
class A{
    public:
    void vPrint(std::shared_ptr<ActualMessage> m){
        if(nullptr == m){
            return;
        }
        string oMessageString = "Message handler 1 with message("+to_string(m->message_size)+") : ";
        char pcHexaBuffer[5];
        for(int i = 0; i < m->message_size; i++){
            snprintf(pcHexaBuffer, 5, "0x%02x ", m->message[i]);
            oMessageString += std::string(pcHexaBuffer)+" ";
        }
        printf("%s\n", oMessageString.c_str());
    }
};

template <class Message>
void vPrint(std::shared_ptr<Message> m){
    if(nullptr == m){
        return;
    }
    string oMessageString = "Message handler 2 with message("+to_string(m->message_size)+") : ";
    char pcHexaBuffer[5];
    for(int i = 0; i < m->message_size; i++){
        snprintf(pcHexaBuffer, 5, "0x%02x ", m->message[i]);
        oMessageString += std::string(pcHexaBuffer)+" ";
    }
    printf("%s\n", oMessageString.c_str());
}


int main(int argc, char* argv[]){
    // Random "message" payload
    uint8_t a[5] = {1,2,3,4,5};
    // Create the MOM object
    MessageOrientedMiddleware<ActualMessage>* oSignalMiddleware = new MessageOrientedMiddleware<ActualMessage>();
    // There should be no signals registered (should print (nil))
    printf("%p\n", oSignalMiddleware->poGetSignal("A.Cool.Signal"));

    // Let us register this signal
    oSignalMiddleware->poSetSignal("A.Cool.Signal");
    // Now it should print the signals' pointer
    printf("%p\n", oSignalMiddleware->poGetSignal("A.Cool.Signal"));

    // Subscribe a normal function to the created signal
    oSignalMiddleware->poSubscribe("A.Cool.Signal", vPrint);
    // By publishing a signal to it, the message "Message handler 2 with message(5) : 0x01 0x02 0x03 0x04 0x05 " should appear
    oSignalMiddleware->vPublish("A.Cool.Signal", new ActualMessage(a, 5));

    // Now let's create another signal
    oSignalMiddleware->poSetSignal("a.b.c.def");

    // This time add both a function
    oSignalMiddleware->poSubscribe("a.b.c.def", vPrint);
    // And a non-static object method
    A l;
    // As expected, the non-static method also requires an object reference
    oSignalMiddleware->poSubscribe<A>("a.b.c.def", &A::vPrint, &l);
    // And subscribe via a subscriber object
    std::shared_ptr<Subscriber<ActualMessage>> poSubscriber = oSignalMiddleware->poSubscribe("a.b.c.def");

    // Publish a couple of messages, which should pop up for both handlers
    oSignalMiddleware->vPublish("a.b.c.def", new ActualMessage(a, 4));
    oSignalMiddleware->vPublish("a.b.c.def", new ActualMessage(a, 3));

    // Those messages are also present inside the subscriber queue and can be retrieve with:
    printf("Subscriber queue status: %s\n", poSubscriber->bHasMessage() ? "Not Empty" : "Empty");
    printf("Retrieving messages from subscriber\n");
    vPrint<ActualMessage>(poSubscriber->poGetMessage());
    vPrint<ActualMessage>(poSubscriber->poGetMessage());
    printf("Subscriber queue status: %s\n", poSubscriber->bHasMessage() ? "Not Empty" : "Empty");


    delete oSignalMiddleware;
}