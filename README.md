# Pandora
Do you want to protect yourself from Pegassus? use Pandora

Take a look at Wikipedia's Pegassus explanation

Pegasus is spyware developed by the Israeli cyberarms firm NSO Group that can be covertly installed on mobile phones
(and other devices) running most versions of iOS and Android. The 2021 Project Pegasus revelations suggest that the
current Pegasus software can exploit all recent iOS versions up to iOS 14.6. As of 2016, Pegasus was capable of
reading text messages, tracking calls, collecting passwords, location tracking, accessing the target device's
microphone and camera, and harvesting information from apps. The spyware is named after Pegasus, the winged horse
of Greek mythology. It is a Trojan horse computer virus that can be sent "flying through the air" to infect cell phones.

So my idea is to generate all messages (audio, text) on an external secure unconnected portable device, encrypt it,
send it to your insecure device and and then send it to the recipient.

If you use audio be carefull not to be near to your insecure mobile phone, because it will be meaningless to encrypt the
message if they can hear you anyway.

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


