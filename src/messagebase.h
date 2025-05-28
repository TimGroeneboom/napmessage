#pragma once

#include <nap/core.h>

namespace nap
{
    //////////////////////////////////////////////////////////////////////////

    /**
     * MessageBase is a base class for all messages that can be dispatched by the MessageService.
     */
    class NAPAPI MessageBase
    {
        RTTI_ENABLE()
    public:
        virtual ~MessageBase() = default;

        /**
         * Use this method to get the type of the message.
         * @tparam T must be derived from MessageBase
         * @return true if this message is of type T, false otherwise
         */
        template<typename T>
        bool is() const { return get_type().is_derived_from<T>(); }

        /**
         * Use this method to get the message as a specific type T. Asserts if the message is not of type T.
         * @tparam T must be derived from MessageBase
         * @return reference to this message as type T. Asserts if the message is not of type T.
         */
        template<typename T>
        T& as()
        {
            assert(is<T>());
            return static_cast<T&>(*this);
        }
    };
}