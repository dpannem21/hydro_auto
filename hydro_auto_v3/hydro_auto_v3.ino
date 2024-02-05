//DEBUGGING MECHANISMS=======================
bool debug = false;
inline void serial_debug_val(float value, bool newline = true){
  if(debug && newline) Serial.println(value);
  else if(debug) Serial.print(value);
}
inline void serial_debug_msg(String msg, bool newline = true){
  if(debug && newline) Serial.println(msg);
  else if(debug) Serial.print(msg);
}

//===========================================
//LIBRARIES==================================
  #include <TFT_HX8357.h>         //HiLetgo LCD screen for Arduino Mega2560
  
  #include <DHT.h>                //DHT Sensor Library, v1.4.4 by Adrafruit
  
  #include <OneWire.h>            //DallasTemperature, v3.9.0, by Miles Burton
  #include <DallasTemperature.h>  //OneWire, v2.3.7, by Paul Stoffregen

  #include <EEPROM.h>             //DFRobot TDS Sensor 
  #include <GravityTDS.h> 

  #include <SoftwareSerial.h>     //Set up communication between the ESP8266 and Arduino

  #include <avr/wdt.h> //watchdog timer


//===========================================
//PIN & SENSOR CONFIGURATIONS================

  //DHT 22 --> air temp. & humidity
      #define DHTPIN 6 //digital interrupt pin*
      #define DHTTYPE DHT22 //DHT 22 (AM302)
      DHT dht(DHTPIN,DHTTYPE);       //initialize DHT sensor for normal 16mhz arduino
      float air_humidity;
      float air_temp;
      
  //DS18B20 --> water temp.
      #define ONE_WIRE_BUS 7 //digital interrupt pin*
      OneWire oneWire(ONE_WIRE_BUS); //initialize DS1820 sensor
      DallasTemperature sensors(&oneWire);
      float water_temp;
  
  //DIGITEN --> Water Flow Rate Sensor
      #define samplingTime 80000 //(in microseconds)
      #define FLOWPIN 20 //digital interrupt pin*
      float flow_rate;
      volatile int pulse; //volatile needed since this variable is modified by an interrupt*
  
  //ANALOG SENSOR GENERAL SETTINGS
      #define samplingInterval 20000 //(in microseconds)
      #define samplingSize 40 
  
  //DFRobot --> pH Sensor
      #define pH_offset 0
      #define PHPIN 0 //analog pin**
      int pHArray[samplingSize];
      double pH_value;
      
  //DFRobot --> ORP Sensor
      #define ORPPIN 2 //analog pin**
      #define ORP_offset 0
      #define ORP_POWER_VOLTAGE 5.00
      int ORPArray[samplingSize];
      double ORP_value;
  
  //DFRobot --> TDS Sensor
      #define TDSPIN 4
      float TDS_value;
      GravityTDS tds;   
//===========================================
//LCD CONFIGURATIONS=========================
    #define font 2 //font size
    TFT_HX8357 tft = TFT_HX8357(); //initialize LCD
    String tft_message;
    String tft_unit;
//===========================================
//IoT CONFIGURATIONS=========================
    #define RX 10 //blue       --> HAVE to use these specific pins for software serial on Arduino Mega2560**
    #define TX 9 //orange 

    //WiFi information given by Interfaith*
    String AP;
    String PASS;
   
    String API;
    String HOST;
    String PORT;
    int countTrueCommand;
    int countTimeCommand; 
    boolean found = false; 
    SoftwareSerial esp8266(RX,TX); 
    int IoT_cycle_count = 0;
    bool IoT_enable = true;
    bool IoT_connected = true;
    int IoT_delay = 5000;
    String getData;
    String value_conversion;
    String CWJAP;
    String CWMODE;
    String AT;
    char* OK = "OK";
    String field;
    int IoT_reset_count = 0;

    String CIPMUX;
    String CIPSTART;
    String CIPSEND;
    String CIPCLOSE;
//===========================================
//ERROR LED==================================
    #define R  2
    #define G  3
    #define B  4
//===========================================

void setup() {
  Serial.begin(9600); //DHT,DS18B20,water flow
  //serial_debug_msg("Serial Config --> Completed");

  //STRING HANDLING==========================
  
    PORT.reserve(3);
    AT.reserve(3);
    
    field.reserve(10);    
    tft_unit.reserve(10);
    
    AP.reserve(15);   
    PASS.reserve(15);
    value_conversion.reserve(15);
    CIPMUX.reserve(15);
    CIPCLOSE.reserve(15);

    CWMODE.reserve(20);
    API.reserve(20);
    
    HOST.reserve(25);
    tft_message.reserve(25);

    CIPSEND.reserve(50);
    CIPSTART.reserve(75);
    CWJAP.reserve(75);
    if(!(getData.reserve(100))){
      //serial_debug_msg("Not enough memory");
    }

    AP   = "TC8715D1C-5G";       // AP NAME -----------------------------------------------------------------> WIFI NAME
    PASS = "TC8715D3EB01C"; // AP PASSWORD --------------------------------------------------------------> WIFI PASSWORD
    API  = "UFFV2Q1VYED10I2U";   // Write API KEY ---------------------------------------------------> THINGSPEAK API WRITE KEY
    HOST = "api.thingspeak.com";
    PORT = "80";
    CWJAP = "AT+CWJAP=\""; CWJAP += AP; CWJAP += "\",\""; CWJAP += PASS; CWJAP += "\"";
    CWMODE = "AT+CWMODE=1";
    AT = "AT";
  //=========================================

  //INTERNET OF THINGS (IoT)=================
   if(IoT_enable){
      esp8266.begin(9600);
      bool IoT_status1 = sendCommand(AT,2,OK);
      bool IoT_status2 = sendCommand(CWMODE,2,OK);
      bool IoT_status3 = sendCommand(CWJAP,2,OK);
      if(IoT_status1 && IoT_status2 && IoT_status3) IoT_connected = true;
      else{
        IoT_connected = false;
        IoT_enable = false; //turn off IoT if initial connection fails* prevent hangups
      }
      //serial_debug_msg("IoT Config --> Completed");
    }
  //=========================================
  
  //DIGITAL SENSORS==========================
     //DHT 22
      dht.begin();
      //serial_debug_msg("DHT22 Config --> Completed");
  
      //DS18B20
      sensors.begin();
      //serial_debug_msg("DS182B20 Config --> Completed");
      
    //Water Flow Sensor
    pinMode(FLOWPIN, INPUT);
    //serial_debug_msg("DIGITEN Water Hall Effet Flow Sensor Config --> Completed");
    
  //=========================================
  //ANALOG SENSORS===========================
    
    //TDS Sensor
    tds.setPin(TDSPIN);
    tds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
    tds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    tds.begin();  //initialization
    //serial_debug_msg("DFRobot TDS Sesnor Config --> Completed");
    
  //=========================================
  //LIGHT SCREEN DISPLAY (LCD)===============
    tft.init();
    tft.setRotation(2); //set text rotation to be 90 degrees*
    //serial_debug_msg("LCD Config --> Completed");
  //=========================================
  //ERROR LED================================
    pinMode(R, OUTPUT);
    pinMode(G, OUTPUT);
    pinMode(B, OUTPUT);
  //=========================================
}

void loop() {
  bool LCD_error = display_header();
  //Air Temperature and Humidity------------------------------------------
    air_humidity = dht.readHumidity();
    air_temp = (((dht.readTemperature()* 9.0) /5.0) + 32.0);
    
      //serial_debug_msg("air humidity --> ",false);
      //serial_debug_val(air_humidity);
      //serial_debug_msg("air temp --> ", false);
      //serial_debug_val(air_temp);

    tft_message = "Air Humidity"; tft_unit = "%";
    bool airhumidity_error = display_sensor(air_humidity, tft_message, tft_unit);
    tft_message = "Air Temp"; tft_unit = "F";
    bool airtemp_error = display_sensor(air_temp, tft_message, tft_unit);
    
  //------------------------------------------------------------------------
  //Water Temperature-------------------------------------------------------
    sensors.requestTemperatures(); //send command to poll the water temperature
    water_temp = ( ((sensors.getTempCByIndex(0) * 9.0) /5.0) + 32.0);
    
      //serial_debug_msg("water temp --> ",false);
      //serial_debug_val(water_temp);
      
    tft_message = "Water Temp"; tft_unit = "F";
    bool watertemp_error = display_sensor(water_temp, tft_message, tft_unit);
    
  //------------------------------------------------------------------------ 
  //Water Flow Rate---------------------------------------------------------
    flow_rate = read_flow(samplingTime);
    
     // serial_debug_msg("water flow rate --> ",false);
     // serial_debug_val(flow_rate);

    tft_message = "Flow Rate"; tft_unit = "mL/sec";
    bool flowrate_error = display_sensor(flow_rate, tft_message, tft_unit);
    
  //------------------------------------------------------------------------ 
  //pH----------------------------------------------------------------------
    pH_value = read_pH(pHArray,samplingSize);
    
     // serial_debug_msg("water pH --> ",false);
     // serial_debug_val(pH_value);

    tft_message = "pH"; tft_unit = "";
    bool pH_error = display_sensor(pH_value, tft_message, tft_unit);
    
  //------------------------------------------------------------------------
  //ORP---------------------------------------------------------------------
    ORP_value = read_ORP(ORPArray,samplingSize);
    
     // serial_debug_msg("water ORP --> ",false);
     // serial_debug_val(ORP_value);

    tft_message = "ORP"; tft_unit = "";
    bool ORP_error = display_sensor(ORP_value, tft_message, tft_unit);
    
  //------------------------------------------------------------------------
  //TDS--------------------------------------------------------------------
    tds.setTemperature((water_temp-32)*(5.0/9.0)); //water temperature in celcius* --> use water_temp when integrated*
    tds.update();
    TDS_value = tds.getTdsValue();
      //serial_debug_msg("water TDS --> ",false);
      //serial_debug_val(TDS_value);

    tft_message = "TDS"; tft_unit = "ppm";
    bool TDS_error = display_sensor(TDS_value,tft_message, tft_unit);  
  //-----------------------------------------------------------------------

    //serial_debug_msg("Sensor Collection Completed");
    
    display_IoT((IoT_connected), IoT_enable);
        
    if(IoT_enable){
     if(IoT_connected == true){
        bool send1,send2,send3,send4,send5,send6,send7,send8;
        
        if(!(airhumidity_error) && (IoT_cycle_count == 0)){
           tft_message = "Air Humidity --> IoT";
           tft.println(tft_message);
           field = "field1";
           send1 = sendData_IoT(air_humidity,field);
           
        }
        else if(IoT_cycle_count == 0){
          tft_message = "Air Humidity --X";
          tft.println(tft_message);
        }
        
        if(!(airtemp_error) && (IoT_cycle_count == 1)){
          tft_message = "Air Temp --> IoT";
          tft.println(tft_message);
          field = "field2";
          send2 = sendData_IoT(air_temp,field);
          
        }
        else if(IoT_cycle_count == 1){
          tft_message = "Air Temp --X";
          tft.println(tft_message);
        }
        
        if(!(watertemp_error) && (IoT_cycle_count == 2)){
          tft_message = "Water Temp --> IoT";
          tft.println(tft_message);
          field = "field3";
          send3 = sendData_IoT(water_temp,field);
          
        }
        else if(IoT_cycle_count == 2){
          tft_message = "Water Temp --X";
          tft.println(tft_message);
        }
        
        if(!(flowrate_error) && (IoT_cycle_count == 3)){
          tft_message = "Flow Rate --> IoT";
          tft.println(tft_message);
          field = "field4";
          send4 = sendData_IoT(flow_rate, field);
          
        }
        else if(IoT_cycle_count == 3){
          tft_message = "Flow Rate --X";
          tft.println(tft_message);
        }
        
        if(!(pH_error) && (IoT_cycle_count == 4)){
          tft_message = "pH --> IoT";
          tft.println(tft_message);
          field = "field5";
          send5 = sendData_IoT(pH_value, field);
          
        }
        else if(IoT_cycle_count == 4){
          tft_message = "pH --X";
          tft.println(tft_message);
        }
        
        if(!(ORP_error) && (IoT_cycle_count == 5)){
          tft_message = "ORP --> IoT";
          tft.println(tft_message);
          field = "field6";
          send6 = sendData_IoT(ORP_value, field);
          
        }
        else if(IoT_cycle_count == 5){
          tft_message = "ORP --X";
          tft.println(tft_message);
        }
        
        if(!(TDS_error) && (IoT_cycle_count == 6)){
          tft_message = "TDS --> IoT";
          tft.println(tft_message);
          field = "field7";
          send7 = sendData_IoT(TDS_value, field);
          
        }
        else if(IoT_cycle_count == 6){
          tft_message = "TDS --X";
          tft.println(tft_message);
        }

        
        if(send1 || send2 || send3 || send4 || send5 || send6 || send7 || send8) IoT_connected = true;
        else{
          IoT_connected = false;
        }
      }
    }

    //serial_debug_msg("IoT Collection Completed");
    
    if( (LCD_error) && (!IoT_connected) && 
      (airhumidity_error && airtemp_error && watertemp_error && flowrate_error && pH_error && ORP_error && TDS_error) ){
        setColor(255,0,0); //RED means very bad --> nothing working 
        //serial_debug_msg("RED");
    }
    else if ((LCD_error) && (!IoT_connected) && 
            (airhumidity_error || airtemp_error || watertemp_error || flowrate_error || pH_error || ORP_error || TDS_error)){
        setColor(255,165,0);
        //serial_debug_msg("ORANGE");
      }
    else if( ((LCD_error) || (!IoT_connected)) && 
           (airhumidity_error || airtemp_error || watertemp_error || flowrate_error || pH_error || ORP_error || TDS_error)){
        setColor(255,206,177);
        //serial_debug_msg("BLUE");
      }
    else{
        setColor(0,255,0);
        //serial_debug_msg("GREEN");
      }

    IoT_cycle_count++;
    if(IoT_cycle_count > 6){
      //Enable the watchdog timer
      wdt_enable(WDTO_2S);
      //serial_debug_msg("Watchdog Triggered");
      while(true){
      }
      //trigger the watchdog timer to reset
      IoT_cycle_count = 0;
    } 

    //serial_debug_val(IoT_cycle_count);
    
    delay(10000);
 }

//CUSTOM FUNCTIONS===========================
  //LCD--------------------------------------
    bool display_header(void){
      size_t write_count = 0;
      // Fill screen with random colour so we can see the effect of printing with and without 
      // a background colour defined
      tft.fillScreen(TFT_BLACK);
      
      // Set "cursor" at top left corner of display (0,0) and select font 2
      // (cursor will move to next line automatically during printing with 'tft.println'
      //  or stay on the line is there is room for the text with tft.print)
      tft.setCursor(0, 0, 2);
      // Set the font colour to be white with a black background, set text size multiplier to 1
      tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(font);
      // We can now plot text on screen using the "print" class
      write_count += tft.println("Hydroponic Vitals");
      tft.println("");
      
      return (write_count > 0);
    }
    
    bool display_sensor(float val, const String& vital, const String& unit){
      bool error = false;

      if(val < 0 || isnan(val) || val == DEVICE_DISCONNECTED_C){
        error = true;
      }
      
      tft.setTextColor(TFT_WHITE); tft.setTextFont(font);
      tft.print(vital);
      tft.print(": ");
      if(error == false) tft.print(val);
      else tft.print(" --- ");
      tft.print(" ");
      tft.println(unit);

      return error;
    }
    
    void display_IoT(bool IoT_status, bool enable){
       if(enable){
         tft.println("");
         tft.setTextColor(TFT_WHITE); tft.setTextFont(font);
         tft.print("IoT: ");
         if(IoT_status == true) tft.println("Connected");
         else tft.println("Not Connected");
       }
       else{
         tft.println("");
         tft.setTextColor(TFT_WHITE); tft.setTextFont(font);
         tft.print("IoT: ");
         tft.println("*Not Enabled*");
       }
    }
  //-----------------------------------------
  //DIGITAL----------------------------------
    float read_flow(int sample_time){
      //calibration factor --> The hall-effect flow sensor outputs 
      //approximately 23 pulses per second per litre/minute of flow.
      //as specified by DIGITEN on the product description
      
      float calibrationFactor = 23;
      //reset pulse counter to 0
      pulse = 0; 
      //attach the interrupt
      attachInterrupt(digitalPinToInterrupt(FLOWPIN),pulseCounter,FALLING);
      //start the delay
      delayMicroseconds(sample_time);
      //after the delay
      float rate = ((pulse * calibrationFactor) * 1000)/(((sample_time/1000.0)/60.0)) ;
      //1) convert sample time to milliseconds ( /1000.0)
      //2) convert to seconds ( /60.0)
      //3) convert to mL ( * 1000.0)
      //4) apply the calibration factor ( / calibration factor)
      
      //detatch the interrupt before returning from the function
      detachInterrupt(digitalPinToInterrupt(FLOWPIN));
      return rate;
    }
    
    void pulseCounter(){
      pulse++;
    }
  //-----------------------------------------
  //ANALOG-----------------------------------
    double averagearray(int* arr, int number){
      int i;
      int max,min;
      double avg;
      long amount=0;
      if(number<=0){
        Serial.println("Error number for the array to avraging!/n");
        return 0;
      }
      if(number<5){   //less than 5, calculated directly statistics
        for(i=0;i<number;i++){
          amount+=arr[i];
        }
        avg = amount/number;
        return avg;
      }else{
        if(arr[0]<arr[1]){
          min = arr[0];max=arr[1];
        }
        else{
          min=arr[1];max=arr[0];
        }
        for(i=2;i<number;i++){
          if(arr[i]<min){
            amount+=min;        //arr<min
            min=arr[i];
          }else {
            if(arr[i]>max){
              amount+=max;    //arr>max
              max=arr[i];
            }else{
              amount+=arr[i]; //min<=arr<=max
            }
          }//if
        }//for
        avg = (double)amount/(number-2);
      }//if
      return avg;
    }
    
    double read_ORP(int *arr, int arr_length){
      int index = 0;
      double ORP;
      while(index < arr_length){
        arr[index++] = analogRead(ORPPIN);
        delayMicroseconds(samplingInterval); 
      }
      ORP = ((30*(double)ORP_POWER_VOLTAGE*1000)
              -(75*averagearray(arr, arr_length)
              *ORP_POWER_VOLTAGE*1000/1024))/75-ORP_offset;
    
      return ORP;
    }
  
    double read_pH(int *arr, int arr_length){ 
        int index = 0;
        double voltage, pH;
        while(index < arr_length){
          arr[index++] = analogRead(PHPIN);
          delayMicroseconds(samplingInterval); 
        }
        voltage = averagearray(arr,arr_length)*5.0/1024;
        pH = (3.5 * voltage) + pH_offset;
        return pH;
      }
  //-----------------------------------------
  //INTERNET OF THINGS-----------------------
   bool sendData_IoT(float val, String& field){
     value_conversion = "";
     value_conversion.concat(val);
     getData = "GET /update?api_key="; getData +=API; getData +="&"; getData +=field; getData+="="; getData+=value_conversion;
     //serial_debug_msg(getData);

     CIPMUX = "AT+CIPMUX=1";
     bool b1 = sendCommand("AT+CIPMUX=1",5,OK);

     CIPSTART = "AT+CIPSTART=0,\"TCP\",\""; CIPSTART+= HOST; CIPSTART +="\","; CIPSTART += PORT;
     bool b2 = sendCommand(CIPSTART,15,OK);

     CIPSEND = "AT+CIPSEND=0,";
     CIPSEND.concat(getData.length()+4);
     bool b3 = sendCommand(CIPSEND,4,">");

     esp8266.println(getData);delay(IoT_delay);countTrueCommand++;
     CIPCLOSE = "AT+CIPCLOSE=0";
     sendCommand(CIPCLOSE,5,OK);
     
     if(b1 || b2 || b3) return true;
     else return false;
   }
    bool sendCommand(const String& command, int maxTime, char readReplay[]) {
      found = false;
     
      //Serial.print(countTrueCommand);
      //Serial.print(". at command => ");
      //Serial.print(command);
      //Serial.print(" ");      
      while(countTimeCommand < (maxTime*1))
      {
        esp8266.println(command);//at+cipsend
        if(esp8266.find(readReplay))//ok
        {
          found = true;
          break;
        }
      
        countTimeCommand++;
        
      }
      
      if(found == true)
      {
        //Serial.println("OYI");
        countTrueCommand++;
        countTimeCommand = 0;
      }
      
      if(found == false)
      {
        //Serial.println("Fail");
        countTrueCommand = 0;
        countTimeCommand = 0;
      }
      
      return found;
   }
  //-----------------------------------------
  //Error LED--------------------------------
  //RGB Error-LED
    void setColor(int redValue, int greenValue, int blueValue) {
      analogWrite(R, redValue);
      analogWrite(G, greenValue);
      analogWrite(B, blueValue);
    }
  //-----------------------------------------
//===========================================
