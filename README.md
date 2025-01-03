# WMI

Created by Thomas Sparber 2016

Modified with improvements by Ricardo Carvalho 2024

## GOAL

This is a very simple library written in C++ to execute WMI queries.
The aim is to make it as simple as possible and stick as much as
possible to the C++ standard (avoid Microsoft specific things) so that
it even compiles smoothly on MinGW.

## Usage

I think it is so simple that a small program explains the usage without
any further comments:
```cpp
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
```
The include file <wmi.hpp> contains all interfaces to execute WMI queries.
The include file <wmiclasses.hpp> contains some predefined WMI classes
(e.g. Win32_ComputerSystem or Win32_Service...)

Create WMI classes
------------------
As already mentioned, the include file <wmiclasses.hpp> provides some standard
WMI classes, but it is also very easy to add more of them. All you need to do is:
```cpp
    struct Win32_MyCustomClass
    {
    
        /**
          * This function is called by requestWmi and requestAllWmi
          * with the actual WmiResult
         **/
        void setProperties(const WmiResult &result, std::size_t index)
        {
            //EXAMPLE EXTRACTING PROPERTY TO CLASS
		    result.extract(index, "name", (*this).name);
        }
    
        /**
          * This function is used to determine the actual WMI class name
         **/
        static std::string getWmiClassName()
        {
            return "Win32_MyCustomClass";
        }
    
        /**
          * This function can be optionally implemented if the wmi class
		  * is not member of cimv2. In such a case, this function needs
		  * to return the root for this WMI class
         **/
        /*static std::string getWmiPath()
        {
            return "not cimv2";
        }*/

        string name;
        //All the other properties you wish to read from WMI
    
    }; //end struct Win32_ComputerSystem
```
These two functions are the only requirements your class needs to have.


## Compiling

Requirements:

Target Windows platform only.
You'll only need a C++11 compatible compiler, it should compile on pretty much about anything.

- GCC 4.8.1+
- clang 3.3+
- MSVC 18.0 (Visual Studio 2013)+
- Intel 15.0+

### CMake

Use can you CMake to easily setup the build environment.
