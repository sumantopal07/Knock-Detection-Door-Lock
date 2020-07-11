#include <Servo.h>
Servo myservo;
int pos = 0;
const int knockSensor = A0;
const int programSwitch = 8;
const int threshold = 100;
const int rejectValue = 25;
const int averageRejectValue = 15;
const int knockFadeTime = 150;
const int lockTurnTime = 650;
const int maximumKnocks = 10;
const int knockComplete = 1200;
int secretCode[10] = {100, 100, 0, 00, 00, 0, 0, 0, 0, 0};
int knockReadings[maximumKnocks];
int knockSensorValue = 0;
int programButtonPressed = false;
void setup()
{
    myservo.attach(3);
    pinMode(programSwitch, INPUT);
    digitalWrite(programSwitch, HIGH);
    Serial.begin(9600);
    Serial.println("Program start.");
}
void loop()
{
    knockSensorValue = analogRead(knockSensor);
    Serial.println(digitalRead(programSwitch));
    if (digitalRead(programSwitch) == 0)
    {
        programButtonPressed = true;
    }
    else
    {
        programButtonPressed = false;
    }

    if (knockSensorValue >= threshold)
    {
        listenToSecretKnock();
    }
}
// Records the timing of knocks.
void listenToSecretKnock()
{
    Serial.println("knock starting");
    int i = 0;
    // First lets reset the listening array.
    for (i = 0; i < maximumKnocks; i++)
    {
        knockReadings[i] = 0;
    }
    int currentKnockNumber = 0;     // Incrementer for the
    array.int startTime = millis(); // Reference for when this
    knock started.int now = millis();
    do
    {
        //listen for the next knock or wait for it to timeout.
        knockSensorValue = analogRead(knockSensor);
        if (knockSensorValue >= threshold)
        { //got
            another knock...
                //record the delay time.
                Serial.println("KNOCK");
            now = millis();
            knockReadings[currentKnockNumber] = now - startTime;
            currentKnockNumber++;
            startTime = now;
            delay(knockFadeTime);
        }
        now = millis();
    } while ((now - startTime < knockComplete) &&
             (currentKnockNumber < maximumKnocks));

    if (programButtonPressed == false)
    {
        if (validateKnock() == true)
        {
            triggerDoorUnlock();
        }
        else
        {
            Serial.println("Secret knock failed.");
            delay(100);
        }
    }
    else
    {
        validateKnock();
        Serial.println("New lock stored.");
        for (i = 0; i < 10; i++)
        {
            Serial.println(secretCode[i]);
        }
        delay(900);
    }
}
void triggerDoorUnlock()
{
    Serial.println("Door unlocked!");
    for (pos = 0; pos <= 180; pos += 1)
    {
        myservo.write(pos);
        delay(3);
    }
    for (pos = 180; pos >= 0; pos -= 1)
    { // goes from 180
        degrees to 0 degrees
            myservo.write(pos);
        delay(3);
    }
}
boolean validateKnock()
{
    int i = 0;
    int currentKnockCount = 0;
    int secretKnockCount = 0;
    int maxKnockInterval = 0; // We use this later to
    normalize the times.

        for (i = 0; i < maximumKnocks; i++)
    {
        if (knockReadings[i] > 0)
        {
            currentKnockCount++;
        }
        if (secretCode[i] > 0)
        { //todo: precalculate this.
            secretKnockCount++;
        }

        if (knockReadings[i] > maxKnockInterval)
        { // collect
            normalization data while we're looping. maxKnockInterval = knockReadings[i];
        }
    }
    if (programButtonPressed == true)
    {
        for (i = 0; i < maximumKnocks; i++)
        { // normalize the times
            secretCode[i] = map(knockReadings[i], 0,
                                maxKnockInterval, 0, 100);
        }
        delay(1000);
        for (i = 0; i < maximumKnocks; i++)
        {
            if (secretCode[i] > 0)
            {
                delay(map(secretCode[i], 0, 100, 0,
                          maxKnockInterval)); // Expand the time back out to what it
                was.Roughly.
            }
            delay(50);
        }
        return false; // We don't unlock the door when we are
        recording a new knock.
    }

    if (currentKnockCount != secretKnockCount)
    {
        return false;
    }

    //error removal
    int totaltimeDifferences = 0;
    int timeDiff = 0;
    for (i = 0; i < maximumKnocks; i++)
    {
        knockReadings[i] = map(knockReadings[i], 0,
                               maxKnockInterval, 0, 100);
        timeDiff = abs(knockReadings[i] - secretCode[i]);
        if (timeDiff > rejectValue)
        {
            return false;
        }
        totaltimeDifferences += timeDiff;
    }
    if (totaltimeDifferences / secretKnockCount > averageRejectValu
                                                      e)
    {
        return false;
    }
    return true;
}