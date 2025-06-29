
#ifndef evtemi
#define evtemi

#include <any>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
template <typename T> using EvtCallback = std::function<void(T newVal)>;
using EvtCallbackNoParam = std::function<void()>;

template <typename T> struct EventType
{
  private:
    EventType(const char *type) : eventType(type)
    {
    }

  public:
    const char *eventType;
    static EventType<T> of(const char *type)
    {
        return EventType<T>(type);
    }
};

class EventEmitter
{

  private:
    std::unordered_map<std::string, std::vector<std::any *>> listeners;

  public:
    template <typename U> void emit(EventType<U> type, U data)
    {
        auto it = listeners.find(type.eventType);
        if (it != listeners.end())
        {
            for (std::any *&cb : it->second)
            {
                EvtCallback<U> *callback = (EvtCallback<U> *)cb;
                (*callback)(data);
            }
        }
    }

    template <typename U> void on(const EventType<U> type, EvtCallback<U> cb)
    {

        listeners[type.eventType].push_back((std::any *)new EvtCallback<U>(cb));
    };
};

#endif