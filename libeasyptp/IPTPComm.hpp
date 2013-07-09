#ifndef LIBPTP_PP_IPTPCOMM_H_
#define LIBPTP_PP_IPTPCOMM_H_

namespace PTP {

    /**
     * @class IPTPComm
     * @brief An interface containing basic methods for writing and reading PTP data
     * 
     * With \c IPTPComm, it's possible to extend libptp++ to communicate over 
     * essentially any protocol.  This class serves as an interface for the 
     * underlying implementation.  You can then pass a class that implements 
     * \c IPTPComm to \c PTPCamera or \c CHDKCamera and change the underlying 
     * protocol.
     * 
     * Depending on the protocol, you'll probably need some additional methods to do
     * some communication setup.  Note that \c CameraBase will only use this to send
     * and receive data, and not perform any of the setup for you.  You *must* set
     * up the connection on your own before passing a communication protocol to
     * \c CameraBase, or something could fail.
     * 
     * @todo I'd like to ship libptp++ with at least a libusb(x) implementaiton of
     *       IPTPComm
     * @todo Socket implementation of IPTPComm?
     * @todo Common exceptions that can be used in any implementation?
     */ 
    class IPTPComm {
        public:
            virtual ~IPTPComm() { }
            /**
             * @brief Check that we have open communication
             * 
             * @return True if we can _bulk_write and _bulk_read, false otherwise
             */
            virtual bool is_open() = 0;
            /**
             * @brief Write data to the protocol
             * 
             * _bulk_write handles writing data to the protocol.  The first 
             * parameter is the data that needs to be written, followed by the 
             * length of that data, and optionally a timeout parameter.  
             * 
             * @return true if the data was successfully written, or false
             * if there was a problem
             * @todo Common exceptions
             */
            virtual bool _bulk_write(const unsigned char * bytestr, const int length, const int timeout=0) = 0;
            /**
             * @brief Read data from the protocol
             * 
             * _bulk_read handles reading data from the protocol.  It must return
             * this data as a pointer to an unsigned character (allocated via
             * new unsigned char[], so that it can be delete[]-ed later).  Size is 
             * the amount of data we would like to read, and transferred is how much
             * gets actually transferred (it is possible to have transferred < size).
             * Optionally, a timeout can also be provided.
             * 
             * @return true if the data was read successfully, false if there was a
             * problem
             * @todo Common exceptions
             */
            virtual bool _bulk_read(unsigned char * data_out, const int size, int * transferred, const int timeout=0) = 0;
    };

}

#endif /* LIBPTP_PP_IPTPCOMM_H_ */
