# Esp8266 Message Board

Message board for group decision-making in LAN




## About The Prototype

Hacker Union is an open wifi network used for group decision-making and communication in a local area. It could be used for communication within residences, workspaces, schools, gatherings or possibly protests, where internet access is limited or unavailable. This web server is being hosted on a small and portable wifi chip powered by USB or battery.

Posts are submitted and voted on, with IP and cookie verifications preventing multiple votes. The highest ranking proposition is then group edited and saved as a coherent and concise legislation.

The module shares and collates content with other nearby HU wifi chips, forming a decentralized mesh network. Connection to the internet is possible given an SSID, password, and port-forwarding access on a router. Given an internet connection, the module will upload and backup all content to the internet.

HU serves as an example of the ad-hoc mesh networks that will be possible as hardware becomes increasingly cheaper, smaller and more powerful in the future. The chip alone costs ~$5. The source code will be uploaded soon.

## Screenshots

Over 750,000 bytes of memory remain for new threads, replies and polls, saved in a flat file database.   

![alt tag](Esp8266MessageBoard/Images/board.png)


Includes a Captive Portal and DNS Liar to redirect all new traffic to the homepage.

![alt tag](Esp8266MessageBoard/Images/captive_portal.jpg)


Powered by any USB port, may be powered by lithium cell. 

![alt tag](Esp8266MessageBoard/Images/module.jpg)
