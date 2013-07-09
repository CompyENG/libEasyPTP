#ifndef LIBPTP_PP_CHDKCAMERA_H_
#define LIBPTP_PP_CHDKCAMERA_H_

#include <string>
#include <vector>
#include "CameraBase.hpp"

namespace PTP {
    
    class PTPContainer;
    class LVData;
    class IPTPComm;

    class CHDKCamera : public CameraBase {
        static uint8_t * _pack_file_for_upload(uint32_t * out_size, const std::string local_filename, const std::string remote_filename);
        public:
            CHDKCamera();
            CHDKCamera(IPTPComm * protocol);
            float get_chdk_version(void);
            uint32_t check_script_status(void);
            uint32_t execute_lua(const std::string script, uint32_t * script_error, const bool block=false);
            void read_script_message(PTPContainer& out_data, PTPContainer& out_resp);
            uint32_t write_script_message(const std::string message, const uint32_t script_id=0);
            bool upload_file(const std::string local_filename, const std::string remote_filename, int timeout=0);
            char * download_file(const std::string filename, const int timeout);
            void get_live_view_data(LVData& data_out, const bool liveview=true, const bool overlay=false, const bool palette=false);
            std::vector<std::string> _wait_for_script_return(const int timeout);
    };
    
}

#endif /* LIBPTP_PP_CHDKCAMERA_H_ */
