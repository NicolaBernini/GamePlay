#include "Base.h"
#include "AIAgent.h"
#include "Node.h"

namespace gameplay
{

/**
  * @brief Default Constructor 
  * @note Creates and AI State Machine associated to this instance and stores a pointer to internal field 
  */
AIAgent::AIAgent()
    : _stateMachine(NULL), _node(NULL), _enabled(true), _listener(NULL), _next(NULL)
{
    _stateMachine = new AIStateMachine(this);
}

AIAgent::~AIAgent()
{
    //** Safe Destruction simply consists of deleting and setting the pointer to NULL 
    SAFE_DELETE(_stateMachine);
}


AIAgent* AIAgent::create()
{
    //** This is a factory-like method 
    return new AIAgent();
}

const char* AIAgent::getId() const
{
    if (_node)
        return _node->getId();

    return "";
}

Node* AIAgent::getNode() const
{
    return _node;
}
    
void AIAgent::setNode(Node* node)
{
    _node = node;
}

AIStateMachine* AIAgent::getStateMachine()
{
    return _stateMachine;
}

bool AIAgent::isEnabled() const
{
    return (_node && _enabled);
}

void AIAgent::setEnabled(bool enabled)
{
    _enabled = enabled;
}

void AIAgent::setListener(Listener* listener)
{
    _listener = listener;
}

void AIAgent::update(float elapsedTime)
{
    _stateMachine->update(elapsedTime);
}

bool AIAgent::processMessage(AIMessage* message)
{
    // Handle built-in message types.
    switch (message->_messageType)
    {
    case AIMessage::MESSAGE_TYPE_STATE_CHANGE:
        {
            // Change state message
            //** @todo Is this copy necessary ? 
            const char* stateId = message->getString(0);
            if (stateId)
            {
                //** @todo Is this copy necessary ? 
                AIState* state = _stateMachine->getState(stateId);
                if (state)
                    _stateMachine->setStateInternal(state);
            }
        }
        break;
    case AIMessage::MESSAGE_TYPE_CUSTOM:
        break;
    }

    // Dispatch message to registered listener.
    //** If `_listener` exists the message is dispatched 
    if (_listener && _listener->messageReceived(message))
        return true;
        
    //** The Script in the associated Node is called passing the Received Message 
    if (_node && _node->fireScriptEvent<bool>(GP_GET_SCRIPT_EVENT(Node, messageReceived), dynamic_cast<void*>(_node), message))
        return true;

    return false;
}

}
