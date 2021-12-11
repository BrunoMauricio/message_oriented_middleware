# Message Oriented Middleware (MOM)

Proof of concept implementation of a publisher-subscriber message delivery system in C++

## Concept
The main concepts within MOM are the signals that make it up, the subscribers that receive the messages and the messages themselves.

Signals are abstract entities that are analog to a distribution center, represented globally by its' full address or Globally Unique ID (GUID) and locally by its' Locally Unique ID (LUID).

All signals have parent signals and can have child signals, with the exception of the "root" signal, which has no parent.

A signals' GUID is the string sum of each parent signals' LUID, from the root all the way down to the signals' own LUID, with each LUID separated from each other by a dot (".").

Signal tree generation example:
```
Create signal "signal_0", "signal_1", "signal_2"
Print signals
                          root
                            .
                            |
            +---------------+--------------+
            |               |              |
        signal_0        signal_1       signal_2

Create signal "signal_0.signal_3", "signal_0.signal_4", "signal_0.signal_5"
Print signals
                          root
                            .
                            |
            +---------------+--------------+
            |               |              |
        signal_0        signal_1       signal_2
            |
  +---------+---------+
  |         |         |          .....
signal_3 signal_4  signal_5

```
The "root" is implicit in the signal path when accessing a signal by its' GUID, in any function.

MOM is a templated class which accepts any Message class as long as it has the fields "message" which is a memory address type, and a field "message_size" which has the size in bytes of the provided memory address.

On receiving a Message pointer, MOM wraps it in a shared_ptr in such a way that the object is automatically and properly deallocated when no more references exist. As such, the object should have no raw pointers, and if a raw pointer does exist before being passed into MOM, it should be "forgotten", and NEVER FREED.

Subscribers are the end entities that receive the messages in their inner queue. They work in one of two ways, pull or push.

When setting up a pull based Susbcriber, a subscriber object is retrieved from MOM, and the messages can be accessed from their queue whenever necessary.

To setup a push based Subscriber, a handler must be provided. A handler is a function with a particular signature, namely "void(std::shared_ptr<Message>)", that is called for each new Message that arrives at the subscribed Signal.

Currently, there is no multi-threading (future feature) so not only is every message delivered sequentially, but a handler only runs after the previous handlers have finished. This is not the best solution possible, but as there are several ways to go about it, I decided to leave it until the end and to eventually define a good configuration and thread management strategy.

As per many publisher-subscriber systems, MOM works with 4 main functions: GetSignal, SetSignal, Publish and Subscribe

These can be used to check/retrieve a signal, create a signal, publish a message to a signal, or subscribe to receive messages from a signal


## Usage
What follows is a short description of the MOM interface. The example in ExampleMOM.cpp, plus the comments in the .h files, should provide a better understanding of the overall functionality.

A signal can only be used after it is created. The signal getter and setter methods are:

```
Signal<Message>* poGetSignal(std::string oSignalGUID);
Signal<Message>* poSetSignal(std::string oSignalGUID);
```

Publishing a message is easy, simply call the publish method with the appropriate signal GUID and Message:

```
void vPublish(std::string oSignalGUID, Message* poMessage);
```

There are three ways to subscribe to a signal. To obtain a subscriber with a queue which can be accessed to retrieve the received Message in a "pull" strategy, simply use the following method with only the GUID parameter, **and store the subscriber object for later access**:

```
std::shared_ptr<Subscriber<Message>> poSubscribe(std::string oSignalGUID, void (*pfHandler)(std::shared_ptr<Message>) = nullptr);
```

To setup handlers in a "push" strategy, that deal with Messages as soon as they are published, use either of the following methods depending on whether the handler is a static or non-static function/method:

```
std::shared_ptr<Subscriber<Message>> poSubscribe(std::string oSignalGUID, void (*pfHandler)(std::shared_ptr<Message>) = nullptr);

template <class T>
std::shared_ptr<Subscriber<Message>> poSubscribe(std::string oSignalGUID, void (T::*pfHandler)(std::shared_ptr<Message>), T* poObjectReference){
```

For concrete implementation examples, see ExampleMOM.cpp. The output from running its' main should be similar to:

```
Message handler 2 with message(5) : 0x01 0x02 0x03 0x04 0x05 
PAYLOAD BEING CLEARED 0 0x4dccf20
Message handler 2 with message(4) : 0x01 0x02 0x03 0x04 
Message handler 1 with message(4) : 0x01 0x02 0x03 0x04 
Message handler 2 with message(3) : 0x01 0x02 0x03 
Message handler 1 with message(3) : 0x01 0x02 0x03 
Subscriber queue status: Not Empty
Retrieving messages from subscriber
Message handler 2 with message(4) : 0x01 0x02 0x03 0x04 
PAYLOAD BEING CLEARED 0 0x4dcea70
Message handler 2 with message(3) : 0x01 0x02 0x03 
PAYLOAD BEING CLEARED 0 0x4dceec0
Subscriber queue status: Empty
```

As this middleware deals with memory, it is important to run some sort of memory check tool to see if there are any leaks. Currently, running the following command shows that code that is tested is leak free.
```
g++ ExampleMOM.cpp; valgrind --leak-check=full -s ./a.out
```


## Roadmap
* Main priority
* 1) Multi-threading:
* * Allow per signal configuration of how multi-threading should be performed (wait for handler to finish? timeout after x seconds?)
* * Launch all handlers in a separate thread according to the configuration;
* * Queue incoming signals.
* 2) Priority queueing:
* * Allow publishing Messages with different levels of priority, and turn the queue into a priority queue

## Contributing
This is a proof of concept, and perhaps even useless, but I'm open to suggestions.

## Authors and acknowledgment
Authored by Bruno Mauricio (@BrunoMauricio)

## License
This project is not meant/allowed to be used anywhere. This may change in the future, on request.

## Project status
On going

