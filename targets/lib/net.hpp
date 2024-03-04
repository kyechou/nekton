#ifndef MIMESIS_TARGETS_LIBS_NET_HPP
#define MIMESIS_TARGETS_LIBS_NET_HPP

#include <string>
#include <vector>

struct Interface {
    int fd;
    std::string if_name;
};

/**
 * @brief Bind an fd to the given existing interface.
 *
 * @param if_name Name of the interface to be opened.
 * @return The file descriptor bound to the device.
 */
int open_existing_interface(const std::string &if_name);

/**
 * @brief Open a file descriptor for each existing interface, except for
 * loopback and SIT devices.
 *
 * @return A map of file descriptors to the corresponding interface names.
 */
std::vector<Interface> open_existing_interfaces();

/**
 * @brief Close the given interface file descriptors.
 */
void close_interface_fds(const std::vector<Interface> &interfaces);

#endif // MIMESIS_TARGETS_LIBS_NET_HPP
