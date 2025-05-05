
#ifndef FCPP_COMPONENT_GPS_TRACE_H_
#define FCPP_COMPONENT_GPS_TRACE_H_

#include <string>
#include <vector>
#include <fstream>
#include <sstream> 
#include <stdexcept>
#include <iostream>

#include "rapidxml.hpp" // RapidXML header


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {

namespace gps {
/**
 * @brief Class handling
 */
class gps_trace {
    public: // visible by net objects and the main program
        struct gps_track_point
        {
            double lat;
            double lon;

            //todo: add timestamp management
        };

        
        /**
         * @brief Main constructor.
         * 
         * @param src_gpx_file The src of the gpx file to load
         */
        gps_trace(const std::string& src_gpx_file) {
            if (!load_gpx_file(src_gpx_file)) {
                std::cout << "gps_trace() - Error while opening the gpx file";
            }
        };

        bool load_gpx_file(const std::string& src) {
            std::ifstream file(src);
            if (!file.is_open()) {
                std::cout << "load_gpx_file() - Failed to open file";
                return false; // Failed to open file
            }

            // Save file to string stream for processing
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            file.close();

            rapidxml::xml_document<> doc;
            try {
                doc.parse<0>(&content[0]);
            } catch (const rapidxml::parse_error& e) {
                return false;
            } catch (const std::exception& e) {
                return false;
            }

            return true;
        }

    private:
        std::vector<gps_track_point> track;
};

}
}

#endif // FCPP_COMPONENT_GPS_TRACE_H_