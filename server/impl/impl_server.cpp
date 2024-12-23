#ifdef MCUDRV_C28X

#include <ucanopen/c28x/server/impl/impl_server.hpp>

namespace ucanopen {

impl::Server::Server(mcu::c28x::can::Module& can_module,
                     NodeId node_id,
                     ODEntry* object_dictionary,
                     size_t object_dictionary_size)
        : node_id_(node_id),
          can_module_(can_module),
          dictionary_(object_dictionary),
          dictionary_size_(object_dictionary_size) {
    nmt_state_ = NmtState::initializing;
    init_message_objects();
    init_object_dictionary();
}

void impl::Server::init_message_objects() {
    for (size_t i = 0; i < cob_count; ++i) {
        message_objects_.push_back(mcu::c28x::can::MessageObject());
    }

    for (size_t i = 0; i < cob_count; ++i) {
        message_objects_[i].obj_id = i;
        message_objects_[i].frame_id = calculate_cob_id(Cob(i), this->node_id_);
        message_objects_[i].frame_type = CAN_MSG_FRAME_STD;
        message_objects_[i].frame_idmask = 0;
        message_objects_[i].data_len = cob_data_len[i];
    }

    message_objects_[Cob::emcy].obj_type
            = message_objects_[Cob::tpdo1].obj_type
            = message_objects_[Cob::tpdo2].obj_type
            = message_objects_[Cob::tpdo3].obj_type
            = message_objects_[Cob::tpdo4].obj_type
            = message_objects_[Cob::tsdo].obj_type
            = message_objects_[Cob::heartbeat].obj_type
            = CAN_MSG_OBJ_TYPE_TX;

    message_objects_[Cob::nmt].obj_type
            = message_objects_[Cob::sync].obj_type
            = message_objects_[Cob::time].obj_type
            = message_objects_[Cob::rpdo1].obj_type
            = message_objects_[Cob::rpdo2].obj_type
            = message_objects_[Cob::rpdo3].obj_type
            = message_objects_[Cob::rpdo4].obj_type
            = message_objects_[Cob::rsdo].obj_type
            = CAN_MSG_OBJ_TYPE_RX;

    message_objects_[Cob::emcy].flags
            = message_objects_[Cob::tpdo1].flags
            = message_objects_[Cob::tpdo2].flags
            = message_objects_[Cob::tpdo3].flags
            = message_objects_[Cob::tpdo4].flags
            = message_objects_[Cob::tsdo].flags
            = message_objects_[Cob::heartbeat].flags
            = CAN_MSG_OBJ_NO_FLAGS;

    message_objects_[Cob::nmt].flags
            = message_objects_[Cob::sync].flags
            = message_objects_[Cob::time].flags
            = message_objects_[Cob::rpdo1].flags
            = message_objects_[Cob::rpdo2].flags
            = message_objects_[Cob::rpdo3].flags
            = message_objects_[Cob::rpdo4].flags
            = message_objects_[Cob::rsdo].flags
            = CAN_MSG_OBJ_RX_INT_ENABLE;

    for (int i = 1; i < cob_count; ++i) {
        // count from 1 - skip dummy COB
        this->can_module_.setup_message_object(message_objects_[i]);
    }
}

void impl::Server::init_object_dictionary() {
    assert(dictionary_ != NULL);

    std::sort(dictionary_, dictionary_ + dictionary_size_);

    // Check OBJECT DICTIONARY correctness
    for (size_t i = 0; i < dictionary_size_; ++i) {
        // OD is sorted
        if (i < (dictionary_size_ - 1)) {
            assert(dictionary_[i] < dictionary_[i+1]);
        }

        for (int j = i+1; j < dictionary_size_; ++j) {
            // no od-entries with equal {index, subinex}
            assert((dictionary_[i].key.index != dictionary_[j].key.index)
                || (dictionary_[i].key.subindex != dictionary_[j].key.subindex));

            // no od-entries with equal {category, subcategory, name}
            bool categoryEqual = ((strcmp(dictionary_[i].object.category, dictionary_[j].object.category) == 0) ? true : false);
            bool subcategoryEqual = ((strcmp(dictionary_[i].object.subcategory, dictionary_[j].object.subcategory) == 0) ? true : false);
            bool nameEqual = ((strcmp(dictionary_[i].object.name, dictionary_[j].object.name) == 0) ? true : false);
            assert(!categoryEqual || !subcategoryEqual || !nameEqual);
        }

        if (dictionary_[i].object.has_read_permission()) {
            assert((dictionary_[i].object.read_func != OD_NO_INDIRECT_READ_ACCESS)
                || (dictionary_[i].object.ptr != OD_NO_DIRECT_ACCESS));
        }

        if (dictionary_[i].object.has_write_permission()) {
            assert(dictionary_[i].object.write_func != OD_NO_INDIRECT_WRITE_ACCESS
               || (dictionary_[i].object.ptr != OD_NO_DIRECT_ACCESS));
        }

        if (dictionary_[i].object.default_value.has_value()) {
            ODObjectDataType data_type = dictionary_[i].object.data_type;
            assert(data_type != OD_EXEC && data_type != OD_STRING);
        }
    }
}

} // namespace ucanopen

#endif
