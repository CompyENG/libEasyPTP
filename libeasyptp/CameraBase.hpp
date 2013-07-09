#ifndef LIBPTP_PP_CAMERABASE_H_
#define LIBPTP_PP_CAMERABASE_H_

#include <libusb-1.0/libusb.h>

namespace PTP {
    
    class PTPContainer;
    class IPTPComm;

    class CameraBase {
        private:
            IPTPComm * protocol;
            uint32_t _transaction_id;
            void init();
            
        protected:
            int get_and_increment_transaction_id(); // What a beautiful name for a function
            
        public:
            CameraBase();
            CameraBase(IPTPComm * protocol);
            ~CameraBase();
            void set_protocol(IPTPComm * protocol);
            bool reopen();
            int send_ptp_message(const PTPContainer& cmd, const int timeout=0);
            void recv_ptp_message(PTPContainer& out, const int timeout=0);
            void ptp_transaction(PTPContainer& cmd, PTPContainer& data, const bool receiving, PTPContainer& out_resp, PTPContainer& out_data, const int timeout=0);
    };
}

#endif /* LIBPTP_PP_CAMERABASE_H_ */
