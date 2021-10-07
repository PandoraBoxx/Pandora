# Pandora
Do you want to protect yourself from Pegassus? Then use Pandora!

Take a look at Wikipedia's [Pegassus](https://en.wikipedia.org/wiki/Pegasus_(spyware)) explanation

"Pegasus is spyware developed by the Israeli cyberarms firm NSO Group that can be covertly installed on mobile phones
(and other devices) running most versions of iOS and Android. The 2021 Project Pegasus revelations suggest that the
current Pegasus software can exploit all recent iOS versions up to iOS 14.6. As of 2016, Pegasus was capable of
reading text messages, tracking calls, collecting passwords, location tracking, accessing the target device's
microphone and camera, and harvesting information from apps. The spyware is named after Pegasus, the winged horse
of Greek mythology. It is a Trojan horse computer virus that can be sent "flying through the air" to infect cell phones."

## Motivation
Privacy protection is the paramount of freedom.
The idea is to generate all important messages (audio, text) on an external secure <b>"unconnected"</b> portable device,
encrypt and sign them with RSA asymmetric keys, and then transmit them to your insecure personal computer or mobile phone
which will ultimately transmit the messages to the recipients.

If audio is to be used, then care should be taken to avoid being near to an insecure mobile phone, because it will be
meaningless to encrypt a message if they can hear it anyway.

Here the <b>"unconnected"</b> word doesn't mean useless. The OSes (Linux in this case) are tools designed for flexibility,
anyone can easily call [netcat](https://en.wikipedia.org/wiki/Netcat) on a shell and connect to any other computer on a
network or even the internet, the flexibility is great but not very secure. BLE, WIFI, LAN network are all controlled by
the OS and the user only have access to high level functions, to call [netcat](https://en.wikipedia.org/wiki/Netcat) it
is not necessary to worry about Linux drivers, again it is flexible but not very secure.

## Ideas
The communication interface to be used in this project is the UART because it is a simple low level protocol with not much support
at the OS level. This means that it will be very difficult to attack remotely.
Obviously, it is still possible to exfiltrate data through this channel, provided that the system is already compromised, but it
will be very difficult to get command execution through this channel alone.
At the same time Wireless connectivity is desirable, for usability reasons and because a mobile phone App support will be much
easier to develop and the code will be portable.
Therefore a WIFI capable microcontroller like the [ESP32](https://www.espressif.com/en/products/socs/esp32) which can receive AT
commands through the UART is to be used.
Because the [ESP32](https://www.espressif.com/en/products/socs/esp32) processor will be in the middle, compromising this WIFI
channel remotely will be very difficult (and this will be the only channel).
All the other OS functionalities (WIFI, BLE, USB, LAN) will be disabled at the OS level.

## Attack Scenarios
Remote attacks are not the only possible threats. The main board to be used is a Raspberry Pi [See the blog](https://cybersecminds.blogspot.com/)
and the OS will be Raspbian. If the pandora is ever stolen or seized by an untrusted party the privacy will be compromised.
First of all a [Raspberry Pi](https://www.raspberrypi.org/) board uses a micro SD card as a media to hold the OS, all the messages and the
private key on the SD card shall be encrypted. All unencrypted messages will be kept on RAM therefore just a power off will whipe the unencrypted
data (the frozen data attack is possible here but very difficult).

## Future improvements
Another nasty attack scenario is, the device being stolen, in the meantime the SD card being poisoned with malware and then returned without
knowledge of the owner. In the future there will be a system signature check functionality to avoid this scenario.
Also a panic button will be added, for now the private key is encrypted on the SD card, but somebody with resources might try to attack it,
wipe the SD card is a time consuming operation and there is a [wear leveling](https://en.wikipedia.org/wiki/Wear_leveling) functionality
which makes difficult to be sure that the data is really wiped.
To avoid this problem the encrypted private key will be moved to an [EEPROM](https://en.wikipedia.org/wiki/EEPROM) under the microcontroller
domain which can wipe the data in milliseconds.
For now the only devices supported to receive encrypted messages are computers through the UART (Windows or Linux), in the near future
Android phones will be supported, iOS will not be supported.

Hope this helps

First, setup the raspberrypi
download balenaEtcher-1.5.111-x64.AppImage
download 2021-05-07-raspios-buster-armhf-full.zip
using Etcher setup the image on to the microSD (15min)
plug the card into a raspberrypi with monitor and powerup

once the GUI is up and running, open a terminal and run
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install qt5-default
sudo apt-get install qtcreator

Open Qtcreator and go to Help > about plugins and and untick Remote Linux
Restart your raspberry pi
Then go to Kits (Still inside the Build and Run) and check whether Compiler, Debugger and Qt version are set

git clone https://github.com/PandoraBoxx/Pandora.git
for a fresh install will get ERROR unknown module serialport
sudo apt-get install libqt5serialport5-dev
also ERROR unknown module multimedia
sudo apt-get install qtmultimedia5-dev libqt5multimediawidgets5 libqt5multimedia5-plugins libqt5multimedia5


