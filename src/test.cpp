/**
 *
 * WMI
 * @author Thomas Sparber (2016)
 *
 **/

#include <unistd.h>
#include <iostream>

#include <wmi.hpp>
#include <wmiclasses.hpp>

using namespace Wmi;

int main(int /*argc*/, char* /*args*/[]) {
  try {
    Win32_ComputerSystem computer = retrieveWmi<Win32_ComputerSystem>();
    Win32_ComputerSystemProduct product =
        retrieveWmi<Win32_ComputerSystemProduct>();
    SoftwareLicensingService liscense = retrieveWmi<SoftwareLicensingService>();
    Win32_OperatingSystem os_info = retrieveWmi<Win32_OperatingSystem>();

    std::cout << "Computername: " << computer.Name
              << " Domain: " << computer.Domain << std::endl;
    std::cout << "Product: " << product.Name << " UUID:" << product.UUID
              << std::endl;
    std::cout << "Architecture: " << os_info.OSArchitecture << std::endl;
    std::cout << "Roles: " << std::endl;
    for (const std::string& role : computer.Roles) {
      std::cout << " - " << role << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Machine Id:" << liscense.ClientMachineID
              << " Kmsid:" << liscense.KeyManagementServiceProductKeyID
              << std::endl;
    std::cout << "Installed services:" << std::endl;

    // gets all rows and all columns
    for (const Win32_Service& service : retrieveAllWmi<Win32_Service>()) {
      std::cout << service.Caption << " started:" << service.Started
                << " state:" << service.State << std::endl;
    }

    // gets all rows and only specified columns(better performance)
    for (const Win32_Service& service :
         retrieveAllWmi<Win32_Service>("Caption,Started,State")) {
      std::cout << service.Caption << " started:" << service.Started
                << " state:" << service.State << std::endl;
    }

    // Example for using a class that has a non default root (securitycenter2)
    // This can be accombplished by implementing getWmiPath in the wmi class
    std::cout << "Antivirus installed:" << std::endl;
    for (const AntiVirusProduct& antivirus :
         retrieveAllWmi<AntiVirusProduct>()) {
      std::cout << antivirus.DisplayName
                << " | path:" << antivirus.PathToSignedProductExe
                << " state:" << antivirus.ProductState
                << " time: " << antivirus.Timestamp << std::endl;
    }

    // Example for Windows Embedded
    // UWF_Filter filter = retrieveWmi<UWF_Filter>();
    // cout << "UWF Filter enabled:" << filter.CurrentEnabled << std::endl;
  } catch (const WmiException& ex) {
    std::cerr << "Wmi error: " << ex.errorMessage
              << ", Code: " << ex.hexErrorCode() << std::endl;
    return 1;
  }

  return 0;
}