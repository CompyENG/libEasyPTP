/**
 * @file PTPCamera.cpp
 * 
 * @brief A placeholder for generic PTP cameras
 * 
 * Eventually, this file will be full of functions to ease communication with
 * standard (non-CHDK) PTP cameras.
 */
 
#include "PTPCamera.hpp"
#include "libptp++.hpp"

namespace PTP {
    
/**
 * @brief Creates an empty \c PTPCamera.
 *
 * @warning This class is not yet implemented. Creating an object of type
 *          \c PTPCamera will only result in a warning printed to \c stderr.
 */
PTPCamera::PTPCamera() {
    throw ERR_NOT_IMPLEMENTED;
}

} /* namespace PTP */
