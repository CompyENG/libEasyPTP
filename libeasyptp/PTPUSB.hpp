#ifndef LIBPTP_PP_PTPUSB_H_
#define LIBPTP_PP_PTPUSB_H_

#include <libusb-1.0/libusb.h>
#include "IPTPComm.hpp"

namespace PTP {
    
    class PTPUSB : public IPTPComm {
        private:
            libusb_device_handle *handle;
            int usb_error;
            struct libusb_interface_descriptor *intf;
            uint8_t ep_in;
            uint8_t ep_out;
            bool open(libusb_device * dev);
            static libusb_device * find_first_camera();
            void init();
            static int inst_count;
            
        public:
            PTPUSB();
            PTPUSB(libusb_device * dev);
            ~PTPUSB();
            void connect_to_first();
            void connect_to_serial_no(std::string serial);
            virtual bool _bulk_write(const unsigned char * bytestr, const int length, const int timeout);
            virtual bool _bulk_read(unsigned char * data_out, const int size, int * transferred, const int timeout);
            virtual bool is_open();
            void close();
    };
    
}

#endif /* LIBPTP_PP_PTPUSB_H_ */
