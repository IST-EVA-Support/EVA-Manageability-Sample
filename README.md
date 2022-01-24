# EVA-Manageability-Sample

## Introduction 
 
This reference code uses ADLINK managebility library for efficiently packaing AI models or other module.
Examples are provided for package AI model procedure. See the API reference section for detailed reference documentation of the C libraries. <h2>
  
## Dependencies 
  Manageability library implement on `Ubuntu 18.04` and used `libsqlite3-0`、`zip/unzip`、`libssl1.1` etc. third party software package.
* ADLINK managebility runtime library 4.0.X or newer
* OpenSSL
  ```bash
    sudo apt-get install libssl-dev
  ```
* sqlite3
  ```bash
    sudo apt-get update
    sudo apt-get install libsqlite3-0
  ```
  
  ## Ubuntu Installation
* Install ADLINK Managebility runtime library 4.0.X or newer version
 
  
## OpenVino / TensorRT model reference link
Please downlad the model file and copy the `*bin` or `*.engine` to the path under `{source path}\Model` and `{source path}\TensorModel`
* OpenVino
* TensorRT
  
## Compiling
  
### Ubuntu (amd64/aarch64)
Compiling the reference code by cmake script
```bash
git-clone https://github.com/IST-EVA-Support/EVA-Manageability-Sample.git
make
```
Running the ADLINK OTA agent

 ```c
 // Running the package help file.
./carota -h
```
There will list some properties that you can use or modify it.
  ```c
  Option: 
        -p [ Adding EVA AI configuration into package]
        -m [ Packing the AI config=> 0:Classfication(GoogleNet), 1:Face Detection(ssd), 2:PCBA(ssd), 3:Detection(yolo)], 4:Classfication(ssd), 5:Geofence(ssd), 6:Weardetection(ssd)
        -k [ Creating the new key and certificate.]
        -h [ This is help message. ]
  ```

### Optional - Signed update package
Manageability Library support signed update package verify. If system integrator (or customer) hope provide signed update package verify, asset must install certificate from system integrator(or customer).
  
### Optional - Preparing pipeline config package
For example: (Pack the eva configuration file)

 ```c
 //It will pack the `Face Detection` pipeline confiquration file. (OpenVino ssd)
./carota –p –m 1
```
### Preparing pipeline AI model package
Preparing model file into `{source path}\Model` or `{source path}\TensorModel` folder 
Modify your model schema definition as following
  
 ```c
//For example : Using the OpenVino mobilessd model, percision=FP32 version=1.0
INFERENCE_ENGINE=OpenVino
MODEL_NAME=SSD              
PRECISION=FP32                            
VERSION=1.0     
```
  
```c
 //It will pack the AI model file into package (OpenVino ssd)
./carota –p
```
The sample will pop up the message: 
`Add model file into update image. Which model file do you want to add into image?
OpenVino or TensorRT (O/T)?`  
 
Select appropriate model file. Then the sample will show the options step by step
  
`Would you like to sign the package?(Yes/No)`

`Creating the package's version (For example: 1.1, 1.2)...`
  
Finally, the package is ready when poping the following message.
* Please upload the package into the carota server. https://ota.sandbox.limios.net 
* Please record the `package id(PKGID)` form the UpdPkg_vX.X.Signed.zip
* Please assign the `PKGID` to `target version`.
