//
// Created by skaldi on 06.03.26.
//

#pragma once

#include "../../core/dependency_tracker.h"
#include "../../utility/utility.h"

namespace age::audio::effect
{
    class slot;

    class effect_interface
    {
    public:
        friend class slot;

        effect_interface() noexcept = default;

        effect_interface(const effect_interface&) = delete;
        effect_interface& operator=(const effect_interface&) = delete;

        effect_interface(effect_interface&& other) noexcept = default;
        effect_interface& operator=(effect_interface&& other) noexcept = default;

        virtual ~effect_interface() { notify_death(); }

    public:
        void register_slot(slot* value) { m_tracker.add(value); }

    protected:
        virtual void init() = 0;

        bool ensure_handle();
        uint32_t get_handle() const { return m_handle; }

    private:
        static uint32_t gen_handle();
        static void delete_handle(uint32_t value);

        void notify_death();
        void remove_slot(slot* value) { m_tracker.remove(value); }
        void update_slot_address(slot* old_addr, slot* new_addr) { m_tracker.update_address(old_addr, new_addr); }

        core::dependency_tracker<slot, 8> m_tracker;

        unique_handle<uint32_t, delete_handle> m_handle{};
    };
}