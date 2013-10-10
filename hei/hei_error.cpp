///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Dimitry Ishenko
// Distributed under the GNU GPL v2. For full terms please visit:
// http://www.gnu.org/licenses/gpl.html
//
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "hei_error.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace hei
{

///////////////////////////////////////////////////////////////////////////////////////////////////
const std::error_category& hei_category()
{
    static class hei_category instance;
    return instance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string hei_category::message(int ev) const
{
    switch(errc(ev))
    {
    case errc::null                   : return "No Error";
    case errc::not_implemented        : return "Function not implemented";
    case errc::ver_mismatch           : return "Version passed to function not correct for library";
    case errc::unsupported_transport  : return "Supplied transport not supported";
    case errc::invalid_device         : return "Supplied device is not valid";
    case errc::buffer_too_small       : return "Supplied buffer is too small";
    case errc::zero_bytes_received    : return "Zero bytes were returned in the packet";
    case errc::timeout                : return "Timeout error";
    case errc::unsupported_protocol   : return "Supplied protocol not supported";
    case errc::ip_addr_not_initialized: return "The devices IP address has not be set";
    case errc::null_transport         : return "No transport specified";
    case errc::ipx_not_installed      : return "IPX Transport not installed";
    case errc::ipx_open_socket        : return "Error opening IPX Socket";
    case errc::no_packet_driver       : return "No packet driver found";
    case errc::crc_mismatch           : return "CRC did not match";
    case errc::allocation_error       : return "Memory allocation error failed";
    case errc::no_ipx_cache           : return "No cache has been allocated for IPX";
    case errc::invalid_request        : return "Invalid request";
    case errc::no_response            : return "No response was available/requested";
    case errc::invalid_response       : return "Invalid format response was received";
    case errc::data_too_large         : return "Given data is too large";
    case errc::load_proc_error        : return "Error loading procedures";
    case errc::not_loaded             : return "Attempted command before successfull OpenTransport";
    case errc::alignment_error        : return "Data not aligned on proper boundary";
    case errc::file_not_open          : return "File not open";

    case errc::icmp_pkt_found         : return "HEIE_ICMP_PKT_FOUND";
    case errc::arp_pkt_found          : return "HEIE_ARP_PKT_FOUND";
    case errc::type_not_handled       : return "HEIE_TYPE_NOT_HANDLED";
    case errc::link_sense_triggered   : return "HEIE_LINK_SENSE_TRIGGERED";
    case errc::unk_ip_packet          : return "HEIE_UNK_IP_PACKET";
    case errc::unk_ethertype          : return "HEIE_UNK_ETHERTYPE";
    case errc::unk_packet_type        : return "HEIE_UNK_PACKET_TYPE";
    case errc::unk_802x_packet_type   : return "HEIE_UNK_802X_PACKET_TYPE";
    case errc::unk_llc_type           : return "HEIE_UNK_LLC_TYPE";
    case errc::crc_does_not_match     : return "HEIE_CRC_DOES_NOT_MATCH";
    case errc::crc_no_data            : return "HEIE_CRC_NO_DATA";
    case errc::enet_addr_reprogrammed : return "HEIE_ENET_ADDR_REPROGRAMMED";
    case errc::null_data_pointer      : return "HEIE_NULL_DATA_POINTER";
    case errc::size_error             : return "HEIE_SIZE_ERROR";
    case errc::not_found              : return "HEIE_NOT_FOUND";
    case errc::invalid_type           : return "HEIE_INVALID_TYPE";
    case errc::ram_already_locked     : return "HEIE_RAM_ALREADY_LOCKED";
    case errc::invalid_request_int    : return "HEIE_INVALID_REQUEST_INT";
    case errc::timeout_error          : return "HEIE_TIMEOUT_ERROR";
    case errc::flash_program_error    : return "HEIE_FLASH_PROGRAM_ERROR";
    case errc::invalid_os             : return "HEIE_INVALID_OS";
    case errc::invalid_location       : return "HEIE_INVALID_LOCATION";
    case errc::invalid_slot_number    : return "HEIE_INVALID_SLOT_NUMBER";
    case errc::invalid_data           : return "HEIE_INVALID_DATA";
    case errc::module_busy            : return "HEIE_MODULE_BUSY";
    case errc::channel_failure        : return "HEIE_CHANNEL_FAILURE";
    case errc::unused_channels_exist  : return "HEIE_UNUSED_CHANNELS_EXIST";
    case errc::invalid_udp_port       : return "HEIE_INVALID_UDP_PORT";
    case errc::shutdown_os            : return "HEIE_SHUTDOWN_OS";
    case errc::not_my_ip_address      : return "HEIE_NOT_MY_IP_ADDRESS";
    case errc::protection_error       : return "HEIE_PROTECTION_ERROR";
    case errc::unk_type_error         : return "HEIE_UNK_TYPE_ERROR";
    case errc::backplane_init_error   : return "HEIE_BACKPLANE_INIT_ERROR";
    case errc::unk_response           : return "HEIE_UNK_RESPONSE";
    case errc::unk_rxwx_format        : return "HEIE_UNK_RXWX_FORMAT";
    case errc::unk_ack                : return "HEIE_UNK_ACK";
    case errc::unk_nak                : return "HEIE_UNK_NAK";
    case errc::range_error            : return "HEIE_RANGE_ERROR";
    case errc::length_warning         : return "HEIE_LENGTH_WARNING";
    case errc::invalid_base_number    : return "HEIE_INVALID_BASE_NUMBER";
    case errc::invalid_module_type    : return "HEIE_INVALID_MODULE_TYPE";
    case errc::invalid_offset         : return "HEIE_INVALID_OFFSET";
    case errc::invalid_boot_ver_for_os: return "HEIE_INVALID_BOOT_VER_FOR_OS";
    case errc::broken_transmitter     : return "HEIE_BROKEN_TRANSMITTER";
    case errc::invalid_address        : return "HEIE_INVALID_ADDRESS";
    case errc::timing                 : return "HEIE_TIMING";
    case errc::channel_failure_multi  : return "HEIE_CHANNEL_FAILURE_MULTI";
    case errc::serial_setup_error     : return "HEIE_SERIAL_SETUP_ERROR";
    case errc::not_initialized        : return "HEIE_NOT_INITIALIZED";
    case errc::invalid_mode           : return "HEIE_INVALID_MODE";
    case errc::comm_failure           : return "HEIE_COMM_FAILURE";
    case errc::operation_aborted      : return "HEIE_OPERATION_ABORTED";
    case errc::invalid_rx_char        : return "HEIE_INVALID_RX_CHAR";
    case errc::request_naked          : return "HEIE_REQUEST_NAKED";
    case errc::invalid_operation      : return "HEIE_INVALID_OPERATION";
    case errc::val_already_used       : return "HEIE_VAL_ALREADY_USED";
    case errc::module_not_responding  : return "HEIE_MODULE_NOT_RESPONDING";
    case errc::base_changed           : return "I/O Base has changed";
    case errc::module_failure         : return "HEIE_MODULE_FAILURE";
    case errc::parity_error           : return "HEIE_PARITY_ERROR";
    case errc::framing_error          : return "HEIE_FRAMING_ERROR";
    case errc::over_run_error         : return "HEIE_OVER_RUN_ERROR";
    case errc::buffer_overflow        : return "HEIE_BUFFER_OVERFLOW";
    case errc::abort                  : return "HEIE_ABORT";
    case errc::busy                   : return "HEIE_BUSY";
    case errc::drive_trip             : return "Drive has tripped";
    case errc::command_pending        : return "HEIE_COMMAND_PENDING";

    case errc::user_error             : return std::string();

    default                           : return "Unknown error";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
}
