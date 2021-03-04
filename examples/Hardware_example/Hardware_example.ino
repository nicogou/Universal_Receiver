#include <Universal_Receiver.h>

#define RX 2
#define TX 3

int digNb_hw = 1;
int anaNb_hw = 2;
int digPins[NB_MAX_DATA];
int anaPins[NB_MAX_DATA];
bool digInputPullup[NB_MAX_DATA];
bool digReversedLogic[NB_MAX_DATA];
int16_t thresholds[NB_MAX_DATA * 2];

Universal_Receiver *rec;

void setup()
{
    digPins[0] = 6;
    anaPins[0] = A0;
    anaPins[1] = A1;
    digInputPullup[0] = true;
    digReversedLogic[0] = false;
    for (int ii = 0; ii < NB_MAX_DATA; ii++)
    {
        thresholds[ii] = 30;
    }

    rec = new Universal_Receiver(RX, TX, digNb_hw, anaNb_hw, digPins, anaPins, digInputPullup, digReversedLogic, thresholds, BT_HW_HC05);
    Serial.println("Start receiver");
}

void loop()
{
    // put your main code here, to run repeatedly:
    if (rec->state())
    {
        if (rec->receivedData()) // Check if we received data (either bluetooth or hardware input)
        {
            if (rec->isUpdated.bluetooth()) // If we received new bluetooth data.
            {
                Serial.print("\t\tBluetooth Inputs: ");
                for (int ii = 0; ii < rec->rxdata.digitalNb; ii++)
                {
                    Serial.print(ii + 1);
                    Serial.print(":");
                    Serial.print(rec->digital[ii]);
                    Serial.print(", ");
                    Serial.print(rec->lastDigital[ii]);
                    Serial.print("\t");
                }
                for (int ii = 0; ii < rec->rxdata.analogNb; ii++)
                {
                    Serial.print(ii + 1);
                    Serial.print(":");
                    Serial.print(rec->analog[ii]);
                    Serial.print(", ");
                    Serial.print(rec->lastAnalog[ii]);
                    Serial.print("\t");
                }
            }
            if (rec->isUpdated.hardware()) // If we received new hardware data.
            {
                Serial.print("\t\tHardware Inputs: ");
                for (int ii = 0; ii < rec->digitalNb_hw; ii++)
                {
                    Serial.print(ii + 1);
                    Serial.print(":");
                    Serial.print(rec->digital[ii + NB_MAX_DATA]);
                    Serial.print(", ");
                    Serial.print(rec->lastDigital[ii + NB_MAX_DATA]);
                    Serial.print("\t");
                }
                for (int ii = 0; ii < rec->analogNb_hw; ii++)
                {
                    Serial.print(ii + 1);
                    Serial.print(":");
                    Serial.print(rec->analog[ii + NB_MAX_DATA]);
                    Serial.print(", ");
                    Serial.print(rec->lastAnalog[ii + NB_MAX_DATA]);
                    Serial.print("\t");
                }
            }

            // If we printed something, we go to the next line for the next batch of data.
            if (rec->isUpdated.bluetooth() || rec->isUpdated.hardware())
            {
                Serial.println();
            }
        }
    }
}