#pragma once

#include <ucanopen-c28x/ucanopen_def.hpp>
#include <emblib/chrono.hpp>
#include <emblib/static_vector.hpp>

namespace ucanopen {

class Server;

class Node {
    friend class Server;
private:
    Server* server_;

    struct RxMessage {
        uint32_t obj_id;
        emb::chrono::milliseconds timeout;
        emb::chrono::milliseconds timepoint;
        bool unhandled;
        canpayload_t payload;
        uint16_t len;
        void (*handler)(const canpayload_t&);
    };
    emb::static_vector<RxMessage, 4> rx_msgs_;

    struct TxMessage {
        uint32_t obj_id;
        emb::chrono::milliseconds period;
        emb::chrono::milliseconds timepoint;
        uint16_t len;
        canpayload_t (*creator)();
    };
    emb::static_vector<TxMessage, 4> tx_msgs_;

    virtual void register_messages() = 0;
public:
    Node() {}
    virtual ~Node() {}
    void recv(uint32_t obj_id);
    void send();
    void handle();

    virtual void inspect() {}
    bool good() const;
protected:
    void register_rx_message(canid_t id,
                             uint16_t len,
                             emb::chrono::milliseconds timeout,
                             void (*handler)(const canpayload_t&));
    void register_tx_message(canid_t id,
                             uint16_t len,
                             emb::chrono::milliseconds period,
                             canpayload_t (*creator)());
};

} // namespace ucanopen
