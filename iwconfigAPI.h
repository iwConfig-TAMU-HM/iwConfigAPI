/*****************************************************************************************
* Title: 	iwconfigAPI                                                              *
* Purpose: 	iwconfigAPI is dedicated to the wireless interfaces. It is used to set   *
* 		the parameters of the network interface which are specific to the        *
*		wireless operation (for example :   					 *
* 		the frequency). iwconfigAPI may also be used to display those parameters,*
*		and the wireless statistics (extracted from /proc/net/wireless).         *
*                                                                                        *
*		All these parameters and statistics are device dependent. Each driver    *
*		will provide only some of them depending on hardware support, and the    *
*		range of values may change.                                              *
*                                                                                        *
* Created: 	11/24/2019                                                               *
* Author:	David R. Carey Ph.D. 							 *
*		Hiller Measurements LLC							 *
*		david.carey@hillermeas.com						 *
*****************************************************************************************/
/*****************************************************************************************
* Include Files                                                                          *
*****************************************************************************************/
#include<iostream> 
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sstream>
#include<vector>
#include<ctype.h>

/*****************************************************************************************
* Macros and Constants                                                                   *
*****************************************************************************************/
#ifndef _IWCONFIGAPI
#define _IWCONFIGAPI

using namespace std;
// This enumeration type is used when setting txpower. 
enum txMode {automatic, off,on,dBm,mW};
// This enumeration type is used when setting frequency/channel. 
enum fMode {setfreq, setchannel};
enum fUnits {raw,kHz,MHz,GHz};
// This enumeration is used for setting adapter Mode
enum mMode {AdHoc, Managed, Master, Repeater, Secondary, Monitor, Automatic};
// This enumeration type is used when setting RTS Threshold. 
enum RTSmode {rtsauto, rtsoff, rtsfixed, rtsbyte};

/*****************************************************************************************
* Class Decalration                                                                      *
*****************************************************************************************/
class iwconfigAPI
{ 
   private:
/*****************************************************************************************
* Get Standard Outout from Command: This function will capture the information usually   * 
* dispalyed on the terminal and return it as a string.                                   *
*                    adapter/interface names.                                            *
*        output: string containing command standard output                               *
*        input: cmd - string containing the command to be executed                       * 
*****************************************************************************************/
	string GetStdoutFromCommand(string cmd) {
	    string data;
	    FILE * stream;
	    const int max_buffer = 256;
	    char buffer[max_buffer];
	    // append 2>&1 to  command this will direct the stdout to the stream
	    cmd.append(" 2>&1");
	    // execute the command specified by the string cmd.
	    stream = popen(cmd.c_str(), "r");
	    if (stream) { // if stream is true then command has executed. 
	    while (!feof(stream)) // loop on stream until end of file is reached. 
	    if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
	    pclose(stream); // close the command stream
	    }
	    return data;
	}
   public: 
/*****************************************************************************************
* wifi Adapter List: This function will use iwconfig command to identify all wifi        * 
*                    names. The function returns an array of strings containing the      *
*                    adapter/interface names.                                            *
*                    iwconfig will output a list of all avaialble interfaces along with  *
*                    interface parameters. If the interface is not avaialble as a wifi   *
*                    interface then "no wireless extensions." will be returned.          *
*                    This will look for all valid interfaces.                            *
*        output: string array containing the wifi adapter/interface names                *
*        input: void                                                                     * 
*****************************************************************************************/
	vector<string> getWIFIList(){ 
	    string data;
	    int posCR;
	    int posSP;
	    vector<string> data_array;
	    // Execute iwconfig command to find all interfaces. 
	    string iwconfig = GetStdoutFromCommand("iwconfig");
	    string line;    
	    // Break returned string into lines. 
	    while ((posCR = iwconfig.find('\n',1)) < iwconfig.length()) {
		posCR = iwconfig.find('\n',1);
		line = iwconfig.substr(0,posCR); // extract current line
		if (line.length() > 1) { // not a blank line
			// look for "no wireless extensions." if not found process line. 
			if(line.find("no wireless extensions.",1) == string::npos){
				if (line.at(1) != ' ') { // if first character is not a space
				// then the line contains an interface name.  
					posSP = line.find(' ',1);// find first space
				        line = line.substr(1,posSP);// extract interface name
					data_array.push_back(line);// append name to array.
					}
			    	}
			}
		iwconfig.erase(0,posCR);// clear out current line. 
	    }
	    return data_array;
	}
/*****************************************************************************************
* wifi Adapter Count: This function will use iwconfig command to identify all wifi       * 
*                     names. The function returns a count of valid adapters.             *
*        output: integer containing the count of wifi adapters/interfaces                *
*        input: void                                                                     * 
*****************************************************************************************/
	int getWIFICount(){ 
	    int posCR;
	    int cnt = 0;
	    // Execute iwconfig command to find all interfaces. 
	    string iwconfig = GetStdoutFromCommand("iwconfig");
	    string line;    
	    // Break returned string into lines. 
	    while ((posCR = iwconfig.find('\n',1)) < iwconfig.length()) {
		posCR = iwconfig.find('\n',1);
		line = iwconfig.substr(0,posCR);// extract current line
		if (line.length() > 1) {// not a blank line
			// look for "no wireless extensions." if not found process line. 
			if(line.find("no wireless extensions.",1) == string::npos){
				if (line.at(1) != ' ') {// if first character is not a space
				       // then the line contains an interface name.  
					cnt += 1;//increment infterface count
					}
			    	}
			}
		iwconfig.erase(0,posCR);// clear out current line.
	    }
	    return cnt;
	}

/*****************************************************************************************
* Set the ESSID (or Network Name - in some products it may also be called Domain ID).    *
* The ESSID is used to identify cells which are part of the same virtual network.        *
* As opposed to the AP Address or NWID which define a single cell, the ESSID defines a   *
* group of cells connected via repeaters or infrastructure, where the user may roam      *
* transparently.                                                                         *
* With some cards, you may disable the ESSID checking (ESSID promiscuous) with off or any*
* (and on to reenable it).                                                               *
* getESSID and setESSID                                                                  *
*****************************************************************************************/
/*****************************************************************************************
* string getESSID(string wifi)                                                           * 
*        output: string containing the ESSID name                                        *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	string getESSID(string wifi){
	    size_t found;
	    size_t firstQuote;
	    size_t secondQuote;
	    size_t ESSIDLen;
	    string data = "off/any";
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    string iwconfig = GetStdoutFromCommand(cmd);
	    found  = iwconfig.find("ESSID:",1);
	    if (found != string::npos){ // found keyword
	 	   found = found + 6;             
		   size_t firstQuote = iwconfig.find("\"",found);
			    if (firstQuote != string::npos){ // found quote
				   firstQuote = firstQuote + 1;
				   secondQuote = iwconfig.find("\"",firstQuote);
				   ESSIDLen = secondQuote - firstQuote; 
				}
			    else { // off/any
				   firstQuote = found;
	 			   ESSIDLen = 7; 
				}
		   data = iwconfig.substr(firstQuote,ESSIDLen);
	    }
	    return data;
	}

/*****************************************************************************************
* string setESSID(string wifi)                                                           * 
*        output: void                                                                    *
*        input: string containing wifi adapter name. Valid strings: any, on, off or      * 
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	void setESSID(string wifi, string value ){
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    cmd.append(" essid ");
	    cmd.append(value); //value can be a name or the following keyword: any, on, off
	    string iwconfig = GetStdoutFromCommand(cmd); // this command has no return
	    return;
	}

/*****************************************************************************************
* get TX Power: getTX_Power(string wifi)                                                 * 
* Uses iwconfig to poll the interface TX power. The function will search for the string  *
* "Tx-Power=" The next three characters will be the keyword "off" or the power in dBm.   *
* If the interface is off then the function will return -174 dBm (this number should be  *
* below the noise floor of the receiver.                                                 * 
*                                                                                        *
*        output: double containing TX power in dBm                                       *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	double getTX_Power(string wifi){
	    size_t found;
	    double data = -174.0;
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    string iwconfig = GetStdoutFromCommand(cmd);
	    found  = iwconfig.find("Tx-Power=",1);
	    if (found != string::npos){ // found txpower keyword
	 	   found = found + 9;             
		   string sdata =iwconfig.substr(found,3);
		  if (sdata.compare("off") != 0){ // did not find off
	 	   	data = stof(sdata);
			}
	    }
	    return data;
	}

/*****************************************************************************************
* set TX Power: setTXPower(string wifi, txMode mode, int value )                         * 
* Uses iwconfig to set the interface TX power. For cards supporting multiple transmit    *
* powers, sets the transmit power in dBm. If W is the power in Watt, the power in dBm is * 
* P = 30 + 10.log(W). If the value is postfixed by mW, it will be automatically converted*
* to dBm.                                                                                *
* In addition, on and off enable and disable the radio, and auto and fixed enable and    *
* disable power control (if those features are available).                               *
*                                                                                        *
*        output: void                                                                    *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*               mode - enumeration containing:                                           *
*                      automatic = 1 Set TX Power to auto                                *
*                      off = 2 set TX Power off                                          *
*                      on = 3 set TX Power on (restore to previous level)                *
*                      dBm = 4 set power with units dBm                                  *
*                      mW = 5 set poer with units mW                                     *
*               value - the numeric value of the TX power in units set by mode.          *
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	void setTXPower(string wifi, txMode mode, int value ){
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    cmd.append(" txpower ");
		 switch (mode) { 
		    case automatic:
			cmd.append("auto"); 
			break; 
		    case off: 
			cmd.append("off"); 
			break; 
		    case on: 
			cmd.append("on"); 
			break; 
		    case dBm: 
			cmd.append(to_string(value));
			break; 
		    case mW: 
			cmd.append(to_string(value));
			cmd.append("mW"); 
			break; 
		    default: 
			cmd.append(to_string(value));
		    } 
	    string iwconfig = GetStdoutFromCommand(cmd); // this command has no return
	    return;
	}

/*****************************************************************************************
* get Signal Level: double getSignalLevel(string wifi)                                   *
* Uses iwconfig to poll the interface received signal level. The function will search for*
* the string "Signal level=" The next three characters will be the keyword "off" or the  *
* power in dBm.                                                                          *
* If the interface is off then the function will return -174 dBm (this number should be  *
* below the noise floor of the receiver.                                                 * 
*                                                                                        *
*        output: double containing recieved signal level in dBm                          *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	double getSignalLevel(string wifi){
	    size_t found;
	    double data = -174;
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    string iwconfig = GetStdoutFromCommand(cmd);
	    found  = iwconfig.find("Signal level=",1);
	    if (found != string::npos){ // found keyword
	 	   found = found + 13;
		   string sdata =iwconfig.substr(found,3);
		  if (sdata.compare("off") != 0){ // did not find off
	 	   	data = stof(sdata);
			}
	    }
	    return data;
	}
/*****************************************************************************************
* set RSSI: void setSensitivity(string wifi, int value )                                 *
* Received signal strength (RSSI - how strong the received signal is).                   *
* Set the sensitivity threshold. This define how sensitive is the card to poor operating *
* conditions (low signal, interference). Positive values are assumed to be the raw value *
* used by the hardware or a percentage, negative values are assumed to be dBm. Depending *
* on the hardware implementation, this parameter may control various functions.          *
* On modern cards, this parameter usually control handover/roaming threshold, the lowest *
* signal level for which the hardware remains associated with the current Access Point.  *
* When the signal level goes below this threshold the card starts looking for a          *
* new/better Access Point. Some cards may use the number of missed beacons to trigger    *
* this. For high density of Access Points, a higher threshold make sure the card is      *
* always associated with the best AP, for low density of APs, a lower threshold minimize *
* the number of failed handoffs.                                                         *
*        output: void                                                                    *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*               value - RSSI level in dBm                                                *
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	void setSensitivity(string wifi, int value ){
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    cmd.append(" sens ");
	    cmd.append(to_string(value));
	    string iwconfig = GetStdoutFromCommand(cmd); // this command has no return
	    return;
	}

/*****************************************************************************************
* get Frequency: double getFrequency(string wifi)                                        *
* Get the operating frequency in the device in Hz.                                       *
*        output: double Frequency in Hz                                                  *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	double getFrequency(string wifi){
	    double data = 0;
	    string cmd = "iwgetid ";
	    cmd.append(wifi);
	    cmd.append(" --raw --freq");
	    string sfreq = GetStdoutFromCommand(cmd);
	    if (sfreq.length() > 0){ // frequency returned
	 	   	data = stof(sfreq);
	    }
	    return data;
	}
/*****************************************************************************************
* set Frequency: void setFrequency(string wifi, double value, fUnits units)              *
* Set the operating frequency in the device in Hz. You may append the suffix k, M or G to*
* the value (for example, "2.46G" for 2.46 GHz frequency), or add enough '0'.            * 
* Depending on regulations, some frequencies may not be available.                       *
* When using Managed mode, most often the Access Point dictates the frequency and the    *
* driver may refuse the setting of the frequency. In Ad-Hoc mode, the frequency setting  *
* may only be used at initial cell creation, and may be ignored when joining an existing *
* cell.                                                                                  *
*        output: void                                                                    *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*               value - double value of the frequency in units defined in units          *
*                       enumeration                                                      *
*               units - enumeration containing one of the following:                     *
*                       raw = 1 value is input in Hz enter as an integer                 *
*                             for example 5.5 GHz is 5500000000                          *
*                       kHz = 2 value is in kiloHertz 1000's                             *
*                             for example 5.5 GHz is 5500000                             *
*                       MHz = 3 value is in MegaHertz 1000000's                          *
*                             for example 5.5 GHz is 5500                                *
*                       GHz = 4 value is in GigaHertz 1000000000's                       *                     
*                             for example 5.5 GHz is 5.5                                 *
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	void setFrequency(string wifi, double value, fUnits units){
		string cmd = "iwconfig ";
		cmd.append(wifi);
		cmd.append(" freq ");
		cmd.append(to_string(value));
		 switch (units) { 
		    case raw:
			cmd.append(" ");
			break; 
		    case kHz: 
			cmd.append(" k");
			break; 
		    case MHz: 
			cmd.append(" M");
			break; 
		    case GHz: 
			cmd.append(" G");
			break; 
		    default: //raw
			cmd.append(" ");
		    } 
		string iwconfig = GetStdoutFromCommand(cmd); // this command has no return
		return;
	}

/*****************************************************************************************
* get channel: double getChannel(string wifi)                                            *
* Get the operating channel in the device. A value below 1000 indicates a channel number *
* Channels are usually numbered starting at 1. Depending on regulations, some channels   *
* may not be available.                                                                  *
*        output: void                                                                    *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	int getChannel(string wifi){
	    int data = 0;
	    string cmd = "iwgetid ";
	    cmd.append(wifi);
	    cmd.append(" --raw --channel");
	    string schan = GetStdoutFromCommand(cmd);
	    if (schan.length() > 0){ // channel returned
	 	   	data = stoi(schan);
	    }
	    return data;
	}
/*****************************************************************************************
* set channel: void setChannel(string wifi, int value)                                   *
* Set the operating channel in the device. A value below 1000 indicates a channel number *
* Channels are usually numbered starting at 1. Depending on regulations, some channels   *
* may not be available.                                                                  *
* When using Managed mode, most often the Access Point dictates the channel and the      *
* driver may refuse the setting of the channel. In Ad-Hoc mode, the channel setting may  *
* only be used at initial cell creation, and may be ignored when joining an existing cell*
* You may also use off or auto to let the card pick up the best channel (when supported).*
*        output: void                                                                    *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*               value - integer value of the channel                                     *
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	void setChannel(string wifi, int value){
		string cmd = "iwconfig ";
		cmd.append(wifi);
		cmd.append(" channel "); 
		if ( value > 0){ cmd.append(to_string(value));}
		else {	cmd.append("auto");}
		string iwconfig = GetStdoutFromCommand(cmd); // this command has no return
		return;
	}
/*****************************************************************************************
* get Mode: int getMode(string wifi)                                                     * 
*        Uses iwgetid to return the current mode of the interface.                       *
*        output: integer mapped to the mode enumeration:                                 *
*                AdHoc = 1                                                               *
*                Managed = 2                                                             *
*                Master = 3                                                              * 
*                Repeater = 4                                                            * 
*                Secondary = 5                                                           * 
*                Monitor = 6                                                             * 
*                Auto = 7                                                                *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	int getMode(string wifi){
	    int data = Managed;
	    string cmd = "iwgetid ";
	    cmd.append(wifi);
	    cmd.append(" --raw --mode");

	    string sMode = GetStdoutFromCommand(cmd);
	    if (sMode.length() > 0){ // Mode returned
	 	   	data = stoi(sMode);
	    }
	    return data;
	}
/*****************************************************************************************
* set Mode: void setMode(string wifi, mMode mode)                                        *
* Uses iwconfig to set the interface operating mode. Set the operating mode of the device*
* which depends on the network topology. The mode can be Ad-Hoc (network composed of only*
* one cell and without Access Point), Managed (node connects to a network composed of    *
* many Access Points, with roaming), Master (the node is the synchronisation master or   *
* acts as an Access Point), Repeater (the node forwards packets between other wireless   *
* nodes), Secondary (the node acts as a backup master/repeater), Monitor (the node is not*
* associated with any cell and passively monitor all packets on the frequency) or Auto.  *
*        output: void                                                                    *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*               mode - integer mapped to the mode enumeration:                           *
*                AdHoc = 1                                                               *
*                Managed = 2                                                             *
*                Master = 3                                                              * 
*                Repeater = 4                                                            * 
*                Secondary = 5                                                           * 
*                Monitor = 6                                                             * 
*                Auto = 7                                                                *
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	void setMode(string wifi, mMode mode){
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    cmd.append(" mode ");
		 switch (mode) { 
		    case AdHoc:
			cmd.append("Ad-Hoc");
			break; 
		    case Managed: 
			cmd.append("Managed");
			break; 
		    case Master: 
			cmd.append("Master");
			break; 
		    case Repeater: 
			cmd.append("Repeater");
			break; 
		    case Secondary: 
			cmd.append("Secondary");
			break; 
		    case Monitor: 
			cmd.append("Monitor");
			break; 
		    case Automatic: 
			cmd.append("auto");
			break; 
		    default: //Automatic
			cmd.append("auto");
		    } 
	    string sMode = GetStdoutFromCommand(cmd);
	    return;
	}
/*****************************************************************************************
* get Access Point: string getAccessPoint(string wifi)                                   *
* Uses iwgetid to return the MAC address of the Wireless Access Point or the Cell.       *
* An address equal to 00:00:00:00:00:00 means that the card failed to associate with an  *
* Access Point (most likely a configuration issue). The Access Point parameter will be   *
* shown as Cell in ad-hoc mode (for obvious reasons), but otherwise works the same.      *
*        output: string containing the access point address                              *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	string getAccessPoint(string wifi){
	    string sap = "No Access Point";
	    string tsap;
	    string cmd = "iwgetid ";
	    cmd.append(wifi);
	    cmd.append(" --raw --ap");
	    tsap = GetStdoutFromCommand(cmd);
	    if (tsap.length() > 0){ // access point returned
	 	   	sap = tsap;
	    }
	    return sap;
	}
/*****************************************************************************************
* set Access Point: void setAccessPoint(string wifi, string value)                       *
* Uses iwconfig to set the MAC address of the Wireless Access Point or the Cell.         *
*        output: void                                                                    *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*               value - string containing the access point address.                      *
*                       may contain keywords any or off or mac address                   *
*                       with the format 00:00:00:00:00:00                                * 
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	void setAccessPoint(string wifi, string value ){
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    cmd.append(" ap ");
	    cmd.append(value);  
	    string iwconfig = GetStdoutFromCommand(cmd); // this command has no return
	    return;
	}
/*****************************************************************************************
* nick: This command was unsupported.  
* Set the nickname, or the station name. Some 802.11 products do define it, but this is  *
* not used as far as the protocols (MAC, IP, TCP) are concerned and completely useless as*
* far as configuration goes. Only some wireless diagnostic tools may use it.             *
*****************************************************************************************/
/*****************************************************************************************
* get Bit Rate: double getBitRate(string wifi)                                           *
*        output: double containing the current bit rate in Mb/s                          *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	double getBitRate(string wifi){
	    size_t found;
	    double data = 0;
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    string iwconfig = GetStdoutFromCommand(cmd);
	    found  = iwconfig.find("Bit Rate=",1);
	    if (found != string::npos){ // found keyword
	 	   found = found + 9;
	           size_t spfound  = iwconfig.find("Mb/s",found)-found;
		   string sdata =iwconfig.substr(found,spfound);
     	   	   data = stof(sdata);
	    }
	    return data;
	}
/*****************************************************************************************
* set Bit Rate: double setBitRate(string wifi)                                           *
* For cards supporting multiple bit rates, set the bit-rate in b/s. The bit-rate is the  *
* speed at which bits are transmitted over the medium, the user speed of the link is     *
* lower due to medium sharing and various overhead.                                      *
* You may append the suffix k, M or G to the value (decimal multiplier : 10^3, 10^6 and  *
* 10^9 b/s), or add enough '0'.                                                          *
*        output: double containing the current bit rate                                  *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	void setBitRate(string wifi, double value, fUnits units){
		string cmd = "iwconfig ";
		cmd.append(wifi);
		cmd.append(" rate ");
		cmd.append(to_string(value));
		 switch (units) { 
		    case raw:
			cmd.append(" ");
			break; 
		    case kHz: 
			cmd.append(" k");
			break; 
		    case MHz: 
			cmd.append(" M");
			break; 
		    case GHz: 
			cmd.append(" G");
			break; 
		    default: //raw
			cmd.append(" ");
		    } 
		string iwconfig = GetStdoutFromCommand(cmd); // this command has no return
		return;
	}

/*****************************************************************************************
* get RTS Threshold: getRTS(string wifi)                                                 * 
* Uses iwconfig to poll the interface RTS Threshold. The function will search for the    *
* "RTS thr" The next three characters will be the keyword "off" or the RTS threshold in  *
* bytes.                                                                                 * 
*                                                                                        *
*        output: double containing RTS threshold                                         *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	double getRTS(string wifi){
	    size_t found;
	    double data = 0;
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    string iwconfig = GetStdoutFromCommand(cmd);
	    found  = iwconfig.find("RTS thr",1);
	    if (found != string::npos){ // found RTS thr keyword
	 	   found = found + 8;             
	           string sdata =iwconfig.substr(found,3);
		  if (sdata.compare("off") != 0){ // did not find off
			size_t spfound  = iwconfig.find("B",found)-found;
			sdata =iwconfig.substr(found,spfound);
	 	   	data = stof(sdata);
			}
	    }
	    return data;
	}

/*****************************************************************************************
* set RTS Threshold: setRTS(string wifi, RTSMode mode, int value )                       * 
* Uses iwconfig to set the interface RTS Threshold. RTS/CTS adds a handshake before each *
* packet transmission to make sure that the channel is clear. This adds overhead, but    *
* increases performance in case of hidden nodes or a large number of active nodes. This  *
* parameter sets the size of the smallest packet for which the node sends RTS ; a value  *
* equal to the maximum packet size disables the mechanism. You may also set this         *
* parameter to auto, fixed or off.                                                       *
*        output: void                                                                    *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*               mode - enumeration containing:                                           *
*                      rtsauto = 1 Set RTS Threshold to auto                             *
*                      rtsoff = 2 set RTS Threshold to off                               *
*                      rtsfixed = 3 set RTS Threshold to fixed                           *
*                      rtsbyte = 4 set RTS Threshold to number in value                  *
*               value - the numeric value of the RTS Threshold                           *
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	void setRTS(string wifi, RTSmode mode, int value ){
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    cmd.append(" rts ");
		 switch (mode) { 
		    case rtsauto:
			cmd.append("auto"); 
			break; 
		    case rtsoff: 
			cmd.append("off"); 
			break; 
		    case rtsfixed: 
			cmd.append("fixed"); 
			break; 
		    case rtsbyte: 
			cmd.append(to_string(value));
			break; 
		    default: //byte number
			cmd.append(to_string(value));
		    } 
	    string iwconfig = GetStdoutFromCommand(cmd); // this command has no return
	    return;
	}

/*****************************************************************************************
* get Fragment Threshold: getFrag(string wifi)                                           * 
* Uses iwconfig to poll the interface Fragment Threshold. The function will search for   *
* "Fragement thr" The next three characters will be the keyword "off" or the Fragment    *
* threshold in bytes.                                                                    * 
*                                                                                        *
*        output: double containing Fragment threshold                                    *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	double getFrag(string wifi){
	    size_t found;
	    double data = 0;
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    string iwconfig = GetStdoutFromCommand(cmd);
	    found  = iwconfig.find("Fragment thr",1);
	    if (found != string::npos){ // found RTS thr keyword
	 	   found = found + 13;             
	           string sdata =iwconfig.substr(found,3);
		  if (sdata.compare("off") != 0){ // did not find off
			size_t spfound  = iwconfig.find("B",found)-found;
			sdata =iwconfig.substr(found,spfound);
	 	   	data = stof(sdata);
			}
	    }
	    return data;
	}

/*****************************************************************************************
* set Fragment Threshold: setFrag(string wifi, RTSMode mode, int value )                 * 
* Uses iwconfig to set the interface Fragmentation RTS Threshold. Fragmentation allows to*
* split an IP packet in a burst of smaller fragments transmitted on the medium. In most  *
* cases this adds overhead, but in a very noisy environment this reduces the error       *
* penalty and allow packets to get through interference bursts. This parameter sets the  *
* maximum fragment size which is always lower than the maximum packet size.              *
* This parameter may also control Frame Bursting available on some cards, the ability to *
* send multiple IP packets together. This mechanism would be enabled if the fragment size*
* is larger than the maximum packet size.                                                *
* You may also set this parameter to auto, fixed or off.                                 *
*        output: void                                                                    *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*               mode - enumeration containing: (same enum as RTS)                        *
*                      rtsauto = 1 Set Frag Threshold to auto                            *
*                      rtsoff = 2 set Frag Threshold to off                              *
*                      rtsfixed = 3 set Frag Threshold to fixed                          *
*                      rtsbyte = 4 set Frag Threshold to number in value                 *
*               value - the numeric value of the Frag Threshold                          *
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	void setFrag(string wifi, RTSmode mode, int value ){
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    cmd.append(" frag ");
		 switch (mode) { 
		    case rtsauto:
			cmd.append("auto"); 
			break; 
		    case rtsoff: 
			cmd.append("off"); 
			break; 
		    case rtsfixed: 
			cmd.append("fixed"); 
			break; 
		    case rtsbyte: 
			cmd.append(to_string(value));
			break; 
		    default: //byte number
			cmd.append(to_string(value));
		    } 
	    string iwconfig = GetStdoutFromCommand(cmd); // this command has no return
	    return;
	}

/*****************************************************************************************
* key/enc: 
* Used to manipulate encryption or scrambling keys and security mode.                    *
* To set the current encryption key, just enter the key in hex digits as                 *
* XXXX-XXXX-XXXX-XXXX or XXXXXXXX. To set a key other than the current key, prepend or   *
* append [index] to the key itself (this won't change which is the active key). You can  *
* also enter the key as an ASCII string by using the s: prefix. Passphrase is currently  *
* not supported.                                                                         *
* To change which key is the currently active key, just enter [index] (without entering  *
* any key value).                                                                        *
* off and on disable and reenable encryption.                                            *
* The security mode may be open or restricted, and its meaning depends on the card used. *
* With most cards, in open mode no authentication is used and the card may also accept   *
* non-encrypted sessions, whereas in restricted mode only encrypted sessions are accepted*
* and the card will use authentication if available.                                     *
* If you need to set multiple keys, or set a key and change the active key, you need to  *
* use multiple key directives. Arguments can be put in any order, the last one will take *
* precedence.                                                                            *
*****************************************************************************************/
/*****************************************************************************************
* power: 
* Used to manipulate power management scheme parameters and mode.
* To set the period between wake ups, enter period 'value'. To set the timeout before    *
* going back to sleep, enter timeout 'value'. To set the generic level of power saving,  *
* enter saving 'value'. You can also add the min and max modifiers. By default, those    *
* values are in seconds, append the suffix m or u to specify values in milliseconds or   *
* microseconds. Sometimes, those values are without units (number of beacon periods,     *
* dwell, percentage or similar).                                                         *
* off and on disable and reenable power management. Finally, you may set the power       *
* management mode to all (receive all packets), unicast (receive unicast packets only,   *
* discard multicast and broadcast) and multicast (receive multicast and broadcast only,  *
* discard unicast packets).                                                              *
*****************************************************************************************/
/*****************************************************************************************
* retry: 
*****************************************************************************************/
/*****************************************************************************************
* get Retry Limits: getRetry(string wifi)                                                * 
* Uses iwconfig to poll the interface Retry Limit. The function will search for either   *
* "Retry short limit:" or "Retry short  long limit:" The next three characters will be   *
* Retry limit                                                                            *
*                                                                                        *
*        output: int containing retry Limit                                           *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	int getRetry(string wifi){
	    size_t found;
	    int data = 0;
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    string iwconfig = GetStdoutFromCommand(cmd);
	    found  = iwconfig.find("Retry short limit:",1);
	    if (found != string::npos){ // found Retry keyword
	 	   found = found + 18;             
	           string sdata =iwconfig.substr(found,3);
	 	   data = stoi(sdata);
	         }
	    else {
		    found  = iwconfig.find("Retry short  long limit:",1);
		    if (found != string::npos){ // found Retry keyword
		 	   found = found + 24;             
			   string sdata =iwconfig.substr(found,3);
		 	   data = stoi(sdata);
			 }
	         }
	    return data;
	}
/*****************************************************************************************
* set Retry Limits: setRetry(string wifi)                                                * 
* Uses iwconfig to set the interface Retry Limit.                                        *
* Most cards have MAC retransmissions, and some allow to set the behaviour of the retry  *
* mechanism.                                                                             *
* To set the maximum number of retries, enter limit 'value'. This is an absolute value   *
* (without unit), and the default (when nothing is specified).                           *
*                                                                                        *
*        output: void                                                                    *
*        input: wifi - string containing wifi adapter/interface name.                    * 
*               value - the numeric value of the Limit                                   *
*        Note: use getWIFIList to obtain a list of wifi adapter/interface names.         * 
*****************************************************************************************/
	void setRetry(string wifi, int value ){
	    string cmd = "iwconfig ";
	    cmd.append(wifi);
	    cmd.append(" retry ");
	    cmd.append(to_string(value));  
	    string iwconfig = GetStdoutFromCommand(cmd); // this command has no return
	    return;
	}

/*****************************************************************************************
* iwconfig interface modu: This command was unsupported. 
* Force the card to use a specific set of modulations. Modern cards support various      *
* modulations, some which are standard, such as 802.11b or 802.11g, and some proprietary.*
* This command force the card to only use the specific set of modulations listed on the  *
* command line. This can be used to fix interoperability issues.                         *
* The list of available modulations depend on the card/driver and can be displayed using *
* iwlist modulation. Note that some card/driver may not be able to select each modulation*
* listed independantly, some may come as a group. You may also set this parameter to auto*
* let the card/driver do its best.                                                       *
*****************************************************************************************/
/*****************************************************************************************
* iwconfig interface commit: This command was unsupported. 
* Some cards may not apply changes done through Wireless Extensions immediately (they may*
* wait to aggregate the changes or apply it only when the card is brought up via         *
* ifconfig). This command (when available) forces the card to apply all pending changes. *
* This is normally not needed, because the card will eventually apply the changes, but   *
* can be useful for debugging.                                                           *
*****************************************************************************************/
};
#endif 
