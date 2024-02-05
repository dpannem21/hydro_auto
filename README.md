**Project Background**

My team from Engineers Without Borders (EWB) started a joint project with Interfaith Food Shuttle to implement a hydroponics system inside one of their greenhouses. Interfaith Food Shuttle’s (IFS) mission is to help provide food to those who deal with food insecurity throughout North Carolina, including the Raleigh-Wake area. The IFS team here has been considering implementing a hydroponics system to explore more sustainable and optimized methods of farming. I am one of the project leads on the EWB team and one aspect we have to consider is monitoring post-implementation. However, this tends to be impractical for a couple of reasons. Project teams are constantly shifting with EWB members graduating or leaving the organization and expertise on projects isn’t easily transferable to new members through just documentation, which tend to have gaps or are hard to interpret without the original authors. Another issue is, IFS should be equipped with the appropriate tools to keep a more routine track of the hydroponics system and not rely on EWB. Since this particular system is responsible for maintaining a steady and stable environment to grow food, it is even more important that IFS can independently manage the hydroponics system. This project hopes to tackle this problem by using an embedded systems architecture specifically designed for monitoring the vitals and “health” of the hydroponics system.

**Technical Requirements**

  1) Weather-proof & rigid enclosure
  2) Voltage regulators & proper power distribution
  3) Display that will show current live-readings
  4) External memory that can hold data logs 
  5) Error-Indicator LED array
  6) Components don’t require long-term maintenance 
  7) Components are easily replaceable
  8) Tracks enough vitals to provide useful insights

**Vitals and Thresholds**

The typical vitals that are monitored in hydroponic systems are listed below along with the thresholds that each vital should be within and the insights that each vital provides. The two vitals that will NOT be tracked with a sensor are ‘EC’ and ‘Dissolved Oxygen’. This is primarily because these sensors are expensive and require constant maintenance to provide useful data. EC, pH, and TDS monitoring both provide insights into how well the plant roots can absorb nutrients and concentration of nutrients and since we will have a pH and TDS sensor, it is ok to not have a measurement of EC. Dissolved Oxygen and ORP both provide insights into the concentration of nutrients in the water, so it is ok to not have a measurement of dissolved oxygen since we are tracking ORP.

| Vital            | Threshold                        | Description                                                                                                                                                                                                                                                                                                                                                                                           |
|------------------|----------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Air Temp.        | 62-72o F                         | These two vitals are common across all plants and growing methodologies. We can’t control these as easily as the other vitals but they are important to record and log.                                                                                                                                                                                                                               |
| Humidity         | ~50%                             |                                                                                                                                                                                                                                                                                                                                                                                                       |
| Water Temp.      | 65-80o F                         | Since hydroponics grows plants through feeding nutrients via water delivery, it’s important that the water temperature is monitored.                                                                                                                                                                                                                                                                  |
| Water Flow Rate  | .3-.5 L/min                      | A steady water flow rate is important in keeping the mechanics of the system running smoothly but also providing enough time to the roots to absorb nutrients from the solution                                                                                                                                                                                                                       |
| pH               | 5.8-6.2                          | The pH indicates the acidity of the solution which affects the roots ability to absorb nutrients.                                                                                                                                                                                                                                                                                                     |
| EC               | 1.5-2.5 dS/cm                    | Most plant nutrients come in the form of soluble ionic (charged) compounds, so the EC value is directly correlated to the concentration of nutrients in the solution. A high EC can prevent the roots from absorbing nutrients due increased osmotic pressure and a low EC can result in a decreased yield.                                                                                           |
| ORP              | 300-500 mV                       | The ORP value will give insights into the concentration of oxidants in the water-nutrient solution. Oxidants are helpful in killing pathogens and keeping water sterile. The ORP can be adjusted through adding things such as hydrogen peroxide to the solution. The ORP values can be used to make sure the right amount is being added and the chemical environment isn’t too harsh for the roots. |
| Dissolved Oxygen | ≥ 5 mg/L                         | It’s important to maintain a high dissolved oxygen concentration in the solution because the roots are submerged in water versus soil                                                                                                                                                                                                                                                                 |
| TDS              | 600-1000 PPM (parts per million) | TDS gives you a clear idea of the strength or concentration of your nutrient solution. You can get accurate information regarding the amount of nutrients that your plants are getting from the water.                                                                                                                                                                                                |

**Design and Architecture**

Software Logical Flow

![image](https://github.com/dpannem21/hydro_auto/assets/95881563/4d6279c7-1164-4de5-86f3-a23c47e7cdd5)

Hardware Schematic

![image](https://github.com/dpannem21/hydro_auto/assets/95881563/d967ae41-c31a-4133-839c-dadc9d89c2c8)

**Final Implementation**

Embedded System 

![image](https://github.com/dpannem21/hydro_auto/assets/95881563/bf4b3c8f-219f-496e-a7f7-360b511c1c2c)

IoT Dashboard - ThingSpeak

![image](https://github.com/dpannem21/hydro_auto/assets/95881563/8e5b181e-adcb-4bf7-8060-8ae1479b6073)

Testing it at IFS's greenhouse on a prototype hydroponic system
![image](https://github.com/dpannem21/hydro_auto/assets/95881563/8b3f4dd7-cd69-4b63-b414-392eee3c47d4)

**B.O.M (Bill of Materials)**
| Item                                                            | Price  | Quantity | Link    |
|-----------------------------------------------------------------|--------|----------|---------|
| Arduino Mega 2560 (mcu)                                         | $47.99 | 2        | Amazon  |
| Heat Sink Kit                                                   | $13.99 | 1        | Amazon  |
| Breadboard Jumper Wires Kit  11.8 in / 30cm                     | $8.99  | 1        | Amazon  |
| RGB LED & Resistor Kit                                          | $8.99  | 1        | Amazon  |
| Perfboard Kit                                                   | $13.49 | 1        | Amazon  |
| Pin Header Kit                                                  | $12.99 | 1        | Amazon  |
| LCD Display                                                     | $19.99 | 2        | Amazon  |
| ESP8266 IoT Module (3 pcs)                                      | $8.99  | 1        | Amazon  |
| Air Temperature & Humidity Sensor (2 pc)                        | $12.99 | 1        | Amazon  |
| Water Temperature Sensor (5 pc)                                 | $11.58 | 1        | Amazon  |
| pH sensor                                                       | $56.90 | 1        | DFRobot |
| ORP sensor                                                      | $89.00 | 1        | DFRobot |
| TDS sensor                                                      | $11.80 | 1        | DFRobot |
| Water Flow Rate sensor                                          | $9.99  | 1        | Amazon  |
| Weather-proof electrical case  8.7 x 6.7 x 4.3 Clear case model | $22.99 | 1        | Amazon  |
| Power Supply Module 12V 2A (2 pc)                               | $11.75 | 1        | Amazon  |
| Power Supply Module 5V 2A                                       | $9.99  | 1        | Amazon  |
| Surge Protector Power Strip 3 outlets, 15ft                     | $16.66 | 1        | Amazon  |
| TOTAL: (pre-tax & shipping)                                     | ~ $450 |          |         |


I would like to thank NCSU's honors college for funding this project and for all the people at EWB and IFS who helped with the design and thought that went into this project :) 

![image](https://github.com/dpannem21/hydro_auto/assets/95881563/4ef8c580-08f4-4969-a729-1ad0320cc97e) ![image](https://github.com/dpannem21/hydro_auto/assets/95881563/d022745a-9b52-4b0e-a498-3b812a585784)
![image](https://github.com/dpannem21/hydro_auto/assets/95881563/c02b9603-72ff-4f46-98b5-eb78fa70b1f1)





