#pragma once

// External Includes
#include <nap/service.h>
#include <nap/signalslot.h>

#include "messagebase.h"

namespace nap
{
	class NAPAPI MessageService : public Service
	{
		RTTI_ENABLE(Service)
	public:
		// Default Constructor
		MessageService(ServiceConfiguration* configuration) : Service(configuration)	{ }

		/**
		 * Subscribe to a message of type T. T must be derived from WTMessageBase.
		 * @tparam T must be derived from WTMessageBase
		 * @param slot reference to the slot that should be triggered when a message of type T is dispatched.
		 */
		template<typename T>
		void subscribeToMessage(Slot<const T&>& slot);

		/**
		 * Unsubscribe from a message of type T. T must be derived from WTMessageBase.
		 * @tparam T must be derived from WTMessageBase
		 * @param slot reference to the slot that should no longer be triggered when a message of type T is dispatched.
		 */
		template<typename T>
		void unsubscribeFromMessage(Slot<const T&>& slot);

		/**
		 * Dispatch a message of type T to all slots that are subscribed to this message type.
		 * @tparam T must be derived from WTMessageBase
		 * @param message reference to the message of type T that should be dispatched.
		 */
		template<typename T>
		void dispatchMessage(const T& message);
	private:
		/**
		 * SlotContainerBase is a base class for slot containers that can hold slots of different types.
		 */
		class SignalContainerBase
		{
			RTTI_ENABLE()
		public:
			virtual ~SignalContainerBase() = default;

			/**
			 * Use this method to check if the slot container holds a slot of type T.
			 * @tparam T the SlotContainer type
			 * @return true if the slot container holds a slot of type T, false otherwise
			 */
			template<typename T>
			bool is() const { return get_type().is_derived_from<T>(); }

			/**
			 * Use this method to get the slot container as a specific type T. Asserts if the slot container does not hold a slot of type T.
			 * @tparam T the SlotContainer type
			 * @return reference to the slot container as type T
			 */
			template<typename T>
			T& as()
			{
				assert(is<T>());
				return static_cast<T&>(*this);
			}
		};

		/**
		 * @tparam T the message type that this slot container holds
		 */
		template<typename T>
		class SignalContainer : public SignalContainerBase
		{
			RTTI_ENABLE(SignalContainerBase)
		public:
			/**
			 * Trigger the slot with a message of type T.
			 * @param message const reference to the message of type T that should be triggered.
			 */
			void trigger(const T& message)
			{
				mSignal.trigger(message);
			}

			/**
			 * Connect a slot to this signal container.
			 * @param slot slot that should be connected to the signal in this signal container.
			 */
			void connect(Slot<const T&>& slot)
			{
				mSignal.connect(slot);
			}

			/**
			 * Disconnect a slot from this signal container.
			 * @param slot reference to the slot that should be disconnected from the signal in this signal container.
			 */
			void disconnect(Slot<const T&>& slot)
			{
				mSignal.disconnect(slot);
			}

		private:
			Signal<const T&> mSignal;
		};

		std::unordered_map<rtti::TypeInfo, std::unique_ptr<SignalContainerBase>> mSignalContainers;
	};


	template<typename T>
  void MessageService::subscribeToMessage(Slot<const T&>& slot)
	{
		// find slot of type
		auto it = mSignalContainers.find(RTTI_OF(T));
		if (it == mSignalContainers.end())
		{
			// create new slot container
			auto container = std::make_unique<SignalContainer<T>>();
			mSignalContainers[RTTI_OF(T)] = std::move(container);
		}

		// connect
		auto& signal_container = mSignalContainers[RTTI_OF(T)];
		assert(signal_container->get_type().is_derived_from(RTTI_OF(SignalContainer<T>)));
		auto& container = signal_container->template as<SignalContainer<T>>();
		container.connect(slot);
	}


	template<typename T>
	void MessageService::unsubscribeFromMessage(Slot<const T&>& slot)
	{
		// find slot of type
		auto it = mSignalContainers.find(RTTI_OF(T));
		if (it != mSignalContainers.end())
		{
			// remove slot from container
			auto& signal_container = it->second;

			assert(signal_container->get_type().is_derived_from(RTTI_OF(SignalContainer<T>)));
			auto& container = signal_container->template as<SignalContainer<T>>();
			container.disconnect(slot);
		}
	}


	template <typename T>
	void MessageService::dispatchMessage(const T& message)
	{
		assert(message.get_type().is_derived_from(RTTI_OF(MessageBase)));
		auto it = mSignalContainers.find(RTTI_OF(T));
		if (it != mSignalContainers.end())
		{
			// remove slot from container
			auto& signal_container = it->second;

			assert(signal_container->get_type().is_derived_from(RTTI_OF(SignalContainer<T>)));
			auto& container = signal_container->template as<SignalContainer<T>>();
			container.trigger(message);
		}
	}
}
