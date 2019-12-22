#include <iostream>
#include <array>
#include <memory>
#include <functional>
#include <vector>
#include <queue>
#include <chrono>

enum class EState
{
    Init,
    Halted,
    Running,
    Running_Interrupted,
    Undefined
};

enum class Message
{
    Empty,
    Request_Running,
    Request_Stopping,
    Request_Interrupt
};

using Guard = std::function<bool(EState, Message)>;
using Transition = std::pair<Guard,EState>;

using TransitionTable = std::vector<Transition>;

namespace Version2
{

    class State
    {
        public:
          State(std::string name, EState superState)
              : superState(superState), name(std::move(name)) {};
          void entry(){};
          void exit(){};
          EState
          getSuperState() {
              return superState;
          }
          const std::string& getName() const
          {
              return name;
          }
        
      private:
        EState superState {EState::Undefined};
        std::string name;
    };

    using StateTable = std::vector<State>;
    using TransitionTable = std::vector<std::vector<Transition>>;

    StateTable states = {State("init", EState::Undefined),
                         State("halted", EState::Undefined),
                         State("running", EState::Undefined),
                         State("running - interrupted", EState::Running)};

    Transition InitToHalted{[](EState currentState, Message m) { return true; },
                            EState::Halted};
    Transition HaltedToRunning{
        [](EState currentState, Message m) { return m == Message::Request_Running; },
        EState::Running};
    Transition RunningToHalted{
        [](EState currentState, Message m) { return m == Message::Request_Stopping; },
        EState::Halted};
    Transition RunningToInterrupted{
        [](EState currentState, Message m) { return m == Message::Request_Interrupt; },
        EState::Running_Interrupted};
    Transition InterruptedToRunning{
        [](EState currentState, Message m) { return m == Message::Request_Interrupt; },
        EState::Running};

    TransitionTable transitions = {{InitToHalted},
                                   {HaltedToRunning},
                                   {RunningToHalted, RunningToInterrupted},
                                   {InterruptedToRunning}};

    EState
    getNextState(EState current, Message m) {
        for(auto &t : transitions[static_cast<size_t>(current)]) {
            if(t.first(current, m)) {
                return t.second;
            }
        }
        return current;
    }

    class Statemachine
    {
        public:
        void run()
        {
            while(!inputEvents.empty()) {

                Message m = inputEvents.front();
                inputEvents.pop();
                EState next = getNextState(current, m);
                if(current == next)
                {
                    //try with superstate
                    EState superState = states[static_cast<size_t>(current)].getSuperState();
                    while(superState!= EState::Undefined) 
                    {
                        next = getNextState(superState, m);
                        if(next!=superState)
                            break;
                        superState = states[static_cast<size_t>(superState)].getSuperState();
                    }
                }
                current = next;
                //std::cout <<  states[static_cast<size_t>(current)].getName() << std::endl;
            }
        }

        void
        setEvent(Message m) {
            inputEvents.push(m);
        }

        private:
            EState current {EState::Init};
            std::queue<Message> inputEvents{{Message::Empty}};
    };
}

EState getNextState(EState current, Message m, const TransitionTable& transitions)
{
    for(auto &t : transitions) {
        if(t.first(current, m)) {
            return t.second;
        }
    }
    return current;
}


class State {
  public:
    State(std::string name) : name(std::move(name)) {}
    virtual ~State() = default;

    virtual EState
    evaluate(EState currentState, Message m)=0;

    const virtual std::string &
    getName() {
        return name;
    }

  private:
    std::string name;
};

class Init : public State
{
    public:
        using State::State;
        EState
        evaluate(EState currentState, Message m) final {
            return getNextState(currentState, m, transitions);
        }

    private:
        Transition InitToHalted{[](EState currentState, Message m) { return true; },
                                      EState::Halted};
        TransitionTable transitions{InitToHalted};
};

class Running: public State {
  public:
    using State::State;

    EState
    evaluate(EState currentState, Message m) override {
        return getNextState(currentState, m, transitions);
    }

  private:
    Transition RunningToHalted{[](EState currentState, Message m) {
                                   return m == Message::Request_Stopping;
                               },
                               EState::Halted};
    Transition RunningToInterrupted{
        [](EState currentState, Message m) { return m == Message::Request_Interrupt; },
        EState::Running_Interrupted};
    TransitionTable transitions{RunningToHalted, RunningToInterrupted};
};

class Running_Interrupted: public Running
{
    public:
        using Running::Running;

        EState
        evaluate(EState currentState, Message m) final {
            
            EState next = getNextState(currentState, m, transitions);
            if(next!=currentState)
            {
                return next;
            }
            return Running::evaluate(currentState, m);
        }

    private:
    Transition InterruptedToRunning{
        [](EState currentState, Message m) { return m == Message::Request_Interrupt; },
        EState::Running};
    TransitionTable transitions{InterruptedToRunning};
};

class Halted: public State {
  public:
    using State::State;
    EState
    evaluate(EState currentState, Message m) final {
        return getNextState(currentState, m, transitions);
    }

  private:
    Transition HaltedToRunning{
        [](EState currentState, Message m) { return m == Message::Request_Running; },
        EState::Running};
    TransitionTable transitions{HaltedToRunning};
};



class Statemachine
{
    public : 
    
        void run() {

            while(!inputEvents.empty()) {

                currentState =
                    states[static_cast<size_t>(currentState)]->evaluate(currentState, inputEvents.front());
                inputEvents.pop();
                //std::cout << getCurrentState() << std::endl;
            }
        }

        void setEvent(Message m)
        {
            inputEvents.push(m);
        }

        const std::string& getCurrentState()
        {
            return states[static_cast<size_t>(currentState)]->getName();
        }

    private:
    std::array<std::unique_ptr<State>, 4> states {
        std::make_unique<Init>("init"), 
        std::make_unique<Halted>("halted"), 
        std::make_unique<Running>("running"),
        std::make_unique<Running_Interrupted>("Running - interrupted")};
    EState currentState {EState::Init};
    std::queue<Message> inputEvents{{Message::Empty}};
};

int main()
{
    Statemachine s;

    auto start = std::chrono::high_resolution_clock::now();
    std::cout << s.getCurrentState() << std::endl;
    for(size_t i = 0; i < 10000; i++) {
        s.setEvent(Message::Request_Stopping);
        s.setEvent(Message::Request_Running);
        s.setEvent(Message::Request_Interrupt);
        s.setEvent(Message::Empty);
        s.setEvent(Message::Request_Stopping);
        s.setEvent(Message::Empty);
        s.setEvent(Message::Request_Interrupt);
        s.setEvent(Message::Request_Stopping);
        s.run();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "v1 (ms): " <<  elapsed.count() << std::endl;

    std::cout << "Version 2" << std::endl;

    Version2::Statemachine s2;

    start = std::chrono::high_resolution_clock::now();
    for(size_t i = 0; i<10000; i++)
    {
        s2.setEvent(Message::Request_Stopping);
        s2.setEvent(Message::Request_Running);
        s2.setEvent(Message::Request_Interrupt);
        s2.setEvent(Message::Empty);
        s2.setEvent(Message::Request_Stopping);
        s2.setEvent(Message::Empty);
        s2.setEvent(Message::Request_Interrupt);
        s2.setEvent(Message::Request_Stopping);
        s2.run();
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "v2 (ms): " << elapsed.count() << std::endl;
}