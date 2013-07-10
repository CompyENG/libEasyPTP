/**
 * Copyright 2013 Bobby Graese <bobby.graese@gmail.com>
 * 
 * This file is part of libEasyPTP.
 *
 *  libEasyPTP is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  libEasyPTP is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with libEasyPTP.  If not, see
 *  <http://www.gnu.org/licenses/>.
 */

#ifndef LIBEASYPTP_CAMERABASE_H_
#define LIBEASYPTP_CAMERABASE_H_

#include <libusb-1.0/libusb.h>

namespace EasyPTP {
    
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

#endif /* LIBEASYPTP_CAMERABASE_H_ */
