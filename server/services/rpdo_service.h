#pragma once

#ifdef MCUDRV_C28X

#include "../impl/impl_server.h"
#include <emblib/chrono.h>

namespace ucanopen {

class RpdoService {
private:
    impl::Server& server_;

    struct Message {
        emb::chrono::milliseconds timeout;
        emb::chrono::milliseconds timepoint;
        bool unhandled;
        can_payload payload;
        void(*handler)(const can_payload&);
    };
    emb::array<Message, 4> rpdo_msgs_;
public:
    RpdoService(impl::Server& server);
    void register_rpdo(CobRpdo rpdo,
                       emb::chrono::milliseconds timeout,
                       void (*handler)(const can_payload&),
                       can_id id = 0);
    void recv_frame(Cob cob);
    void handle_recv_frames();

    bool good(CobRpdo rpdo) const {
        const size_t idx = rpdo.underlying_value();
        if (rpdo_msgs_[idx].timeout.count() <= 0) {
            return true;
        }
        if (emb::chrono::steady_clock::now() <=
            (rpdo_msgs_[idx].timepoint + rpdo_msgs_[idx].timeout)) {
            return true;
        }
        return false;
    }
};

} // namespace ucanopen

#endif
