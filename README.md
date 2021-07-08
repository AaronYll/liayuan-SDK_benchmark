# Azure Storage Queue official SDK perf test

This repo contains the source code of official SDK perf test. You can reproduction the test follow steps:

## Install the Azure Storage client library for C++
To install the Azure Storage client library for C++, you can use the following methods:

- On Windows, use [vcpkg](https://github.com/microsoft/vcpkg) as the dependency manager. Follow the [quickstart](https://github.com/microsoft/vcpkg#getting-started) to initialize vcpkg. Then, use the following command to install the library in `Powershell`:

```powershell 
.\vcpkg.exe install azure-storage-cpp
```


## Build release mode solution in Visual Studio
To use vcpkg with Visual Studio, please follow the [quickstart](https://github.com/microsoft/vcpkg#getting-started), or you can run the cmd in `Powershell`:

```powershell
.\vcpkg\vcpkg integrate install
```
Then you can build the solution in Visual Studio.


## Run the EXE File in ABS Server
To ensure the consistency of the test environment, we test official SDK perf in ABS server (Deployed in test tenant). 
Copy the Release file folder built in last step to ABS server and run EXE in it. 

`Note`: Before running the EXE file in ABS server, please use vcpkg to install the azure storage library either. The steps are the same as above `Install the Azure Storage client library for C++`.