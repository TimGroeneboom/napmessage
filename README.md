# napmessage

## summary

Use this module to send and receive messages across your application. Message system is not thread-safe and messages are expected to be dispatched from the main thread.

Use the message service to subscribe // unsubscribe to messages. To create a message you must extend on `MessageBase`

## Example

Define custom message, for example :

```
/**
 * WTFocusMessage is a message that contains information about the focus position in the world.
 * Subscribe to this message to receive updates about the focus position. This is userful for camera interaction
 */
class NAPAPI WTFocusMessage : public MessageBase
{
    RTTI_ENABLE(MessageBase)
public:
    glm::vec3 mPosition; ///< Property: 'Position' the position in world space where the focus is located
    glm::vec3 mDirection; ///< Property: 'Direction' the direction of the focus, usually the forward vector of the camera
};
```

Then you can dispatch the message through the `MessageService` like this :

```
WTFocusMessage message;
message.mPosition = { 0.0, 1.0, 0.0 }
message.mDirection = { 1.0, 0.0, 0.0 }
mMessageService->dispatchMessage(message);
```

To subscribe to a message, define a slot and callback like this :

```
void onFocusMessage(const WTFocusMessage& focusMessage);
Slot<const WTFocusMessage&> mFocusMessageSlot = { this, &WTControlApp::onFocusMessage};
```

And subscibe like this :

```
mMessageService->subscribeToMessage(mFocusMessageSlot);
```
# napmessage
