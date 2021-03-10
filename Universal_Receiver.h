#ifndef Universal_Receiver_h
#define Universal_Receiver_h

#include <SoftwareSerial.h>
#include <SoftEasyTransfer.h>
#include <EasyTransfer.h>

#define NB_MAX_DATA 14
#define BT_HW_HM10 "HM-10"
#define BT_HW_HC05 "HC-05"

struct UPDATED
{
    // Stores the state of the update of the bluetooth and hardware data.
    bool btOrHw[2]; // Id 0 is the bluetooth data, id 1 is the HW data. True means it has been updated and has new data.

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
    int16_t analogNb;
    int16_t digitalNb;
    int16_t analog[NB_MAX_DATA];
    int16_t digital[NB_MAX_DATA];
    int16_t analogThreshold[NB_MAX_DATA];
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
    String btHardwareConfig; // Stores the type of Bluetooth module used on the Receiver. IDEA: make a struct gathering all the hardware configuration (pin numbers, etc).

    bool isHwSerial;                    // True if a hardware serial is used. False if a software serial is used.
    Stream *hwSerial;                   // Ne fonctionne pas sans ça, même si ce n'est pas utilisé... A investiguer.
    int8_t receiverSerialRx;            // If SW serial is used, stores the Rx pin of the µC.
    int8_t receiverSerialTx;            // If SW serial is used, stores the Tx pin of the µC.
    SoftwareSerial *controllerSerial;   // If SW serial is used, stores the port instance.
    HardwareSerial *hwControllerSerial; // If HW serial is used, stores the port instance.
    bool btState = false;               // Stores the state of the bluetooth connection if known.
    // The HM-10 state pin is at level high when connected, but there are 2 possible behaviour when not connected:
    // 1- state pin oscillates between high and low (500 ms between a change)
    // 2- state pin is low
    // This behaviour can be changed via AT commands : send AT+PIO10 for behaviour 1, and AT+PIO11 for behaviour 2.
    // We will be in behaviour 2 for the purpose of this project.

    // The HC-05 modules that I own have their STATE pin flicker between HIGH and LOW, so it is not usable to determine if there is a connection.

    int8_t analogNb_hw;                     // Stores the number of physical analog inputs on the receiver.
    int8_t digitalNb_hw;                    // Stores the number of physical digital inputs on the receiver.
    int8_t analogPin[NB_MAX_DATA];          // Stores the pin number of the analog inputs.
    int8_t digitalPin[NB_MAX_DATA];         // Stores the pin number of the digital inputs. When available, the Bluetooth state pin is the first index of the wired sensors.
    bool digitalInputPullup[NB_MAX_DATA];   // Index i = true if the digital input at index i needs to be used as INPUT_PULLUP or INPUT.
    bool digitalReversedLogic[NB_MAX_DATA]; // Index i = true if the digital input at index i is at level LOW when idle (normal case is idle HIGH).
    int16_t analog[NB_MAX_DATA * 2];        // Data received via Bluetooth is stored in the first NB_MAX_DATA indexes, data from wired sensors is stored in the last NB_MAX_DATA indexes.
    int16_t digital[NB_MAX_DATA * 2];       // Data received via Bluetooth is stored in the first NB_MAX_DATA indexes, data from wired sensors is stored in the last NB_MAX_DATA indexes.
    int16_t lastAnalog[NB_MAX_DATA * 2];    // Stores the last analog state.
    int16_t lastDigital[NB_MAX_DATA * 2];   // Stores the last digital state.
    int16_t threshold[NB_MAX_DATA];         // Holds the threshold for the analog values. If old_data-threshold <= incoming data <= old_data+threshold, then the incoming data is considered the same as the old data.
    UPDATED isUpdated;
    bool first_data = true;

    // create two EasyTransfer objects.
    SoftEasyTransfer ETin, ETout;   // Used in case of a SW serial port.
    EasyTransfer ETin_hw, ETout_hw; // Used in case of a HW serial port.
    // give a name to the group of data
    RECEIVE_DATA_STRUCTURE rxdata;
    SEND_DATA_STRUCTURE txdata;

    Universal_Receiver(int8_t rx, int8_t tx, int8_t digNb_hw, int8_t anaNb_hw, int8_t digPins[NB_MAX_DATA], int8_t anaPins[NB_MAX_DATA], bool digInputPullup[NB_MAX_DATA], bool digReversedLogic[NB_MAX_DATA], int16_t thresh[NB_MAX_DATA], String btHardware); // Class constructor for a Software Serial port.
    // Not all pins on the Mega and Mega 2560 support change interrupts, so only the following can be used for RX: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), A15 (69).
    // Not all pins on the Leonardo and Micro support change interrupts, so only the following can be used for RX: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
    // On Arduino or Genuino 101 RX doesn't work on Pin 13
    Universal_Receiver(HardwareSerial *stream, int8_t digNb_hw, int8_t anaNb_hw, int8_t digPins[NB_MAX_DATA], int8_t anaPins[NB_MAX_DATA], bool digInputPullup[NB_MAX_DATA], bool digReversedLogic[NB_MAX_DATA], int16_t thresh[NB_MAX_DATA], String btHardware); // Class constructor for a Hardware Serial port.
    Universal_Receiver(HardwareSerial *stream, String btHardware);                                                                                                                                                                                                // Class constructor for a Hardware Serial port.
    Universal_Receiver(int8_t rx, int8_t tx, String btHardware);                                                                                                                                                                                                  // Class constructor for a Hardware Serial port.

    void start(int8_t digNb_hw, int8_t anaNb_hw, int8_t digPins[NB_MAX_DATA], int8_t anaPins[NB_MAX_DATA], bool digInputPullup[NB_MAX_DATA], bool digReversedLogic[NB_MAX_DATA], int16_t thresh[NB_MAX_DATA], String btHardware); // Initiates the non-serial port variables.

    bool state();                      // Check if bluetooth is connected
    bool receivedDataFromController(); // Check if new data has arrived from the controller via bluetooth.
    bool updateWiredInput();           // Check if new data has arrived from the hardware inputs.
    bool receivedData();               // Checks if new data has arrived. Used should use this function and not the previous two.
    bool digitalState(int8_t ii);      // Returns the state of the digital input at index ii.
    bool digitalFalling(int8_t ii);    // True if digital input goes from HIGH to LOW. False if not.
    bool digitalRising(int8_t ii);     // True if digital input goes from LOW to HIGH. False if not.
    bool flushSerialPort();            // Flushes Serial Port.
};

#endif