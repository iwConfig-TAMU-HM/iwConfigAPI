#include "iwconfigAPI.h"

using namespace std;

int main (){

   string essid_name;
   string sap;
   double Power;
   double SigLevel;
   double freq;   
   double bitrate;   
   double RTS;   
   int mode;
   int chan;

   iwconfigAPI wifiAPI;

    int cnt = wifiAPI.getWIFICount();
    vector<string> wifi = wifiAPI.getWIFIList();

    for(int i = 0; i < cnt; i++) {
      wifiAPI.setTXPower(wifi[i],on,23);
      wifiAPI.setFrequency(wifi[i],5.5, GHz);
      wifiAPI.setRTS(wifi[i],rtsauto,256);
      wifiAPI.setFrag(wifi[i],rtsauto,512);
      wifiAPI.setRetry(wifi[i],24);
      wifiAPI.setMode(wifi[i],Managed);
      wifiAPI.setAccessPoint(wifi[i], "any" );
      //essid_name = "Dummy";
      //essid_name.append(to_string(i));
      //setESSID(wifi[i],essid_name);

      essid_name = wifiAPI.getESSID(wifi[i]); 
      cout << wifi[i] << " ESSID" << i << ": " << essid_name << endl;
     Power = wifiAPI.getTX_Power(wifi[i]); 
      cout << wifi[i] << " TXPower" << i << ": " << Power << " dBm" << endl;
      SigLevel = wifiAPI.getSignalLevel(wifi[i]);  
      cout << wifi[i] << " Signal_Level" << i << ": " << SigLevel << " dBm" << endl;
      freq = wifiAPI.getFrequency(wifi[i]);
      cout << wifi[i] << " Frequency" << i << ": " << freq << " Hz" << endl;
      chan = wifiAPI.getChannel(wifi[i]);
      cout << wifi[i] << " Channel" << i << ": " << chan << endl;
      mode = wifiAPI.getMode(wifi[i]);
      cout << wifi[i] << " Mode" << i << ": " << mode << endl;
      bitrate = wifiAPI.getBitRate(wifi[i]); 
     cout << wifi[i] << " Bit Rate" << i << ": " << bitrate << endl;
      RTS = wifiAPI.getRTS(wifi[i]); 
     cout << wifi[i] << " RTS" << i << ": " << RTS << endl;
      RTS = wifiAPI.getFrag(wifi[i]); 
     cout << wifi[i] << " Frag" << i << ": " << RTS << endl;
     sap = wifiAPI.getAccessPoint(wifi[i]);
      cout << wifi[i] << " ap" << i << ": " << sap << endl;
      RTS = wifiAPI.getRetry(wifi[i]);
     cout << wifi[i] << " Retry" << i << ": " << RTS << endl;
    }
  
  return 0;
    }

 
