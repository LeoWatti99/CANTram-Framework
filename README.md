# CANTram-Framework
CANTram steht für CAN-based Task-oriented Remote Automation Modules und ist ein System zur Realisierung dezentraler Steuerungen.

Die Hardware besteht aus Controllern, Hauptknoten und ihren Verbindungsleitungen. Die Controller wiederrum bestehen aus einzelnen Modulen, welche Schnittstellen besitzen. Je nach Bedarf können so beliebige Module schnell zusammengesteckt werden, um alle benötigten Schnittstellen bereit zustellen. 
Softwareseitig werden Controller durch eigene C++ Programme realisiert. Das CANTram-Framework bietet dafür eine Core-Instanz, an die Modul-Instanzen angeschlossen werden. Die Modul-Instanzen wiederrum besitzen Interface-Instanzen, die jeweils eine Hardware-Schnittstelle repräsentieren. Nach der Konfiguration kann ausschließlich mit den Interface-Instanzen gearbeitet werden, sodass die Hardware in hohem Maße abstrahiert ist.

