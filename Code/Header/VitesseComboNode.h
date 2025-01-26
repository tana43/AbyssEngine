#pragma once
#include "ComboNode.h"
#include "ComboSystem.h"

class Vitesse;

//ルートノード
class VitesseComboRoot : public ComboNode<Vitesse>
{
public:
    void Initialize()override {}
    ComboSystemInterface::State Run()override { return ComboSystemInterface::State::Complete; }
    void Finalize()override {}
};

class VitesseComboN1  : public ComboNode<Vitesse>
{
public:
    void Initialize()override;
    ComboSystemInterface::State Run()override;
    void Finalize()override;
};

class VitesseComboN2 : public ComboNode<Vitesse>
{
public:
    void Initialize()override;
    ComboSystemInterface::State Run()override;
    void Finalize()override;
};

class VitesseComboN3 : public ComboNode<Vitesse>
{
public:
    void Initialize()override;
    ComboSystemInterface::State Run()override;
    void Finalize()override;
};

