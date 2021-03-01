#include <Universal_Receiver.h>

#define RX 2
#define TX 3
int zeros[NB_MAX_DATA];
bool falses[NB_MAX_DATA];
int16_t thresholds[NB_MAX_DATA * 2];

Universal_Receiver *rec;

void setup()
{
    for (int ii = 0; ii < NB_MAX_DATA; ii++)
    {
        zeros[ii] = 0;
        falses[ii] = false;
    }
    for (int ii = 0; ii < NB_MAX_DATA * 2; ii++)
    {
        thresholds[ii] = 4;
    }
    rec = new Universal_Receiver(RX, TX, 0, 0, zeros, zeros, falses, falses, thresholds, BT_HW_HC05);
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