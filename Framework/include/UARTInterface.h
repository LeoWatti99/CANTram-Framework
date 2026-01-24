#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

#include "Interface.h"
#include "UARTCore.h"

class UARTInterface : public Interface{
public:
    UARTInterface() = default;

    Interface::Type getType() const override {
        return Interface::BUS;
    }

    //Set communication parameters
    void setBaudrate(uint32_t baudrate)
        {if(uartCore) uartCore->setBaudrate(baudrate);}
    void setDataBits(UARTCore::DataBits dataBits)
        {if(uartCore) uartCore->setDataBits(dataBits);}
    void setParity(UARTCore::Parity parity)
        {if(uartCore) uartCore->setParity(parity);}
    void setStopBits(UARTCore::StopBits stopBits)
        {if(uartCore) uartCore->setStopBits(stopBits);}

    bool send(char data[], size_t length){
        // Implement sending data over UART
        return uartCore->send(data,length);
    }

    size_t read(char* buffer){
        // Implement receiving data over UART
        return uartCore->read(buffer);
    }

    size_t available(){
        // Implement checking available data over UART
        return uartCore->available();
    }

    void setUartCore(UARTCore* core){uartCore = core;}
    UARTCore* getUartCore(){return uartCore;}


private:
    std::function<void(uint32_t, uint8_t, bool, uint8_t)> onBeginCallback;
    uint32_t _baudrate;
    UARTCore::DataBits _dataBits;
    UARTCore::Parity _parity;
    UARTCore::StopBits _stopBits;

    UARTCore* uartCore;

    char** txBuffer;
    char** rxBuffer;

    


    

};

#endif