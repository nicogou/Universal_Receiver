#ifndef Universal_Receiver_h
#define Universal_Receiver_h

#include <SoftwareSerial.h>
#include <SoftEasyTransfer.h>
#include <EasyTransfer.h>

#define NB_MAX_DATA 20
#define BT_HW_HM10 "HM-10"
#define BT_HW_HC05 "HC-05"

struct UPDATED
{
    bool btOrHw[2]; // Id 0 is the bluetooth data, id 1 is the HW data.

    bool bluetooth()
    {
        return this->btOrHw[0];
    }

    bool hardware()
    {
        return this->btOrHw[1];
    }
};

struct RECEIVE_DATA_STRUCTURE
{
    // put your variable definitions here for the data you want to receive
    // THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
    int analogNb;
    int digitalNb;
    int analog[NB_MAX_DATA];
    int digital[NB_MAX_DATA];
};

struct SEND_DATA_STRUCTURE
{
    // put your variable definitions here for the data you want to receive
    // THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO

    // Nothing to be sent back to Controller yet.
};

class Universal_Receiver
{
public:
    String btHardwareConfig;

    bool isHwSerial;
    Stream *hwSerial;

    int receiverSerialRx;
    int receiverSerialTx;
    SoftwareSerial *controllerSerial; // RX, TX
    HardwareSerial *hwControllerSerial;
    //int bluetoothStatePin;
    bool btState = false;
    // The HM-10 state pin is at level high when connected, but there are 2 possible behaviour when not connected:
    // 1- state pin oscillates between high and low (500 ms between a change)
    // 2- state pin is low
    // This behaviour can be changed via AT commands : send AT+PIO10 for behaviour 1, and AT+PIO11 for behaviour 2.
    // We will be in behaviour 2 for the purpose of this project.

    // Wired input pins

    // Total data
    int analogNb_hw;
    int digitalNb_hw;
    int analogPin[NB_MAX_DATA];
    int digitalPin[NB_MAX_DATA]; // The Bluetooth state pin is always the first index of the wired sensors.
    bool digitalInputPullup[NB_MAX_DATA];
    bool digitalReversedLogic[NB_MAX_DATA];
    int analog[NB_MAX_DATA * 2];      // Data received via Bluetooth is stored in the first NB_MAX_DATA indexes, data from wired sensors is stored in the last NB_MAX_DATA indexes.
    int digital[NB_MAX_DATA * 2];     // Data received via Bluetooth is stored in the first NB_MAX_DATA indexes, data from wired sensors is stored in the last NB_MAX_DATA indexes.
    int lastAnalog[NB_MAX_DATA * 2];  // Stores the last analog state.
    int lastDigital[NB_MAX_DATA * 2]; // Stores the last digital state.
    int threshold[NB_MAX_DATA * 2];   // Holds the threshold for the analog values. If old_data-threshold <= incoming data <= old_data+threshold, then the incoming data is considered the same as the old data.
    UPDATED isUpdated;

    // create two EasyTransfer objects.
    SoftEasyTransfer ETin, ETout;
    EasyTransfer ETin_hw, ETout_hw;
    // give a name to the group of data
    RECEIVE_DATA_STRUCTURE rxdata;
    SEND_DATA_STRUCTURE txdata;

    Universal_Receiver(int rx, int tx, int digNb_hw, int anaNb_hw, int digPins[NB_MAX_DATA], int anaPins[NB_MAX_DATA], bool digInputPullup[NB_MAX_DATA], bool digReversedLogic[NB_MAX_DATA], int thresh[NB_MAX_DATA * 2], String btHardware);
    Universal_Receiver(HardwareSerial *stream, int digNb_hw, int anaNb_hw, int digPins[NB_MAX_DATA], int anaPins[NB_MAX_DATA], bool digInputPullup[NB_MAX_DATA], bool digReversedLogic[NB_MAX_DATA], int thresh[NB_MAX_DATA * 2], String btHardware);
    // void setSerialLink(int rx, int tx);
    // void setSerialLink(HardwareSerial *stream);

    void start(int digNb_hw, int anaNb_hw, int digPins[NB_MAX_DATA], int anaPins[NB_MAX_DATA], bool digInputPullup[NB_MAX_DATA], bool digReversedLogic[NB_MAX_DATA], int thresh[NB_MAX_DATA * 2], String btHardware); // Class initializer.
    // Not all pins on the Mega and Mega 2560 support change interrupts, so only the following can be used for RX: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), A15 (69).
    // Not all pins on the Leonardo and Micro support change interrupts, so only the following can be used for RX: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
    // On Arduino or Genuino 101 RX doesn't work on Pin 13
    bool state();                      // Check if bluetooth is connected
    bool receivedDataFromController(); // Check if new data has arrived from the controller
    bool updateWiredInput();
    bool receivedData();
    bool buttonPushed(int ii);
    bool buttonClicked(int ii);
    bool buttonReleased(int ii);
    bool flushSerialPort();
};

#endif