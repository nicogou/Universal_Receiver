#include "Universal_Receiver.h"

Universal_Receiver::Universal_Receiver(int rx, int tx, int digNb_hw, int anaNb_hw, int digPins[NB_MAX_DATA], int anaPins[NB_MAX_DATA], bool digInputPullup[NB_MAX_DATA], bool digReversedLogic[NB_MAX_DATA], int thresh[NB_MAX_DATA * 2], String btHardware)
{
    isHwSerial = false;
    receiverSerialRx = rx;
    receiverSerialTx = tx;
    controllerSerial = new SoftwareSerial(receiverSerialRx, receiverSerialTx);

    start(digNb_hw, anaNb_hw, digPins, anaPins, digInputPullup, digReversedLogic, thresh, btHardware);
}

Universal_Receiver::Universal_Receiver(HardwareSerial *stream, int digNb_hw, int anaNb_hw, int digPins[NB_MAX_DATA], int anaPins[NB_MAX_DATA], bool digInputPullup[NB_MAX_DATA], bool digReversedLogic[NB_MAX_DATA], int thresh[NB_MAX_DATA * 2], String btHardware)
{
    isHwSerial = true;
    hwControllerSerial = stream;

    start(digNb_hw, anaNb_hw, digPins, anaPins, digInputPullup, digReversedLogic, thresh, btHardware);
}

void Universal_Receiver::start(int digNb_hw, int anaNb_hw, int digPins[NB_MAX_DATA], int anaPins[NB_MAX_DATA], bool digInputPullup[NB_MAX_DATA], bool digReversedLogic[NB_MAX_DATA], int thresh[NB_MAX_DATA * 2], String btHardware)
{
    btHardwareConfig = btHardware;

    Serial.begin(9600);
    long baudRate;
    if (btHardwareConfig == BT_HW_HM10)
    {
        baudRate = 9600;
    }
    else if (btHardwareConfig == BT_HW_HC05)
    {
        baudRate = 38400;
    }

    if (isHwSerial)
    {
        hwControllerSerial->begin(baudRate);
        // Start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
        ETin_hw.begin(details(rxdata), hwControllerSerial);
        ETout_hw.begin(details(txdata), hwControllerSerial);
    }
    else
    {
        controllerSerial->begin(baudRate);
        // Start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
        ETin.begin(details(rxdata), controllerSerial);
        ETout.begin(details(txdata), controllerSerial);
    }

    analogNb_hw = anaNb_hw;
    digitalNb_hw = digNb_hw;

    if (digitalNb_hw > 0)
    {
        for (int ii = 0; ii < digitalNb_hw; ii++)
        {
            digitalInputPullup[ii] = digInputPullup[ii];
            digitalReversedLogic[ii] = digReversedLogic[ii];
            digitalPin[ii] = digPins[ii];

            if (digitalInputPullup[ii])
            {
                pinMode(digitalPin[ii], INPUT_PULLUP);
            }
            else
            {
                pinMode(digitalPin[ii], INPUT);
            }
        }
    }

    if (analogNb_hw > 0)
    {
        for (int ii = 0; ii < anaNb_hw; ii++)
        {
            analogPin[ii] = anaPins[ii];
            pinMode(analogPin[ii], INPUT);
        }
    }

    for (int ii = 0; ii < NB_MAX_DATA * 2; ii++)
    {
        lastAnalog[ii] = 0;
        lastDigital[ii] = -1;
        threshold[ii] = thresh[ii];
    }

    flushSerialPort();
}

bool Universal_Receiver::state()
{
    if (btHardwareConfig == BT_HW_HM10)
    {
        int btPin = 0;
        btState = digitalRead(digitalPin[btPin]);
        return btState;
    }
    else if (btHardwareConfig == BT_HW_HC05)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Universal_Receiver::receivedDataFromController()
{
    bool temp;
    if (!isHwSerial)
    {
        temp = ETin.receiveData();
    }
    else
    {
        temp = ETin_hw.receiveData();
    }

    // Get data from receiver
    if (temp)
    {
        bool valid_data = true;
        bool same_data = true;
        for (int ii = 0; ii < NB_MAX_DATA; ii++)
        {
            if (ii < rxdata.analogNb)
            {
                lastAnalog[ii] = analog[ii];
                analog[ii] = rxdata.analog[ii];
            }
            else
            {
                lastAnalog[ii] = 0;
                analog[ii] = 0;
            }
            if (analog[ii] < 0 || analog[ii] > 1023 || lastAnalog[ii] < 0 || lastAnalog[ii] > 1023)
            {
                valid_data = false;
            }
            if (analog[ii] <= lastAnalog[ii] - threshold[ii] || analog[ii] >= lastAnalog[ii] + threshold[ii])
            {
                same_data = false;
            }
        }
        for (int ii = 0; ii < NB_MAX_DATA; ii++)
        {
            if (ii < rxdata.digitalNb)
            {
                lastDigital[ii] = digital[ii];
                digital[ii] = rxdata.digital[ii];
            }
            else
            {
                lastDigital[ii] = 0;
                digital[ii] = 0;
            }
            if (digital[ii] < 0 || digital[ii] > 1 || lastDigital[ii] < 0 || lastDigital[ii] > 1)
            {
                valid_data = false;
            }
            if (lastDigital[ii] != digital[ii])
            {
                same_data = false;
            }
        }
        if (!valid_data)
        {
            return false;
        }
        if (same_data)
        {
            return false;
        }
        return true; // New data has been collected.
    }
    return false; // No new data collected
}

bool Universal_Receiver::updateWiredInput()
{
    bool same_data = true;
    for (int ii = 0; ii < NB_MAX_DATA; ii++)
    {
        if (ii < analogNb_hw)
        {
            lastAnalog[NB_MAX_DATA + ii] = analog[NB_MAX_DATA + ii];
            analog[NB_MAX_DATA + ii] = analogRead(analogPin[ii]);
        }
        else
        {
            lastAnalog[NB_MAX_DATA + ii] = 0;
            analog[NB_MAX_DATA + ii] = 0;
        }
        if (analog[NB_MAX_DATA + ii] <= lastAnalog[NB_MAX_DATA + ii] - threshold[NB_MAX_DATA + ii] || analog[NB_MAX_DATA + ii] >= lastAnalog[NB_MAX_DATA + ii] + threshold[NB_MAX_DATA + ii])
        {
            same_data = false;
        }
    }

    for (int ii = 0; ii < NB_MAX_DATA; ii++)
    {
        if (ii < digitalNb_hw)
        {
            lastDigital[NB_MAX_DATA + ii] = digital[NB_MAX_DATA + ii];
            digital[NB_MAX_DATA + ii] = digitalRead(digitalPin[ii]);
        }
        else
        {
            lastDigital[NB_MAX_DATA + ii] = 0;
            digital[NB_MAX_DATA + ii] = 0;
        }
        if (lastDigital[NB_MAX_DATA + ii] != digital[NB_MAX_DATA + ii])
        {
            same_data = false;
        }
    }

    if (same_data)
    {
        return false;
    }
    return true;
}

bool Universal_Receiver::receivedData()
{
    isUpdated.btOrHw[0] = receivedDataFromController();
    isUpdated.btOrHw[1] = updateWiredInput();

    if (!isUpdated.bluetooth() && !isUpdated.hardware())
    {
        return false;
    }
    return true;
}

bool Universal_Receiver::buttonPushed(int ii)
{
    if (digital[ii] == 0)
    {
        return true;
    }
    return false;
}

bool Universal_Receiver::buttonClicked(int ii)
{
    if (digital[ii] == 0 && lastDigital[ii] == 1)
    {
        return true;
    }
    return false;
}

bool Universal_Receiver::buttonReleased(int ii)
{
    if (digital[ii] == 1 && lastDigital[ii] == 0)
    {
        return true;
    }
    return false;
}

bool Universal_Receiver::flushSerialPort()
{
    if (!isHwSerial)
    {
        while (controllerSerial->available())
        {
            controllerSerial->read();
        }
    }
    else
    {
        while (hwControllerSerial->available())
        {
            hwControllerSerial->read();
        }
    }
}