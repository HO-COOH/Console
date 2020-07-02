
struct Event
{
    ////////////////////////////////////////////////////////////
    /// \brief Keyboard event parameters (KeyPressed, KeyReleased)
    ///
    ////////////////////////////////////////////////////////////
    enum class KeyEvent
    {
        Up, Down, Left, Right
    };

    ////////////////////////////////////////////////////////////
    /// \brief Text event parameters (TextEntered)
    ///
    ////////////////////////////////////////////////////////////
    struct TextEvent
    {
        char code; ///< UTF-32 Unicode value of the character
    };

    ////////////////////////////////////////////////////////////
    /// \brief Mouse move event parameters (MouseMoved)
    ///
    ////////////////////////////////////////////////////////////
    struct MouseMoveEvent
    {
        int x; ///< X position of the mouse pointer, relative to the left of the owner window
        int y; ///< Y position of the mouse pointer, relative to the top of the owner window
    };

    ////////////////////////////////////////////////////////////
    /// \brief Mouse buttons events parameters
    ///        (MouseButtonPressed, MouseButtonReleased)
    ///
    ////////////////////////////////////////////////////////////
    enum class MouseButton
    {
        Left, Middle, Right
    };
    struct MouseButtonEvent
    {
        MouseButton button; ///< Code of the button that has been pressed
        int           x;      ///< X position of the mouse pointer, relative to the left of the owner window
        int           y;      ///< Y position of the mouse pointer, relative to the top of the owner window
    };

    ////////////////////////////////////////////////////////////
    /// \brief Mouse wheel events parameters (MouseWheelMoved)
    ///
    /// \deprecated This event is deprecated and potentially inaccurate.
    ///             Use MouseWheelScrollEvent instead.
    ///
    ////////////////////////////////////////////////////////////
    struct MouseWheelEvent
    {
        int delta; ///< Number of ticks the wheel has moved (positive is up, negative is down)
        int x;     ///< X position of the mouse pointer, relative to the left of the owner window
        int y;     ///< Y position of the mouse pointer, relative to the top of the owner window
    };

    ////////////////////////////////////////////////////////////
    /// \brief Mouse wheel events parameters (MouseWheelScrolled)
    ///
    ////////////////////////////////////////////////////////////
    struct MouseWheelScrollEvent
    {
        Mouse::Wheel wheel; ///< Which wheel (for mice with multiple ones)
        float        delta; ///< Wheel offset (positive is up/left, negative is down/right). High-precision mice may use non-integral offsets.
        int          x;     ///< X position of the mouse pointer, relative to the left of the owner window
        int          y;     ///< Y position of the mouse pointer, relative to the top of the owner window
    };


    /// \brief Enumeration of the different types of events
    ///
    enum EventType
    {
        TextEntered,            ///< A character was entered (data in event.text)
        KeyPressed,             ///< A key was pressed (data in event.key)
        KeyReleased,            ///< A key was released (data in event.key)
        MouseWheelScrolled,     ///< The mouse wheel was scrolled (data in event.mouseWheelScroll)
        MouseButtonPressed,     ///< A mouse button was pressed (data in event.mouseButton)
        MouseButtonReleased,    ///< A mouse button was released (data in event.mouseButton)
        MouseMoved,             ///< The mouse cursor moved (data in event.mouseMove)
        MouseEntered,           ///< The mouse cursor entered the area of the window (no data)
        MouseLeft,              ///< The mouse cursor left the area of the window (no data)

        Count                   ///< Keep last -- the total number of event types
    };

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    EventType type; ///< Type of the event

    union
    {
        KeyEvent              key;               ///< Key event parameters (Event::KeyPressed, Event::KeyReleased)
        TextEvent             text;              ///< Text event parameters (Event::TextEntered)
        MouseMoveEvent        mouseMove;         ///< Mouse move event parameters (Event::MouseMoved)
        MouseButtonEvent      mouseButton;       ///< Mouse button event parameters (Event::MouseButtonPressed, Event::MouseButtonReleased)
        MouseWheelEvent       mouseWheel;        ///< Mouse wheel event parameters (Event::MouseWheelMoved) (deprecated)
        MouseWheelScrollEvent mouseWheelScroll;  ///< Mouse wheel event parameters (Event::MouseWheelScrolled)
    };
};