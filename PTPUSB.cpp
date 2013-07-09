#include <string>
#include <libusb-1.0/libusb.h>

#include "PTPUSB.hpp"
#include "libptp++.hpp"

namespace PTP {
    
int PTPUSB::inst_count = 0;
    
PTPUSB::PTPUSB() {
    this->init();
}

PTPUSB::PTPUSB(libusb_device * dev) {
    this->init();
    this->open(dev);
}

PTPUSB::~PTPUSB() {
    this->close();
    
    PTPUSB::inst_count--;
    if(PTPUSB::inst_count == 0) {
        // Be sure to exit libusb
        libusb_exit(NULL);
    }
}

void PTPUSB::init() {
    if(PTPUSB::inst_count == 0) {
        libusb_init(NULL);  // Make sure to initialize libusb first!
    }
    
    PTPUSB::inst_count++;
    this->handle = NULL;
    this->usb_error = 0;
    this->intf = NULL;
    this->ep_in = 0;
    this->ep_out = 0;
}

void PTPUSB::connect_to_first() {
    // Find the first camera
    libusb_device * dev = this->find_first_camera();
    if(dev == NULL) {
        // TODO: Throw exception
        return;
    }
    
    this->open(dev);
}

/**
 * @brief Find the first camera which is connected.
 *
 * Asks libusb for all the devices connected to the computer, and returns
 * the first PTP device it can find.  
 * 
 * @return A pointer to a \c libusb_device which represents the camera found, or NULL if none found.
 */
libusb_device * PTPUSB::find_first_camera() {
    // discover devices
    libusb_device **list;
    libusb_device *found = NULL;
    ssize_t cnt = libusb_get_device_list(NULL, &list);
    ssize_t i = 0, j = 0, k = 0;
    int err = 0;
    if (cnt < 0) {
        return NULL;
    }

    for (i = 0; i < cnt; i++) {
        libusb_device *device = list[i];
        struct libusb_config_descriptor * desc;
        int r = libusb_get_active_config_descriptor(device, &desc);
        
        if (r < 0) {
            return NULL;
        }
        
        for(j = 0; j < desc->bNumInterfaces; j++) {
            struct libusb_interface interface = desc->interface[j];
            for(k = 0; k < interface.num_altsetting; k++) {
                struct libusb_interface_descriptor altsetting = interface.altsetting[k];
                if(altsetting.bInterfaceClass == 6) { // If this has the PTP interface
                    found = device;
                    break;
                }
            }
            if(found) break;
        }
        
        libusb_free_config_descriptor(desc);
        
        if(found) break;
    }
    
    if(found) {
        libusb_ref_device(found);     // Add a reference to the device so it doesn't get destroyed when we free_device_list
    }
    
    libusb_free_device_list(list, 1);   // Free the device list with dereferencing. Shouldn't delete our device, since we ref'd it
    
    return found;
}

/**
 * @brief Opens the camera specified by \a dev.
 *
 * @param[in] dev The \c libusb_device which specifies which device to connect to.
 * @exception PTP::ERR_ALREADY_OPEN if this \c PTPUSB already has an open device.
 * @exception PTP::ERR_CANNOT_CONNECT if we cannot connect to the camera specified.
 * @return true if we successfully connect, false otherwise.
 */
bool PTPUSB::open(libusb_device * dev) {
    if(this->handle != NULL) {  // Handle will be non-null if the device is already open
        throw PTP::ERR_ALREADY_OPEN;
        return false;
    }

    int err = libusb_open(dev, &(this->handle));    // Open the device, placing the handle in this->handle
    if(err) {
        throw PTP::ERR_CANNOT_CONNECT;
        return false;
    }
    libusb_unref_device(dev);   // We needed this device refed before we opened it, so we added an extra ref. open adds another ref, so remove one ref
    
    struct libusb_config_descriptor * desc;
    int r = libusb_get_active_config_descriptor(dev, &desc);
    
    if (r < 0) {
        this->usb_error = r;
        return false;
    }
    
    int j, k;
    
    for(j = 0; j < desc->bNumInterfaces; j++) {
        struct libusb_interface interface = desc->interface[j];
        for(k = 0; k < interface.num_altsetting; k++) {
            struct libusb_interface_descriptor altsetting = interface.altsetting[k];
            if(altsetting.bInterfaceClass == 6) { // If this has the PTP interface
                this->intf = &altsetting;
                libusb_claim_interface(this->handle, this->intf->bInterfaceNumber); // Claim the interface -- Needs to be done before I/O operations
                break;
            }
        }
        if(this->intf) break;
    }
    
    
    const struct libusb_endpoint_descriptor * endpoint;
    for(j = 0; j < this->intf->bNumEndpoints; j++) {
        endpoint = &(this->intf->endpoint[j]);
        if((endpoint->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK == LIBUSB_ENDPOINT_IN) &&
            (endpoint->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_BULK) {
            this->ep_in = endpoint->bEndpointAddress;
        } else if((endpoint->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_OUT) {
            this->ep_out = endpoint->bEndpointAddress;
        }
    }
    
    libusb_free_config_descriptor(desc);
    
    // If we haven't detected an error by now, assume that this worked.
    return true;
}

/**
 * Perform a \c libusb_bulk_transfer to the "out" endpoint of the connected camera.
 *
 * @warning Make sure \a bytestr is at least \a length bytes in length.
 * @param[in] bytestr Bytes to write through USB.
 * @param[in] length  Number of bytes to read from \a bytestr.
 * @param[in] timeout The maximum number of seconds to attempt to send for.
 * @return 0 on success, libusb error code otherwise.
 * @exception PTP::ERR_NOT_OPEN if not connected to a camera.
 * @see PTPUSB::_bulk_read
 */
bool PTPUSB::_bulk_write(const unsigned char * bytestr, const int length, const int timeout) {
    int transferred;
    
    if(this->handle == NULL) {
        throw PTP::ERR_NOT_OPEN;
        return 0;
    }
    
    unsigned char * write_data = new unsigned char[length];
    std::copy(bytestr, bytestr + length, write_data);
    
    // TODO: Return the amount of data transferred? Check it here? What should we do if not enough was sent?
    bool ret = (libusb_bulk_transfer(this->handle, this->ep_out, write_data, length, &transferred, timeout) == 0);
    
    delete[] write_data;
    
    return ret;
}

/**
 * Perform a \c libusb_bulk_transfer to the "in" endpoint of the connected camera.
 *
 * @warning Make sure \a data_out has enough memory allocated to read at least \a size bytes.
 * @param[out] data_out    The data read from the camera.
 * @param[in]  size        The number of bytes to attempt to read.
 * @param[out] transferred The number of bytes actually read.
 * @param[in]  timeout     The maximum number of seconds to attempt to read for.
 * @return 0 on success, libusb error code otherwise.
 * @exception PTP::ERR_NOT_OPEN if not connected to a camera.
 * @see PTPUSB::_bulk_read
 */
bool PTPUSB::_bulk_read(unsigned char * data_out, const int size, int * transferred, const int timeout) {
    if(this->handle == NULL) {
        throw PTP::ERR_NOT_OPEN;
        return 0;
    }
    
    // TODO: Return the amount of data transferred? We might get less than we ask for, which means we need to tell the calling function?
    return libusb_bulk_transfer(this->handle, this->ep_in, data_out, size, transferred, timeout) == 0;
}

/**
 * @brief Returns true if we can _bulk_read and _bulk_write
 */
bool PTPUSB::is_open() {
    // This should work... and be fairly straightforward!
    return this->handle != NULL;
}

/**
 * Closes the opened USB object.
 * @todo Check for errors in the calls
 */
void PTPUSB::close() {
    if(this->handle != NULL) {
        libusb_release_interface(this->handle, this->intf->bInterfaceNumber);
        libusb_close(this->handle);
        this->handle = NULL;
    }
}

}
