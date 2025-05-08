
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
        struct trkpt
        {
            double lat;
            double lon;

            //todo: add timestamp management

            //todo: trkpt can also have elevation from <ele> child
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

            try {
                rapidxml::xml_document<> doc;
                doc.parse<0>(&content[0]);

                rapidxml::xml_node<> *gpx_node = doc.first_node("gpx");
                if (!gpx_node) { return false; }

                rapidxml::xml_node<> *trk_node = gpx_node->first_node("trk");
                if (!trk_node) { return false; }

                for (rapidxml::xml_node<>* trkseg_node = trk_node->first_node("trkseg"); 
                trkseg_node; 
                trkseg_node = trkseg_node->next_sibling("trkseg")) {
                        
                    for (rapidxml::xml_node<>* trkpt_node = trkseg_node->first_node("trkpt"); 
                    trkpt_node; 
                    trkpt_node = trkpt_node->next_sibling("trkpt")) {
                        
                        trkpt point;
                        rapidxml::xml_attribute<>* lat = trkpt_node->first_attribute("lat");
                        rapidxml::xml_attribute<>* lon = trkpt_node->first_attribute("lon");

                        if (lat && lon) {
                            point.lat = std::stod(lat->value());
                            point.lon = std::stod(lon->value());

                            track.push_back(point);
                            print_trkpt(point);
                        }
                    }
                }

            } catch (const rapidxml::parse_error& e) {
                return false;
            } catch (const std::exception& e) {
                return false;
            }

            return !track.empty(); // Return true if at least one point was loaded
        }

        void print_trkpt(trkpt t) {
            std::cout << t.lat << " - " << t.lon << std::endl;
        }

    private:
        std::vector<trkpt> track;
};

}
}

#endif // FCPP_COMPONENT_GPS_TRACE_H_