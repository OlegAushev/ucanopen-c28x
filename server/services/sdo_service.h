#pragma once


#ifdef MCUDRV_C28X


#include "../impl/impl_server.h"
#include <emblib/algorithm.h>
#include <new>


namespace ucanopen {


class SdoService {
private:
    impl::Server& _server;

    mcu::ipc::Flag _rsdo_flag;
    mcu::ipc::Flag _tsdo_flag;
    can_payload* _rsdo_data;
    can_payload* _tsdo_data;
    static unsigned char cana_rsdo_dualcore_alloc[sizeof(can_payload)];
    static unsigned char canb_rsdo_dualcore_alloc[sizeof(can_payload)];
    static unsigned char cana_tsdo_dualcore_alloc[sizeof(can_payload)];
    static unsigned char canb_tsdo_dualcore_alloc[sizeof(can_payload)];
public:
    SdoService(impl::Server& server, const IpcFlags& ipc_flags);
    void recv();
    void send();
    void handle_received();
private:
    SdoAbortCode _read_expedited(const ODEntry* od_entry, ExpeditedSdo& tsdo, const ExpeditedSdo& rsdo);
    SdoAbortCode _write_expedited(const ODEntry* od_entry, ExpeditedSdo& tsdo, const ExpeditedSdo& rsdo);
    SdoAbortCode _restore_default_parameter(ODObjectKey key);

    static const ODObjectKey restore_default_parameter_key;
};


} // namespace ucanopen


#endif
