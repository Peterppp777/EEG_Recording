# EEG_Recording
 
Arduino Library Credentials:
1. LSM6DS3 Library
The LSM6DS3 is a 3D accelerometer and 3D gyroscope sensor. The library for this sensor might be available through the Arduino Library Manager or directly from the manufacturer's or a third-party GitHub repository.
https://www.arduino.cc/reference/en/libraries/arduino_lsm6ds3/

2. CircularBuffer Library
For the CircularBuffer library, follow the same steps as above. You will likely find a suitable circular buffer library in the Arduino Library Manager.
https://www.arduino.cc/reference/en/libraries/circularbuffer/
(Remember to replace the include with exact file location of the library)

3. NRF52_MBED_TimerInterrupt Library
This library is used for handling timer interrupts on NRF52 devices. You can search for it in the Arduino Library Manager. If it's unavailable there, you must find it on GitHub or another source, download it, and add it to your IDE.
https://github.com/khoih-prog/NRF52_MBED_TimerInterrupt

4. Arduino_CRC32 Library
This library is used for calculating CRC32 checksums. As with the other libraries, first try to find it in the Arduino Library Manager. If it's not available, look for it on GitHub or another source, download the .zip, and add it to your IDE.
https://github.com/arduino-libraries/Arduino_CRC32


Python Module Installation
1. pycrc
pycrc is not a standard Python module and needs to be installed. However, as of my last update, there isn't a package named pycrc directly available through pip that matches the usage you've described. It's possible you're referring to a script or a module for calculating CRCs that needs to be included differently.
https://pypi.org/project/pycrc/

2. google-cloud-storage
The google-cloud-storage library is used to interact with Google Cloud Storage, and it can be installed using pip
https://cloud.google.com/python/docs/reference/storage/latest

For google-cloud-storage, you'll also need to set up authentication to interact with Google Cloud services:

Install the Google Cloud SDK (optional but recommended for managing credentials and interacting with Google Cloud services from the command line):

Follow the instructions here: Google Cloud SDK Documentation

Initialize the SDK (if installed):

Run gcloud init in your terminal and follow the instructions.

Create a Service Account and Download a Key File:

Go to the Google Cloud Console.
Navigate to IAM & Admin > Service Accounts.
Create a new service account, assign it the necessary roles (e.g., Storage Object Admin), and download a JSON key file.
Set the Environment Variable for Authentication:

Set GOOGLE_APPLICATION_CREDENTIALS to the path of your JSON key file. This allows your script to authenticate with Google Cloud services.

On Linux/macOS:
sh
export GOOGLE_APPLICATION_CREDENTIALS="/path/to/your/keyfile.json"

On Windows (Command Prompt):
cmd
set GOOGLE_APPLICATION_CREDENTIALS=C:\path\to\your\keyfile.json

On Windows (PowerShell):
PowerShell
$env:GOOGLE_APPLICATION_CREDENTIALS="C:\path\to\your\keyfile.json"


