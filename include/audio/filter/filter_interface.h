//
// Created by skaldi on 06.03.26.
//

#pragma once

#include "../../core/dependency_tracker.h"

#include "../../utility/utility.h"


namespace age::audio::effect
{
    class slot;
}

namespace age::audio::filter
{
    class filter_interface
    {
    public:
        friend class effect::slot;

        filter_interface() noexcept = default;

        virtual ~filter_interface() { notify_death(); }

        filter_interface(const filter_interface&) = delete;
        filter_interface& operator=(const filter_interface&) = delete;

        filter_interface(filter_interface&& other) noexcept = default;
        filter_interface& operator=(filter_interface&& other) noexcept = default;

    public:
        uint32_t get_handle() const { return m_handle; }

    protected:
        virtual void init() = 0;

        bool ensure_handle();

    private:
        static uint32_t gen_handle();
        static void delete_handle(uint32_t value);

        void register_slot(effect::slot* value) { m_tracker.add(value); }
        void remove_slot(effect::slot* value)   { m_tracker.remove(value); }
        void update_slot_address(effect::slot* old_addr, effect::slot* new_addr) { m_tracker.update_address(old_addr, new_addr); }

        void notify_death();

        core::dependency_tracker<effect::slot, 8> m_tracker;

        unique_handle<uint32_t, delete_handle> m_handle{};
    };
}
