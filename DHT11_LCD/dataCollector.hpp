#include "TimeLib.h"
#include "LinkedList.h"
#include "EasyStringStream.h"

struct TemperatureDataEntry {
    int temp;
    int humidity;
    time_t timeOfTheDay;
};

class HistoryManager {
    LinkedList<TemperatureDataEntry> dataList =  {};

public:
    void add(int temp, int humidity) {
        TemperatureDataEntry entry;
        entry.temp = temp;
        entry.humidity = humidity;
        entry.timeOfTheDay = now();
        dataList.add(entry);
    }

    void sendDataToBt(SoftwareSerial &bluetooth) {
        
        char test[1024];
        EasyStringStream result(test, 1024);
        result<<"h";
        for(int i=0; i<dataList.size(); i++){
            TemperatureDataEntry entry = dataList.get(i);
            result<<entry.temp<<"."<<entry.humidity<<"."<<entry.timeOfTheDay<<"|";
        }
        bluetooth.write(result.get());
    }
}
