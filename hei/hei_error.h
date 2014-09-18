///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013-2014 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef HEI_ERROR_H
#define HEI_ERROR_H

///////////////////////////////////////////////////////////////////////////////////////////////////
#include <system_error>
#include <string>
#include <hei.h>

#undef min
#undef max

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace hei
{

///////////////////////////////////////////////////////////////////////////////////////////////////
enum class errc
{
    null                       = HEIE_NULL,
    not_implemented            = HEIE_NOT_IMPLEMENTED,
    ver_mismatch               = HEIE_VER_MISMATCH,
    unsupported_transport      = HEIE_UNSUPPORTED_TRANSPORT,
    invalid_device             = HEIE_INVALID_DEVICE,
    buffer_too_small           = HEIE_BUFFER_TOO_SMALL,
    zero_bytes_received        = HEIE_ZERO_BYTES_RECEIVED,
    timeout                    = HEIE_TIMEOUT,
    unsupported_protocol       = HEIE_UNSUPPORTED_PROTOCOL,
    ip_addr_not_initialized    = HEIE_IP_ADDR_NOT_INITIALIZED,
    null_transport             = HEIE_NULL_TRANSPORT,
    ipx_not_installed          = HEIE_IPX_NOT_INSTALLED,
    ipx_open_socket            = HEIE_IPX_OPEN_SOCKET,
    no_packet_driver           = HEIE_NO_PACKET_DRIVER,
    crc_mismatch               = HEIE_CRC_MISMATCH,
    allocation_error           = HEIE_ALLOCATION_ERROR,
    no_ipx_cache               = HEIE_NO_IPX_CACHE,
    invalid_request            = HEIE_INVALID_REQUEST,
    no_response                = HEIE_NO_RESPONSE,
    invalid_response           = HEIE_INVALID_RESPONSE,
    data_too_large             = HEIE_DATA_TOO_LARGE,
    load_proc_error            = HEIE_LOAD_PROC_ERROR,
    not_loaded                 = HEIE_NOT_LOADED,
    alignment_error            = HEIE_ALIGNMENT_ERROR,
    file_not_open              = HEIE_FILE_NOT_OPEN,

    icmp_pkt_found             = HEIE_ICMP_PKT_FOUND,
    arp_pkt_found              = HEIE_ARP_PKT_FOUND,
    type_not_handled           = HEIE_TYPE_NOT_HANDLED,
    link_sense_triggered       = HEIE_LINK_SENSE_TRIGGERED,
    unk_ip_packet              = HEIE_UNK_IP_PACKET,
    unk_ethertype              = HEIE_UNK_ETHERTYPE,
    unk_packet_type            = HEIE_UNK_PACKET_TYPE,
    unk_802x_packet_type       = HEIE_UNK_802X_PACKET_TYPE,
    unk_llc_type               = HEIE_UNK_LLC_TYPE,
    crc_does_not_match         = HEIE_CRC_DOES_NOT_MATCH,
    crc_no_data                = HEIE_CRC_NO_DATA,
    enet_addr_reprogrammed     = HEIE_ENET_ADDR_REPROGRAMMED,
    null_data_pointer          = HEIE_NULL_DATA_POINTER,
    size_error                 = HEIE_SIZE_ERROR,
    not_found                  = HEIE_NOT_FOUND,
    invalid_type               = HEIE_INVALID_TYPE,
    ram_already_locked         = HEIE_RAM_ALREADY_LOCKED,
    invalid_request_int        = HEIE_INVALID_REQUEST_INT,
    timeout_error              = HEIE_TIMEOUT_ERROR,
    flash_program_error        = HEIE_FLASH_PROGRAM_ERROR,
    invalid_os                 = HEIE_INVALID_OS,
    invalid_location           = HEIE_INVALID_LOCATION,
    invalid_slot_number        = HEIE_INVALID_SLOT_NUMBER,
    invalid_data               = HEIE_INVALID_DATA,
    module_busy                = HEIE_MODULE_BUSY,
    channel_failure            = HEIE_CHANNEL_FAILURE,
    unused_channels_exist      = HEIE_UNUSED_CHANNELS_EXIST,
    invalid_udp_port           = HEIE_INVALID_UDP_PORT,
    shutdown_os                = HEIE_SHUTDOWN_OS,
    not_my_ip_address          = HEIE_NOT_MY_IP_ADDRESS,
    protection_error           = HEIE_PROTECTION_ERROR,
    unk_type_error             = HEIE_UNK_TYPE_ERROR,
    backplane_init_error       = HEIE_BACKPLANE_INIT_ERROR,
    unk_response               = HEIE_UNK_RESPONSE,
    unk_rxwx_format            = HEIE_UNK_RXWX_FORMAT,
    unk_ack                    = HEIE_UNK_ACK,
    unk_nak                    = HEIE_UNK_NAK,
    range_error                = HEIE_RANGE_ERROR,
    length_warning             = HEIE_LENGTH_WARNING,
    invalid_base_number        = HEIE_INVALID_BASE_NUMBER,
    invalid_module_type        = HEIE_INVALID_MODULE_TYPE,
    invalid_offset             = HEIE_INVALID_OFFSET,
    invalid_boot_ver_for_os    = HEIE_INVALID_BOOT_VER_FOR_OS,
    broken_transmitter         = HEIE_BROKEN_TRANSMITTER,
    invalid_address            = HEIE_INVALID_ADDRESS,
    timing                     = HEIE_TIMING,
    channel_failure_multi      = HEIE_CHANNEL_FAILURE_MULTI,
    serial_setup_error         = HEIE_SERIAL_SETUP_ERROR,
    not_initialized            = HEIE_NOT_INITIALIZED,
    invalid_mode               = HEIE_INVALID_MODE,
    comm_failure               = HEIE_COMM_FAILURE,
    operation_aborted          = HEIE_OPERATION_ABORTED,
    invalid_rx_char            = HEIE_INVALID_RX_CHAR,
    request_naked              = HEIE_REQUEST_NAKED,
    invalid_operation          = HEIE_INVALID_OPERATION,
    val_already_used           = HEIE_VAL_ALREADY_USED,
    module_not_responding      = HEIE_MODULE_NOT_RESPONDING,
    base_changed               = HEIE_BASE_CHANGED,
    module_failure             = HEIE_MODULE_FAILURE,
    parity_error               = HEIE_PARITY_ERROR,
    framing_error              = HEIE_FRAMING_ERROR,
    over_run_error             = HEIE_OVER_RUN_ERROR,
    buffer_overflow            = HEIE_BUFFER_OVERFLOW,
    abort                      = HEIE_ABORT,
    busy                       = HEIE_BUSY,
    drive_trip                 = HEIE_DRIVE_TRIP,
    command_pending            = HEIE_COMMAND_PENDING,

    user_error                 = 0x7fff
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class hei_category: public std::error_category
{
public:
    const char* name() const noexcept override { return "hei"; }
    std::string message(int ev) const override;
};

const std::error_category& hei_category();

///////////////////////////////////////////////////////////////////////////////////////////////////
inline std::error_code make_error_code(hei::errc e)
{ return std::error_code(int(e), hei_category()); }

inline std::error_condition make_error_condition(hei::errc e)
{ return std::error_condition(int(e), hei_category()); }

///////////////////////////////////////////////////////////////////////////////////////////////////
class hei_error: public std::system_error
{
public:
    hei_error(int code): std::system_error(std::error_code(code, hei::hei_category())) { }
    hei_error(int code, const std::string& message): std::system_error(std::error_code(code, hei::hei_category()), message) { }

    hei_error(hei::errc code): std::system_error(std::error_code(static_cast<int>(code), hei::hei_category())) { }
    hei_error(hei::errc code, const std::string& message): std::system_error(std::error_code(static_cast<int>(code), hei::hei_category()), message) { }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace std
{
    template<>
    struct is_error_code_enum<hei::errc>: public true_type { };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#endif // HEI_ERROR_H
