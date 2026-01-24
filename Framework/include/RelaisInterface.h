#ifndef RELAISINTERFACE_H
#define RELAISINTERFACE_H

#include "Interface.h"

class RelaisInterface : public Interface {
  public:
    Interface::Type getType() const override {
        return RELAIS;
    }
  private:

};

#endif