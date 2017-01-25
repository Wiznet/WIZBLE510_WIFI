# WIZBLE510 Wi-Fi Test Firmware

![WIZBLE510](http://wiznetmuseum.com/wp/wp-content/uploads/2017/01/1_1.png "WIZBLE510")

## Summary

Using WIZnet Wi-Fi Module Wizfi310 and the Nordic BLE SoC nRF52832 implemented the BLE to Wi-Fi. BLE Central/Peripheral communicate using a UART Nordic Service (NUS) Profile. And BLE Central and Wizfi310 communicate with UART. Wizfi310 also operates as a TCP Client, TCP Server to communicate with the Wi-Fi (through the line). The purpose of this Application Note is to build a low power IoT node environment. This firmware is based on nRF5 SDK 12.2.0

## H/W Setting

![H/W Setting](http://wiznetmuseum.com/wp/wp-content/uploads/2017/01/WIZBLE510_HW_Setting.png "H/W Setting")

## Refernce
### Wizfi310 AT Command list
http://wizwiki.net/wiki/doku.php?id=products:wizfi310:wizfi310pg:start

### Used Command
<strong>+++</strong> : Exit Data Mode   *In data mode, all UART data is transmitted as Wi-Fi.<br>
<strong>AT+WLEAVE</strong> : WiFi Disassociation <br>
<strong>AT+WNET</strong> : Network Configuration<br>
<strong>AT+WSET</strong> : Wi-Fi Configuration<br>
<strong>AT+WSEC</strong> : Wi-Fi Security Configuration<br>
<strong>AT+WJOIN</strong> : Wi-Fi Association<br>
<strong>AT+SCON</strong> : Socket Open/Connect<br>

## Testing

### Network Setting
Open the project.
\...\WIZBLE510_Wizfi310_Release_1\nRF5_SDK_12.2.0_Wizfi310\examples\ble_central\ble_app_uart_c\pca10040\s132
Open WizFi310_config.h. Setting SSID, AP password, Server IP, Server Port

![WizFi310_config.h](http://wiznetmuseum.com/wp/wp-content/uploads/2017/01/WIZBLE510_HW_Setting1.png "WizFi310_config.h")

### TCP Server
Open Hercules or TCP server tool. Port is set to 5000 in the TCP Server tab and click on the listen.

![Hercules2](http://wizwiki.net/wiki/lib/exe/fetch.php?media=osh:cookie:3-6_bte_testing.jpg "Hercules2")

### Build and write
### The ble peripheral is prepared as ble_app_uart.
### Check the debug message via Segger RTT.
![Segger_RTT](http://wiznetmuseum.com/wp/wp-content/uploads/2017/01/WIZBLE510_segger.png "Segger_RTT")

### Data Input
You can see that Ble peripheral Data goes to TCP Server.
![Data Input](http://wiznetmuseum.com/wp/wp-content/uploads/2017/01/WIZBLE510_Result.png "Data Input")

